#include <gtk/gtk.h>
#include <libintl.h>
#include <locale.h>
#include <libconfig.h>
#include <chrono>
#include <vector>
//#include <iostream>
//#include <sstream>
#include <numeric>
//#include <utility> 
#include <string>
#include <algorithm>
#include <set>
#include <map>


std::chrono::steady_clock::time_point start_time;
std::vector<std::pair<std::string, double>> word_times;

#define _(STRING) gettext(STRING)

// Globale Variable zur Speicherung des Textfelds
static GtkTextView *global_text_view = NULL;
static GtkLabel *global_label = NULL;
static GtkColorButton *global_labelBackgroundColor = NULL;
static GtkColorButton *global_labelForgroudColor = NULL;
static GtkFontButton *global_labelTextButton = NULL;
static GtkButton *global_button_previous = NULL;
static GtkButton *global_button_next = NULL;
static GtkButton *global_button_read = NULL;
static GtkLabel *global_ProgressLabel = NULL;
static GtkProgressBar *global_ProgressBar = NULL;
static GtkSwitch *global_labelProgressSwitch = NULL;
static GtkSpinButton *global_labelWortsPerTimeSpinn = NULL;
static GtkSwitch *global_TimeToNextWordSwitch = NULL;
static GtkSpinButton *global_TimeToNextWordSpinn = NULL;
static GtkButton *global_top_right_button=NULL;
static GtkSwitch *global_StatisticsSwitch = NULL;
static GtkButton *global_results_button = NULL;

static gchar *config_file_path = NULL;


// Globale Variablen zur Verwaltung des Texts und der Wörter
static gchar **words = NULL;
static int current_word_index = 0;
static int total_words = 0;
static gboolean timebased_next_word = false;
static gboolean make_statistics = false;
static guint timer_id;

static void save_settings() {
  config_t cfg;
  config_init(&cfg);

  config_setting_t *root = config_root_setting(&cfg);

  GdkRGBA bg_color;
  gtk_color_chooser_get_rgba(GTK_COLOR_CHOOSER(global_labelBackgroundColor), &bg_color);
  config_setting_t *bg_color_setting = config_setting_add(root, "background_color", CONFIG_TYPE_STRING);
  config_setting_set_string(bg_color_setting, gdk_rgba_to_string(&bg_color));

  GdkRGBA fg_color;
  gtk_color_chooser_get_rgba(GTK_COLOR_CHOOSER(global_labelForgroudColor), &fg_color);
  config_setting_t *fg_color_setting = config_setting_add(root, "foreground_color", CONFIG_TYPE_STRING);
  config_setting_set_string(fg_color_setting, gdk_rgba_to_string(&fg_color));

  const char *font = gtk_font_chooser_get_font(GTK_FONT_CHOOSER(global_labelTextButton));
  config_setting_t *font_setting = config_setting_add(root, "font", CONFIG_TYPE_STRING);
  config_setting_set_string(font_setting, font);

  gboolean show_progress = gtk_switch_get_active(global_labelProgressSwitch);
  config_setting_t *show_progress_setting = config_setting_add(root, "show_progress", CONFIG_TYPE_BOOL);
  config_setting_set_bool(show_progress_setting, show_progress);

  int words_per_time = gtk_spin_button_get_value_as_int(global_labelWortsPerTimeSpinn);
  config_setting_t *words_per_time_setting = config_setting_add(root, "words_per_time", CONFIG_TYPE_INT);
  config_setting_set_int(words_per_time_setting, words_per_time);


  GtkTextBuffer *buffer = gtk_text_view_get_buffer(global_text_view);

  GtkTextIter start, end;
  gtk_text_buffer_get_start_iter(buffer, &start);
  gtk_text_buffer_get_end_iter(buffer, &end);

  char *text = gtk_text_buffer_get_text(buffer, &start, &end, FALSE);
  config_setting_t *text_setting = config_setting_add(root, "text", CONFIG_TYPE_STRING);
  config_setting_set_string(text_setting, text);

  gboolean TimeToNextWord = gtk_switch_get_active(global_TimeToNextWordSwitch);
  config_setting_t *TimeToNextWord_setting = config_setting_add(root, "time_based_next_word", CONFIG_TYPE_BOOL);
  config_setting_set_bool(TimeToNextWord_setting, TimeToNextWord);

  double time_based_next_word_time = gtk_spin_button_get_value(global_TimeToNextWordSpinn);
  config_setting_t *time_based_next_word_time_setting = config_setting_add(root, "time_based_next_word_time", CONFIG_TYPE_FLOAT);
  config_setting_set_float(time_based_next_word_time_setting, time_based_next_word_time);
    
  gboolean make_statistics = gtk_switch_get_active(global_StatisticsSwitch);
  config_setting_t *make_statistics_setting = config_setting_add(root, "make_statistics", CONFIG_TYPE_BOOL);
  config_setting_set_bool(make_statistics_setting, make_statistics);


  if (!config_write_file(&cfg, config_file_path)) {
    fprintf(stderr, _("Error while writing file.\n"));
  }
  

  config_destroy(&cfg);
}
static void load_settings() {
  config_t cfg;
  config_init(&cfg);

  if (!config_read_file(&cfg, config_file_path)) {
    fprintf(stderr, _("Error while reading file: %s:%d - %s\n"),
            config_error_file(&cfg),
            config_error_line(&cfg),
            config_error_text(&cfg));
    fprintf(stderr, _("Trye to corrct"));
    save_settings();
    config_destroy(&cfg);
    return;
  }

  const char *bg_color_str;
  if (config_lookup_string(&cfg, "background_color", &bg_color_str)) {
    GdkRGBA bg_color;
    if (gdk_rgba_parse(&bg_color, bg_color_str)) {
      gtk_color_chooser_set_rgba(GTK_COLOR_CHOOSER(global_labelBackgroundColor), &bg_color);
    }
  }


  const char *fg_color_str;
  if (config_lookup_string(&cfg, "foreground_color", &fg_color_str)) {
    GdkRGBA fg_color;
    if (gdk_rgba_parse(&fg_color, fg_color_str)) {
      gtk_color_chooser_set_rgba(GTK_COLOR_CHOOSER(global_labelForgroudColor), &fg_color);
    }
  }


  const char *font_str;
  if (config_lookup_string(&cfg, "font", &font_str)) {

    // Aktuell eingestellte Schriftart speichern
    char *previous_font = gtk_font_chooser_get_font(GTK_FONT_CHOOSER(global_labelTextButton));

    PangoFontDescription *font_desc = pango_font_description_from_string(font_str);

    if (font_desc != NULL) {
      // Versuche die Schriftart auf die Komponente anzuwenden
      gtk_font_chooser_set_font(GTK_FONT_CHOOSER(global_labelTextButton), font_str);

      // Überprüfe, ob die Schriftart erfolgreich gesetzt wurde
      char *current_font = gtk_font_chooser_get_font(GTK_FONT_CHOOSER(global_labelTextButton));
      if (g_strcmp0(current_font, font_str) != 0) {
        // Setze die vorherige Schriftart wieder ein
        gtk_font_chooser_set_font(GTK_FONT_CHOOSER(global_labelTextButton), previous_font);
      }
      g_free(current_font);
      pango_font_description_free(font_desc);
    } else {
      // Falls die Font-Beschreibung fehlschlägt, setze die vorherige Schriftart wieder ein
      gtk_font_chooser_set_font(GTK_FONT_CHOOSER(global_labelTextButton), previous_font);
    }

    // Speicher freigeben
    g_free(previous_font);
  }



  int show_progress;
  if (config_lookup_bool(&cfg, "show_progress", &show_progress)) {
    gtk_switch_set_active(global_labelProgressSwitch, show_progress);
  }

  int words_per_time;
  if (config_lookup_int(&cfg, "words_per_time", &words_per_time)) {
    if (words_per_time > 0) {
      gtk_spin_button_set_value(global_labelWortsPerTimeSpinn, words_per_time);
    }
  }
  int TimeToNextWord;
  if (config_lookup_bool(&cfg, "time_based_next_word", &TimeToNextWord)) {
    gtk_switch_set_active(global_TimeToNextWordSwitch, TimeToNextWord);
  }

  double time_based_next_word_time;
  if (config_lookup_float(&cfg, "time_based_next_word_time", &time_based_next_word_time)) {
    if (words_per_time > 0) {
      gtk_spin_button_set_value(global_TimeToNextWordSpinn, time_based_next_word_time);
    }
  }


  const char *text_str;
  if (config_lookup_string(&cfg, "text", &text_str)) {
    GtkTextBuffer *buffer = gtk_text_view_get_buffer(global_text_view);
    gtk_text_buffer_set_text(buffer, text_str, -1);

  }
  

   int make_statistics;
  if (config_lookup_bool(&cfg, "make_statistics", &make_statistics)) {
    gtk_switch_set_active(global_StatisticsSwitch, make_statistics);
  }

  config_destroy(&cfg);
}

static void update_button_and_Lable_states() {
  int words_per_time = gtk_spin_button_get_value_as_int(global_labelWortsPerTimeSpinn);

  if (current_word_index <= words_per_time) {
    gtk_widget_set_sensitive(GTK_WIDGET(global_button_previous), FALSE);
  } else {
    gtk_widget_set_sensitive(GTK_WIDGET(global_button_previous), TRUE);
  }
  
  if (current_word_index >= total_words) {
    gtk_widget_set_sensitive(GTK_WIDGET(global_button_next), FALSE);
  } else {
    gtk_widget_set_sensitive(GTK_WIDGET(global_button_next), TRUE);
   
  }
  
    if (current_word_index >= total_words-1) 
    gtk_widget_hide(GTK_WIDGET(global_top_right_button));
  
  
  if (current_word_index >= total_words && make_statistics)
      gtk_widget_show(GTK_WIDGET(global_results_button));
   else     
      gtk_widget_hide(GTK_WIDGET(global_results_button));
  
  
  
  int fake_total_words=total_words;
  int fake_current_word_index=current_word_index;
  if(make_statistics){
    fake_total_words--;
    fake_current_word_index=MIN(fake_total_words,current_word_index);
    
    }
 
  int progress_index = (fake_current_word_index + words_per_time - 1) / words_per_time;
  int total_progress_steps = (fake_total_words + words_per_time - 1) / words_per_time;
  if (words_per_time > 1)
    gtk_label_set_text(global_ProgressLabel, g_strdup_printf("%d/%d - %d/%d", progress_index, total_progress_steps, fake_current_word_index, fake_total_words));
  else
    gtk_label_set_text(global_ProgressLabel, g_strdup_printf("%d/%d", progress_index, total_progress_steps));
  double progress = (double)progress_index / total_progress_steps;
  gtk_progress_bar_set_fraction(global_ProgressBar, progress);
}

// Funktion zum Überprüfen des Textfelds und zum Aktualisieren des "Lesen"-Buttons
static void update_read_button_state(GtkTextBuffer *buffer, gpointer user_data) {
  GtkTextIter start, end;
  gtk_text_buffer_get_start_iter(buffer, &start);
  gtk_text_buffer_get_end_iter(buffer, &end);

  if (gtk_text_iter_equal(&start, &end)) {
    gtk_widget_set_sensitive(GTK_WIDGET(global_button_read), FALSE);
  } else {
    gtk_widget_set_sensitive(GTK_WIDGET(global_button_read), TRUE);
  }
}

static void append_word(gchar ***words2, int *total_words2, const gchar *new_word) {
    *words2 = (gchar **)g_realloc(*words2, (*total_words2 + 1) * sizeof(gchar *));
    (*words2)[*total_words2] = g_strdup(new_word);
    (*total_words2)++;
}

// Funktion zum Aufteilen eines Textes in Wörter
static void split_text_into_words(const gchar *text) {
  
  //g_strfreev(words);  // Alte Wörter freigeben, falls vorhanden
  words=NULL;

  // Text in Wörter aufteilen, wobei Leerzeichen und neue Zeilen als Trennzeichen verwendet werden
  words = g_regex_split_simple("[ \n]+", text, (GRegexCompileFlags) 0, (GRegexMatchFlags) 0);
  
  current_word_index = 0;
  total_words = g_strv_length(words);
  

  // Leere Wörter am Ende entfernen
  if (total_words > 0 && g_strcmp0(words[total_words - 1], "") == 0) {
    g_free(words[total_words - 1]);
    words[total_words - 1] = NULL;
    total_words--;
  }

  // Leere Wörter am Anfang entfernen
  if (total_words > 0 && g_strcmp0(words[0], "") == 0) {
    g_free(words[0]);
    for (int i = 1; i < total_words; i++) {
      words[i - 1] = words[i];
    }
    words[total_words - 1] = NULL;
    total_words--;
  }
  
  if(make_statistics)
    append_word(&words, &total_words, _("Statistiken:"));
 
  update_button_and_Lable_states();
 
}

// Funktion zum Aktualisieren des Labels mit den nächsten Wörtern
static void update_label_with_next_word() {
    if(make_statistics&&current_word_index < total_words){
    auto end_time = std::chrono::steady_clock::now();  // Endzeit erfassen
    std::chrono::duration<double> elapsed_seconds = end_time - start_time;
    if (current_word_index > 0) {
        word_times.push_back({words[current_word_index - 1], elapsed_seconds.count()});  // Zeitdifferenz und Wort speichern
    }
    }   
    if (words != NULL && current_word_index < total_words) {
        int words_per_time = gtk_spin_button_get_value_as_int(global_labelWortsPerTimeSpinn);

        GString *next_words = g_string_new(NULL);

        for (int i = 0; i < words_per_time && current_word_index < total_words; i++) {
            if (g_strcmp0(words[current_word_index], "") != 0) {
                g_string_append(next_words, words[current_word_index]);
                if (i < words_per_time - 1 && current_word_index < total_words - 1) {
                    g_string_append(next_words, " ");
                }
            }
            current_word_index++;
        }

        gtk_label_set_text(global_label, next_words->str);
        g_string_free(next_words, TRUE);
        update_button_and_Lable_states();
    }
    
    if(make_statistics&&current_word_index < total_words)
    start_time = std::chrono::steady_clock::now();  // Startzeit erfassen

}


static void update_label_with_previous_word() {
  if (words != NULL && current_word_index > 1) {
    int words_per_time = gtk_spin_button_get_value_as_int(global_labelWortsPerTimeSpinn);
    current_word_index -= 2 * words_per_time;
    if (current_word_index < 0) {
      current_word_index = 0;
    }

    GString *prev_words = g_string_new(NULL);

    for (int i = 0; i < words_per_time && current_word_index < total_words; i++) {
      if (g_strcmp0(words[current_word_index], "") != 0) {
        g_string_append(prev_words, words[current_word_index]);
        if (i < words_per_time - 1 && current_word_index < total_words - 1) {
          g_string_append(prev_words, " ");
        }
      }
      current_word_index++;
    }

    gtk_label_set_text(global_label, prev_words->str);
    g_string_free(prev_words, TRUE);
    update_button_and_Lable_states();
  }
  if(make_statistics&&current_word_index < total_words)
    start_time = std::chrono::steady_clock::now();  // Startzeit erfassen
}

double calculate_score(const std::string& word, double time) {
    return word.length() / time;
}

double calculate_total_time(const std::vector<std::pair<std::string, double>>& word_times) {
    return std::accumulate(word_times.begin(), word_times.end(), 0.0,
                           [](double sum, const std::pair<std::string, double>& word_time) {
                               return sum + word_time.second;
                           });
}

static std::vector<std::pair<double, int>> scores;
static GtkWidget *grid;
static std::map<int, GtkWidget*> word_labels;
static std::map<int, GtkWidget*> time_labels;
static std::map<int, GtkWidget*> relative_time_labels;
static std::map<int, GtkWidget*> score_labels;
// Identifiziere die drei niedrigsten Scores
std::set<int> lowest_indices;
static GdkRGBA result_fg_color;

static void update_colors(GtkSwitch *widget, gpointer data) {
    gboolean state = gtk_switch_get_active(widget);

    for (int i = 0; i < scores.size(); ++i) {
        int index = scores[i].second;
        GtkWidget *word = word_labels[index];
        GtkWidget *time = time_labels[index];
        GtkWidget *relative_time_widget = relative_time_labels[index];
        GtkWidget *score_widget = score_labels[index];

        if (state) {
            // Color gradient from red to green
            double percentage = static_cast<double>(i) / scores.size();
            int red = static_cast<int>(255 * (1 - percentage));
            int green = static_cast<int>(255 * percentage);
            gchar *css = g_strdup_printf("* { color: rgb(%d,%d,0); }", red, green);
            GtkCssProvider *provider = gtk_css_provider_new();
            gtk_css_provider_load_from_data(provider, css, -1);
            GtkStyleContext *context;

            context = gtk_widget_get_style_context(word);
            gtk_style_context_add_provider(context, GTK_STYLE_PROVIDER(provider), GTK_STYLE_PROVIDER_PRIORITY_USER);
            context = gtk_widget_get_style_context(time);
            gtk_style_context_add_provider(context, GTK_STYLE_PROVIDER(provider), GTK_STYLE_PROVIDER_PRIORITY_USER);
            context = gtk_widget_get_style_context(relative_time_widget);
            gtk_style_context_add_provider(context, GTK_STYLE_PROVIDER(provider), GTK_STYLE_PROVIDER_PRIORITY_USER);
            context = gtk_widget_get_style_context(score_widget);
            gtk_style_context_add_provider(context, GTK_STYLE_PROVIDER(provider), GTK_STYLE_PROVIDER_PRIORITY_USER);

            g_object_unref(provider);
        } else {
            if (lowest_indices.find(index) != lowest_indices.end()) {
                // Reset low-score labels to red
                gchar *css = g_strdup_printf("* { color: red; }");
                GtkCssProvider *provider = gtk_css_provider_new();
                gtk_css_provider_load_from_data(provider, css, -1);
                GtkStyleContext *context;

                context = gtk_widget_get_style_context(word);
                gtk_style_context_add_provider(context, GTK_STYLE_PROVIDER(provider), GTK_STYLE_PROVIDER_PRIORITY_USER);
                context = gtk_widget_get_style_context(time);
                gtk_style_context_add_provider(context, GTK_STYLE_PROVIDER(provider), GTK_STYLE_PROVIDER_PRIORITY_USER);
                context = gtk_widget_get_style_context(relative_time_widget);
                gtk_style_context_add_provider(context, GTK_STYLE_PROVIDER(provider), GTK_STYLE_PROVIDER_PRIORITY_USER);
                context = gtk_widget_get_style_context(score_widget);
                gtk_style_context_add_provider(context, GTK_STYLE_PROVIDER(provider), GTK_STYLE_PROVIDER_PRIORITY_USER);

                g_object_unref(provider);
            } else {
                gchar *css = g_strdup_printf("* { color: %s; }",gdk_rgba_to_string(&result_fg_color));
                GtkCssProvider *provider = gtk_css_provider_new();
                gtk_css_provider_load_from_data(provider, css, -1);
                GtkStyleContext *context;
                
                context = gtk_widget_get_style_context(word);
                gtk_style_context_add_provider(context, GTK_STYLE_PROVIDER(provider), GTK_STYLE_PROVIDER_PRIORITY_USER);
                context = gtk_widget_get_style_context(time);
                gtk_style_context_add_provider(context, GTK_STYLE_PROVIDER(provider), GTK_STYLE_PROVIDER_PRIORITY_USER);
                context = gtk_widget_get_style_context(relative_time_widget);
                gtk_style_context_add_provider(context, GTK_STYLE_PROVIDER(provider), GTK_STYLE_PROVIDER_PRIORITY_USER);
                context = gtk_widget_get_style_context(score_widget);
                gtk_style_context_add_provider(context, GTK_STYLE_PROVIDER(provider), GTK_STYLE_PROVIDER_PRIORITY_USER);

                g_object_unref(provider);
            }
        }
    }
}

static void show_results_window() {
    // Berechne die Gesamtdauer
    double total_time = calculate_total_time(word_times);

    // Erstelle ein neues Fenster
    GtkWidget *results_window = gtk_window_new();
    gtk_window_set_title(GTK_WINDOW(results_window), _("Lese Zeit Ergebnisse"));
    gtk_window_set_default_size(GTK_WINDOW(results_window), 600, 300);
    
    GtkWidget *results = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_window_set_child(GTK_WINDOW(results_window), results);
   
    // Füge die Gesamtzeit hinzu
    std::string total_time_text = _("Gesamtzeit: ") + std::to_string(total_time) + " s\n";
    GtkWidget *total_time_label = gtk_label_new(total_time_text.c_str());
    gtk_widget_set_halign(total_time_label, GTK_ALIGN_START);
    gtk_box_append(GTK_BOX(results), total_time_label);
    
    GtkWidget *scrolled_window = gtk_scrolled_window_new();
    gtk_box_append(GTK_BOX(results), scrolled_window);
    gtk_widget_set_vexpand(scrolled_window, TRUE);
    
    GtkWidget *color = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 2);    
    GtkWidget *color_label = gtk_label_new(_("Farbe:"));
    GtkWidget *color_switch = gtk_switch_new();
    gtk_box_append(GTK_BOX(color), color_label);
    gtk_box_append(GTK_BOX(color), color_switch);
    gtk_box_append(GTK_BOX(results), color);
    
    // Erstelle ein Grid zur Organisation der Widgets
    grid = gtk_grid_new();
    gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(scrolled_window), grid);

    // Füge Spaltenüberschriften hinzu
    GtkWidget *word_label = gtk_label_new(_("Wort"));
    GtkWidget *time_label = gtk_label_new(_("Zeit (s)"));
    GtkWidget *relative_time_label = gtk_label_new(_("Prozentuale Zeit (%)"));
    GtkWidget *score_label = gtk_label_new(_("Punkte"));
    gtk_grid_attach(GTK_GRID(grid), word_label, 0, 1, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), time_label, 1, 1, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), relative_time_label, 2, 1, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), score_label, 3, 1, 1, 1);

    // Berechne Scores und sortiere sie
    scores.clear();
    for (int i = 0; i < word_times.size(); ++i) {
        double score = calculate_score(word_times[i].first, word_times[i].second);
        scores.emplace_back(score, i);
    }
    std::sort(scores.begin(), scores.end());

    for (int i = 0; i < 3 && i < scores.size(); ++i) {
        lowest_indices.insert(scores[i].second);
    }

    // Füge Daten zum Grid hinzu
    int row = 2;
    for (int i = 0; i < word_times.size(); ++i) {
        GtkWidget *word = gtk_label_new(word_times[i].first.c_str());
        GtkWidget *time = gtk_label_new(std::to_string(word_times[i].second).c_str());
        double relative_time = (word_times[i].second / total_time) * 100;
        GtkWidget *relative_time_widget = gtk_label_new(std::to_string(relative_time).c_str());
        double score = calculate_score(word_times[i].first, word_times[i].second);
        GtkWidget *score_widget = gtk_label_new(std::to_string(score).c_str());

        word_labels[i] = word;
        time_labels[i] = time;
        relative_time_labels[i] = relative_time_widget;
        score_labels[i] = score_widget;

        if (lowest_indices.find(i) != lowest_indices.end()) {
            // Färbe die Labels rot
            gtk_widget_set_name(word, "low-score");
            gtk_widget_set_name(time, "low-score");
            gtk_widget_set_name(relative_time_widget, "low-score");
            gtk_widget_set_name(score_widget, "low-score");

            gchar *css = g_strdup_printf("* { color: red; }");
            GtkCssProvider *provider = gtk_css_provider_new();
            gtk_css_provider_load_from_data(provider, css, -1);
            GtkStyleContext *context;

            context = gtk_widget_get_style_context(word);
            gtk_style_context_add_provider(context, GTK_STYLE_PROVIDER(provider), GTK_STYLE_PROVIDER_PRIORITY_USER);
            context = gtk_widget_get_style_context(time);
            gtk_style_context_add_provider(context, GTK_STYLE_PROVIDER(provider), GTK_STYLE_PROVIDER_PRIORITY_USER);
            context = gtk_widget_get_style_context(relative_time_widget);
            gtk_style_context_add_provider(context, GTK_STYLE_PROVIDER(provider), GTK_STYLE_PROVIDER_PRIORITY_USER);
            context = gtk_widget_get_style_context(score_widget);
            gtk_style_context_add_provider(context, GTK_STYLE_PROVIDER(provider), GTK_STYLE_PROVIDER_PRIORITY_USER);

            g_object_unref(provider);
        }

        gtk_grid_attach(GTK_GRID(grid), word, 0, row, 1, 1);
        gtk_grid_attach(GTK_GRID(grid), time, 1, row, 1, 1);
        gtk_grid_attach(GTK_GRID(grid), relative_time_widget, 2, row, 1, 1);
        gtk_grid_attach(GTK_GRID(grid), score_widget, 3, row, 1, 1);

        ++row;
    }
    GtkStyleContext *context_Style = gtk_widget_get_style_context(GTK_WIDGET(results_window));
    gtk_style_context_lookup_color(context_Style, "theme_fg_color", &result_fg_color);



    // Connect the switch to the update_colors function
    g_signal_connect(color_switch, "notify::active", G_CALLBACK(update_colors),NULL);
    
    // Zeige alle Widgets an
    gtk_widget_show(results_window);

}

static void remove_word_timer(){
if (timebased_next_word) {
    if (current_word_index <= total_words-1) 
    gtk_widget_show(GTK_WIDGET(global_top_right_button));
    if(timer_id > 0){
    g_source_remove(timer_id);
    timer_id = 0;}
}
}
static void update_label_with_word_by_human(gpointer user_data, bool next) {
if(next)
  update_label_with_next_word();
if(!next)
update_label_with_previous_word();

remove_word_timer();
}
static void run_timer(GtkButton *button, gpointer user_data);

static gboolean update_label_with_next_word_by_timer(gpointer data) {
  update_label_with_next_word();
   run_timer(NULL ,data);

return FALSE;
}

double calculate(double m, double w) {
    return m * (pow(w * w, 1.0 / 3.0));
}

static void run_timer(GtkButton *button, gpointer user_data){
  GtkStack *stack = GTK_STACK(user_data);
  int text_length = strlen(gtk_label_get_text(GTK_LABEL(global_label)));
  int words_per_time = gtk_spin_button_get_value_as_int(global_labelWortsPerTimeSpinn);
  double time_per_word = gtk_spin_button_get_value(global_TimeToNextWordSpinn);

  if (words != NULL && current_word_index < total_words && g_strcmp0(gtk_stack_get_visible_child_name(stack), "page2") == 0 && timebased_next_word){
    gtk_widget_hide(GTK_WIDGET(global_top_right_button));
    timer_id = g_timeout_add(calculate(time_per_word, text_length)*1000, update_label_with_next_word_by_timer, stack);
  }else timer_id =0;
}

static void update_label_with_next_word_by_human(GtkButton *button, gpointer user_data){update_label_with_word_by_human(user_data,true);}
static void update_label_with_previou_word_by_human(GtkButton *button, gpointer user_data){update_label_with_word_by_human(user_data,false);}

// Funktion zum Abrufen der Standard-Schriftart
static gchar* get_default_font_name() {
  GtkSettings *settings = gtk_settings_get_default();
  gchar *font_name;
  g_object_get(settings, "gtk-font-name", &font_name, NULL);
  return font_name;
}

// Funktion zum Abrufen der Standard-Hintergrund- und Schriftfarbe
static void get_default_colors(GdkRGBA *bg_color, GdkRGBA *fg_color, GtkWidget *window) {
  GtkStyleContext *context = gtk_widget_get_style_context(window);

  gtk_style_context_lookup_color(context, "theme_bg_color", bg_color);
  gtk_style_context_lookup_color(context, "theme_fg_color", fg_color);
}

// Callback-Funktion, die beim Klicken auf den Button zum Wechseln zu Seite 1 aufgerufen wird
static void on_switch_to_page1(GtkWidget *widget, gpointer data) {
  remove_word_timer();
  GtkStack *stack = GTK_STACK(data);
  gtk_stack_set_visible_child_name(stack, "page1");
}

// Funktion zum Anwenden der Label-Stile
static void apply_label_styles() {
  GdkRGBA bg_color, fg_color;
  gtk_color_chooser_get_rgba(GTK_COLOR_CHOOSER(global_labelBackgroundColor), &bg_color);
  gtk_color_chooser_get_rgba(GTK_COLOR_CHOOSER(global_labelForgroudColor), &fg_color);

  gchar *font_desc_str = gtk_font_chooser_get_font(GTK_FONT_CHOOSER(global_labelTextButton));

  PangoFontDescription *font_desc = pango_font_description_from_string(font_desc_str);
  const gchar *font_family = pango_font_description_get_family(font_desc);

  gint font_size = pango_font_description_get_size(font_desc) / PANGO_SCALE;

  gchar *css = g_strdup_printf(
                 "* {"
                 "  background-color: %s;"
                 "  color: %s;"
                 "  font-family: '%s';"
                 "  font-size: %dpt;"
                 "}",
                 gdk_rgba_to_string(&bg_color),
                 gdk_rgba_to_string(&fg_color),
                 font_family,
                 font_size
               );

  GtkCssProvider *provider = gtk_css_provider_new();
  gtk_css_provider_load_from_data(provider, css, -1);

  GtkStyleContext *context = gtk_widget_get_style_context(GTK_WIDGET(global_label));
  gtk_style_context_add_provider(context, GTK_STYLE_PROVIDER(provider), GTK_STYLE_PROVIDER_PRIORITY_USER);

  g_object_unref(provider);
  g_free(css);
  g_free(font_desc_str);
  pango_font_description_free(font_desc);
}

// Callback-Funktion, die beim Klicken auf den Button zum Wechseln zu Seite 2 aufgerufen wird
static void on_switch_to_page2(GtkWidget *widget, gpointer data) {
  GtkStack *stack = GTK_STACK(data);
  GtkTextBuffer *buffer = gtk_text_view_get_buffer(global_text_view);
   
  if(make_statistics)
  word_times.clear(); 
  
  make_statistics = gtk_switch_get_active(global_StatisticsSwitch);

  GtkTextIter start, end;
  gtk_text_buffer_get_start_iter(buffer, &start);
  gtk_text_buffer_get_end_iter(buffer, &end);

  char *text = gtk_text_buffer_get_text(buffer, &start, &end, FALSE);

  split_text_into_words(text);
  apply_label_styles();

  g_free(text);

  gboolean state = gtk_switch_get_active(global_labelProgressSwitch);
  if (state) {
    gtk_widget_show(GTK_WIDGET(global_ProgressLabel));
    gtk_widget_show(GTK_WIDGET(global_ProgressBar));
  } else {
    gtk_widget_hide(GTK_WIDGET(global_ProgressLabel));
    gtk_widget_hide(GTK_WIDGET(global_ProgressBar));
  }
  
  timebased_next_word = gtk_switch_get_active(global_TimeToNextWordSwitch);
  gtk_widget_hide(GTK_WIDGET(global_top_right_button));
  gtk_stack_set_visible_child_name(stack, "page2");

  if (timebased_next_word)
    update_label_with_next_word_by_timer(stack);
  else
    update_label_with_next_word();

  save_settings();
}

// Tastensignal-Callback-Funktion
static gboolean on_key_press(GtkEventControllerKey *controller, guint keyval, guint keycode, GdkModifierType state, gpointer data) {
  GtkStack *stack = GTK_STACK(data);
  const gchar *visible_child_name = gtk_stack_get_visible_child_name(stack);
  if (g_strcmp0(visible_child_name, "page2") != 0)
    return FALSE;

  if (keyval != GDK_KEY_Left && keyval != GDK_KEY_Escape && keyval != GDK_KEY_Down) {
    update_label_with_word_by_human(stack,true);
    return TRUE;
  } else if (keyval == GDK_KEY_Left || keyval == GDK_KEY_Down) {
    update_label_with_word_by_human(stack,false);
    return TRUE;
  } else if (keyval == GDK_KEY_Escape) {
    on_switch_to_page1(NULL, stack);
    return TRUE;
  }
  return FALSE;
}
static void on_reset_button_clicked(GtkButton *button, gpointer user_data) {
  // Setze die Standard-Hintergrund- und Schriftfarbe zurück
  GdkRGBA bg_color, fg_color;
  get_default_colors(&bg_color, &fg_color, GTK_WIDGET(user_data));
  gtk_color_chooser_set_rgba(GTK_COLOR_CHOOSER(global_labelBackgroundColor), &bg_color);
  gtk_color_chooser_set_rgba(GTK_COLOR_CHOOSER(global_labelForgroudColor), &fg_color);

  // Setze die Schriftart und -größe zurück
  PangoFontDescription *default_font_desc = pango_font_description_from_string(get_default_font_name());
  pango_font_description_set_size(default_font_desc, 50 * PANGO_SCALE);  // Setze Größe auf 50pt
  gchar *default_font_desc_str = pango_font_description_to_string(default_font_desc);
  gtk_font_chooser_set_font(GTK_FONT_CHOOSER(global_labelTextButton), default_font_desc_str);

  pango_font_description_free(default_font_desc);
  g_free(default_font_desc_str);

  // Setze den Fortschrittsschalter zurück
  gtk_switch_set_active(global_labelProgressSwitch, TRUE);

  // Setze die Anzahl der Wörter pro Zeitspanne zurück
  gtk_spin_button_set_value(global_labelWortsPerTimeSpinn, 1);

  // Leere das Textfeld
  GtkTextBuffer *buffer = gtk_text_view_get_buffer(global_text_view);
  gtk_text_buffer_set_text(buffer, "", -1);

  update_read_button_state(buffer, NULL); // Aktualisiere den Zustand des "Lesen"-Buttons

  gtk_spin_button_set_value(global_TimeToNextWordSpinn, 0.9);

  gtk_switch_set_active(global_TimeToNextWordSwitch, FALSE);
}

static void TimeToNextWordSwitchtoggle(GtkSwitch *widget, gpointer data){
if(gtk_switch_get_active(global_TimeToNextWordSwitch))
    gtk_widget_set_sensitive(GTK_WIDGET(global_TimeToNextWordSpinn), TRUE);
else
    gtk_widget_set_sensitive(GTK_WIDGET(global_TimeToNextWordSpinn), FALSE);

}
// Funktion zum Erstellen von Seite 1
static GtkWidget *create_page1(GtkStack *stack, GtkWidget *window) {
    GtkWidget *page1 = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);

    GtkWidget *label = gtk_label_new(_("Einstellungen:"));
    gtk_widget_set_halign(label, GTK_ALIGN_CENTER);

    GtkWidget *Backround = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 2);
    GtkWidget *labelBackground = gtk_label_new(_("Hintergrund:"));
    global_labelBackgroundColor = GTK_COLOR_BUTTON(gtk_color_button_new());

    GtkWidget *Forgroud = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 2);
    GtkWidget *labelForgroud = gtk_label_new(_("Schrift:"));
    global_labelForgroudColor = GTK_COLOR_BUTTON(gtk_color_button_new());

    GtkWidget *Text = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 2);
    GtkWidget *labelTextSize = gtk_label_new(_("Schrift Größe:"));
    global_labelTextButton = GTK_FONT_BUTTON(gtk_font_button_new());

    GtkWidget *Progress = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 2);
    GtkWidget *labelProgress = gtk_label_new(_("Fortschritt Zeigen:"));
    global_labelProgressSwitch = GTK_SWITCH(gtk_switch_new());

    GtkAdjustment *adjustment = gtk_adjustment_new(1, 1, 100000000, 1, 10, 1);
    GtkWidget *WortsPerTime = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 2);
    GtkWidget *labelWortsPerTime = gtk_label_new(_("Angezeigte wörter:"));
    global_labelWortsPerTimeSpinn = GTK_SPIN_BUTTON(gtk_spin_button_new(adjustment, 1, 0));

    GtkAdjustment *adjustment2 = gtk_adjustment_new(0.9, 0.001, 100000000, 0.05, 0.5, 1);
    GtkWidget *TimeToNextWord = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 4);
    GtkWidget *labelTimeToNextWord = gtk_label_new(_("Zeitbassirt nechstes Wort:"));
    global_TimeToNextWordSwitch = GTK_SWITCH(gtk_switch_new());
    GtkWidget *label2TimeToNextWord = gtk_label_new(_("Zeit (sec):"));
    global_TimeToNextWordSpinn = GTK_SPIN_BUTTON(gtk_spin_button_new(adjustment2, 1, 3));
    
    GtkWidget *Statistics = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 2);
    GtkWidget *labelStatistics = gtk_label_new(_("Statistiken erheben:"));
    global_StatisticsSwitch = GTK_SWITCH(gtk_switch_new());

    gtk_widget_set_tooltip_text(TimeToNextWord, _("Die Zeit wird berechnet durch:\nZeit−Pro−Wort×(₃√Wort−Länge )"));
    GtkWidget *ResetButton = gtk_button_new_with_label(_("Zurücksetzen"));

    gtk_switch_set_active(GTK_SWITCH(global_labelProgressSwitch), TRUE);

    // Erstelle eine Schriftartbeschreibung mit Standardgröße 50
    PangoFontDescription *default_font_desc = pango_font_description_from_string(get_default_font_name());
    pango_font_description_set_size(default_font_desc, 50 * PANGO_SCALE);  // Setze Größe auf 50pt
    gchar *default_font_desc_str = pango_font_description_to_string(default_font_desc);
    gtk_font_chooser_set_font(GTK_FONT_CHOOSER(global_labelTextButton), default_font_desc_str);

    pango_font_description_free(default_font_desc);
    g_free(default_font_desc_str);

    global_button_read = GTK_BUTTON(gtk_button_new_with_label(_("Lesen")));
    global_text_view = GTK_TEXT_VIEW(gtk_text_view_new());
    GtkWidget *scrolled_window = gtk_scrolled_window_new();
    gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(scrolled_window), GTK_WIDGET(global_text_view));
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolled_window), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);

    // Standard-Hintergrund- und Schriftfarbe abrufen und setzen
    GdkRGBA bg_color, fg_color;
    get_default_colors(&bg_color, &fg_color, window);
    gtk_color_chooser_set_rgba(GTK_COLOR_CHOOSER(global_labelBackgroundColor), &bg_color);
    gtk_color_chooser_set_rgba(GTK_COLOR_CHOOSER(global_labelForgroudColor), &fg_color);

    gtk_box_append(GTK_BOX(page1), label);
    gtk_box_append(GTK_BOX(Backround), labelBackground);
    gtk_box_append(GTK_BOX(Backround), GTK_WIDGET(global_labelBackgroundColor));
    gtk_box_append(GTK_BOX(page1), Backround);

    gtk_box_append(GTK_BOX(Forgroud), labelForgroud);
    gtk_box_append(GTK_BOX(Forgroud), GTK_WIDGET(global_labelForgroudColor));
    gtk_box_append(GTK_BOX(page1), Forgroud);

    gtk_box_append(GTK_BOX(Text), labelTextSize);
    gtk_box_append(GTK_BOX(Text), GTK_WIDGET(global_labelTextButton));
    gtk_box_append(GTK_BOX(page1), Text);

    gtk_box_append(GTK_BOX(Progress), labelProgress);
    gtk_box_append(GTK_BOX(Progress), GTK_WIDGET(global_labelProgressSwitch));
    gtk_box_append(GTK_BOX(page1), Progress);

    gtk_box_append(GTK_BOX(WortsPerTime), labelWortsPerTime);
    gtk_box_append(GTK_BOX(WortsPerTime), GTK_WIDGET(global_labelWortsPerTimeSpinn));
    gtk_box_append(GTK_BOX(page1), WortsPerTime);

    gtk_box_append(GTK_BOX(TimeToNextWord), labelTimeToNextWord);
    gtk_box_append(GTK_BOX(TimeToNextWord), GTK_WIDGET(global_TimeToNextWordSwitch));
    gtk_box_append(GTK_BOX(TimeToNextWord), label2TimeToNextWord);
    gtk_box_append(GTK_BOX(TimeToNextWord), GTK_WIDGET(global_TimeToNextWordSpinn));
    gtk_box_append(GTK_BOX(page1), TimeToNextWord);
    
    gtk_box_append(GTK_BOX(Statistics), labelStatistics);
    gtk_box_append(GTK_BOX(Statistics), GTK_WIDGET(global_StatisticsSwitch));
    gtk_box_append(GTK_BOX(page1), Statistics);

    gtk_box_append(GTK_BOX(page1), ResetButton);

    gtk_box_append(GTK_BOX(page1), scrolled_window);
    gtk_widget_set_vexpand(scrolled_window, TRUE);
    gtk_box_append(GTK_BOX(page1), GTK_WIDGET(global_button_read));
    gtk_widget_set_hexpand(GTK_WIDGET(global_button_read), FALSE);
    gtk_widget_set_vexpand(GTK_WIDGET(global_button_read), FALSE);

    update_read_button_state(gtk_text_view_get_buffer(GTK_TEXT_VIEW(global_text_view)), NULL);

    g_signal_connect(GTK_WIDGET(global_button_read), "clicked", G_CALLBACK(on_switch_to_page2), stack);
    g_signal_connect(gtk_text_view_get_buffer(GTK_TEXT_VIEW(global_text_view)), "changed", G_CALLBACK(update_read_button_state), NULL);
    g_signal_connect(ResetButton, "clicked", G_CALLBACK(on_reset_button_clicked), window);
        g_signal_connect(global_TimeToNextWordSwitch, "notify::active", G_CALLBACK(TimeToNextWordSwitchtoggle), NULL);

    return page1;
}

// Funktion zum Erstellen von Seite 2
static GtkWidget *create_page2(GtkStack *stack, GtkWidget *window) {
    GtkWidget *page2 = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    global_label = GTK_LABEL(gtk_label_new(""));
    GtkWidget *scrolled_window = gtk_scrolled_window_new();
    
    global_results_button = GTK_BUTTON(gtk_button_new_with_label(_("Ergebnissezeigen")));

    GtkWidget *ProgressBox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 2);
    global_ProgressLabel = GTK_LABEL(gtk_label_new("/"));
    global_ProgressBar = GTK_PROGRESS_BAR(gtk_progress_bar_new());

    GtkWidget *hbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 2);
    global_button_previous = GTK_BUTTON(gtk_button_new_with_label("←"));
    global_button_next = GTK_BUTTON(gtk_button_new_with_label("→"));

    GtkWidget *button2 = gtk_button_new_with_label(_("Zurück"));

    GtkWidget *spacer1 = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_widget_set_vexpand(spacer1, TRUE);
    gtk_box_append(GTK_BOX(page2), spacer1);

    gtk_widget_set_halign(GTK_WIDGET(global_label), GTK_ALIGN_CENTER);
    gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(scrolled_window), GTK_WIDGET(global_label));
    gtk_box_append(GTK_BOX(page2), GTK_WIDGET(scrolled_window));
    gtk_widget_set_hexpand(scrolled_window, TRUE);
    gtk_scrolled_window_set_overlay_scrolling(GTK_SCROLLED_WINDOW(scrolled_window), FALSE);
    gtk_widget_set_vexpand(GTK_WIDGET(global_label), TRUE);
    
    gtk_box_append(GTK_BOX(page2), GTK_WIDGET(global_results_button));
    gtk_widget_hide(GTK_WIDGET(global_results_button));

    GtkWidget *spacer2 = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_widget_set_vexpand(spacer2, TRUE);
    gtk_box_append(GTK_BOX(page2), spacer2);

    gtk_box_append(GTK_BOX(ProgressBox), GTK_WIDGET(global_ProgressLabel));
    gtk_box_append(GTK_BOX(ProgressBox), GTK_WIDGET(global_ProgressBar));
    gtk_box_append(GTK_BOX(page2), ProgressBox);
    gtk_widget_set_hexpand(GTK_WIDGET(global_ProgressBar), TRUE);

    gtk_box_append(GTK_BOX(hbox), GTK_WIDGET(global_button_previous));
    gtk_box_append(GTK_BOX(hbox), GTK_WIDGET(global_button_next));
    gtk_box_append(GTK_BOX(page2), hbox);

    gtk_widget_set_hexpand(GTK_WIDGET(global_button_previous), TRUE);
    gtk_widget_set_hexpand(GTK_WIDGET(global_button_next), TRUE);
    gtk_widget_set_halign(hbox, GTK_ALIGN_FILL);
    gtk_widget_set_hexpand(hbox, TRUE);

    gtk_box_append(GTK_BOX(page2), button2);

    g_signal_connect(GTK_WIDGET(global_button_previous), "clicked", G_CALLBACK(update_label_with_previou_word_by_human), stack);
    g_signal_connect(GTK_WIDGET(global_button_next), "clicked", G_CALLBACK(update_label_with_next_word_by_human), stack);
    g_signal_connect(button2, "clicked", G_CALLBACK(on_switch_to_page1), stack);
    g_signal_connect(global_results_button, "clicked", G_CALLBACK(show_results_window), NULL);

    // Erstellen eines Overlays
    GtkWidget *overlay = gtk_overlay_new();
    gtk_overlay_set_child(GTK_OVERLAY(overlay), page2);

    gchar *css = g_strdup_printf(
        ".transparent-button {background-color: transparent;border: none;box-shadow: none;}"
    );

    GtkCssProvider *provider = gtk_css_provider_new();
    gtk_css_provider_load_from_data(provider, css, -1);

    // Button für die rechte obere Ecke
    global_top_right_button = GTK_BUTTON(gtk_button_new_with_label("▶️"));
    GtkStyleContext *context = gtk_widget_get_style_context(GTK_WIDGET(global_top_right_button));
    gtk_style_context_add_provider(context, GTK_STYLE_PROVIDER(provider), GTK_STYLE_PROVIDER_PRIORITY_USER);
    gtk_style_context_add_class(gtk_widget_get_style_context(GTK_WIDGET(global_top_right_button)), "transparent-button");

    g_signal_connect(GTK_WIDGET(global_top_right_button), "clicked", G_CALLBACK(run_timer), stack);

    gtk_overlay_add_overlay(GTK_OVERLAY(overlay), GTK_WIDGET(global_top_right_button));
    gtk_widget_set_halign(GTK_WIDGET(global_top_right_button), GTK_ALIGN_END);
    gtk_widget_set_valign(GTK_WIDGET(global_top_right_button), GTK_ALIGN_START);
    g_object_unref(provider);
    g_free(css);
    gtk_widget_hide(GTK_WIDGET(global_top_right_button));

    return overlay;
}

// Callback-Funktion, die beim Schließen des Fensters aufgerufen wird
static void on_window_destroy(GtkWidget *widget, gpointer data) {
  GMainLoop *loop = (GMainLoop *)data;
  g_main_loop_quit(loop);
}
static void on_close_request(GtkWidget *widget, gpointer data) {
  save_settings();
}

int main(int argc, char *argv[]) {
    gtk_init();

    setlocale(LC_ALL, "");
    bindtextdomain("FastReader", "locale");
    textdomain("FastReader");

    const gchar *config_dir = g_get_user_config_dir();
    if (config_dir == NULL) {
        printf(_("Konnte das Standard-Konfigurationsverzeichnis nicht abrufen.\n"));
        return 1;
    }

    gchar *fastreader_dir = g_build_filename(config_dir, "FastReader", NULL);

    if (g_mkdir_with_parents(fastreader_dir, 0700) != 0) {
        printf(_("Fehler beim Erstellen des Verzeichnisses FastReader.\n"));
        g_free(fastreader_dir);
        return 1;
    }

    config_file_path = g_build_filename(fastreader_dir, "config.cfg", NULL);

    GtkWidget *window = gtk_window_new();
    gtk_window_set_title(GTK_WINDOW(window), _("Fast Reader"));
    gtk_window_set_default_size(GTK_WINDOW(window), 400, 600);

    GMainLoop *loop = g_main_loop_new(NULL, FALSE);
    g_signal_connect(window, "destroy", G_CALLBACK(on_window_destroy), loop);
    g_signal_connect(window, "close-request", G_CALLBACK(on_close_request), NULL);

    GtkWidget *stack = gtk_stack_new();
    gtk_stack_set_transition_type(GTK_STACK(stack), GTK_STACK_TRANSITION_TYPE_SLIDE_LEFT_RIGHT);
    gtk_stack_add_named(GTK_STACK(stack), create_page1(GTK_STACK(stack), window), "page1");
    gtk_stack_add_named(GTK_STACK(stack), create_page2(GTK_STACK(stack), window), "page2");
    
    load_settings();
    
    if(gtk_switch_get_active(global_TimeToNextWordSwitch))
       gtk_widget_set_sensitive(GTK_WIDGET(global_TimeToNextWordSpinn), TRUE);
    else
       gtk_widget_set_sensitive(GTK_WIDGET(global_TimeToNextWordSpinn), FALSE);
    
    GtkEventController *controller = gtk_event_controller_key_new();
    g_signal_connect(controller, "key-pressed", G_CALLBACK(on_key_press), stack);
    gtk_widget_add_controller(window, controller);
    
    gtk_window_set_child(GTK_WINDOW(window), stack);

    gtk_widget_show(window);
    g_main_loop_run(loop);

    g_main_loop_unref(loop);
    g_free(fastreader_dir);
    g_free(config_file_path);
    return 0;
}
