#include <gtk/gtk.h>
#ifdef USE_ADWAITA
#include <adwaita.h>
#endif
#include <libintl.h>
#include <locale.h>
#include <libconfig.h>
#include <chrono>
#include <vector>
#include <numeric>
#include <string>
#include <algorithm>
#include <set>
#include <map>

#include <iostream>


#define FAST_READER_VERSION "7.9"


std::chrono::steady_clock::time_point start_time;
std::vector<std::pair<std::string, double>> word_times;

#define _(STRING) gettext(STRING)

// Globale Variable zur Speicherung des Textfelds
GtkTextView *global_text_view = NULL;
GtkLabel *global_label = NULL;
GtkColorDialogButton *global_labelBackgroundColor = NULL;
GtkColorDialogButton *global_labelForgroudColor = NULL;
GtkFontDialogButton *global_labelTextButton = NULL;
GtkButton *global_button_previous = NULL;
GtkButton *global_button_next = NULL;
GtkButton *global_button_read = NULL;
GtkLabel *global_ProgressLabel = NULL;
GtkProgressBar *global_ProgressBar = NULL;
GtkSwitch *global_labelProgressSwitch = NULL;
GtkSpinButton *global_labelWortsPerTimeSpinn = NULL;
GtkSwitch *global_TimeToNextWordSwitch = NULL;
GtkSpinButton *global_TimeToNextWordSpinn = NULL;
GtkButton *global_top_right_button=NULL;
GtkSwitch *global_StatisticsSwitch = NULL;
GtkButton *global_results_button = NULL;
GtkWidget *global_copy_button = NULL;
GtkWidget *global_clear_button = NULL;
GtkWidget *global_pause_button = NULL;
GtkSwitch *global_SwitchLongerTimeOnLongWord = NULL;
GtkSpinButton *global_SpinnButtonLongerTimeOnLongWord = NULL;
GtkSwitch *global_SwitchLongerTimeFirstWord = NULL;
GtkSpinButton *global_SpinnButtonLongerTimeOnFirstWord= NULL;
GtkSpinButton *global_SpinnButtonLongerTimeOnLongWordMultyplyer= NULL;
GtkButton *global_top_right_jump_button= NULL;
GtkSwitch *global_labelBackgroundColorSwitch= NULL;
GtkSwitch *global_labelForgroudColorSwitch= NULL;

char *last_text = NULL;
bool same_text=false;

gchar *config_file_path = NULL;


// Globale Variablen zur Verwaltung des Texts und der Wörter
gchar **words = NULL;
int current_word_index = 0;
int last_word_index=0;
int total_words = 0;
gboolean timebased_next_word = false;
gboolean make_statistics = false;
guint timer_id;

void save_settings() {
    config_t cfg;
    config_init(&cfg);

    config_setting_t *root = config_root_setting(&cfg);

    const GdkRGBA *bg_color=
        gtk_color_dialog_button_get_rgba(global_labelBackgroundColor);
    config_setting_t *bg_color_setting = config_setting_add(root, "background_color", CONFIG_TYPE_STRING);
    config_setting_set_string(bg_color_setting, gdk_rgba_to_string(bg_color));

    const GdkRGBA *fg_color=
        gtk_color_dialog_button_get_rgba(global_labelForgroudColor);
    config_setting_t *fg_color_setting = config_setting_add(root, "foreground_color", CONFIG_TYPE_STRING);
    config_setting_set_string(fg_color_setting, gdk_rgba_to_string(fg_color));

    gboolean bg_color_switch = gtk_switch_get_active(global_labelBackgroundColorSwitch);
    config_setting_t *bg_color_switch_setting = config_setting_add(root, "background_color_switch", CONFIG_TYPE_BOOL);
    config_setting_set_bool(bg_color_switch_setting, bg_color_switch);

    gboolean fg_color_switch = gtk_switch_get_active(global_labelForgroudColorSwitch);
    config_setting_t *fg_color_switch_setting = config_setting_add(root, "foreground_color_switsh", CONFIG_TYPE_BOOL);
    config_setting_set_bool(fg_color_switch_setting, fg_color_switch);

    const char *font = pango_font_description_to_string(gtk_font_dialog_button_get_font_desc(global_labelTextButton));
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

    gboolean longer_time_on_long_word = gtk_switch_get_active(global_SwitchLongerTimeOnLongWord);
    config_setting_t *longer_time_on_long_word_setting = config_setting_add(root, "longer_time_on_long_word", CONFIG_TYPE_BOOL);
    config_setting_set_bool(longer_time_on_long_word_setting, longer_time_on_long_word);

    double longer_time_on_long_word_value = gtk_spin_button_get_value(global_SpinnButtonLongerTimeOnLongWord);
    config_setting_t *longer_time_on_long_word_value_setting = config_setting_add(root, "longer_time_on_long_word_value", CONFIG_TYPE_FLOAT);
    config_setting_set_float(longer_time_on_long_word_value_setting, longer_time_on_long_word_value);


    double longer_time_on_long_word_multyplayer = gtk_spin_button_get_value(global_SpinnButtonLongerTimeOnLongWordMultyplyer);
    config_setting_t *longer_time_on_long_word_multyplayer_setting = config_setting_add(root, "longer_time_on_long_word_multyplayer", CONFIG_TYPE_FLOAT);
    config_setting_set_float(longer_time_on_long_word_multyplayer_setting, longer_time_on_long_word_multyplayer);

    gboolean longer_time_on_first_word = gtk_switch_get_active(global_SwitchLongerTimeFirstWord);
    config_setting_t *longer_time_on_first_word_setting = config_setting_add(root, "longer_time_on_first_word", CONFIG_TYPE_BOOL);
    config_setting_set_bool(longer_time_on_first_word_setting, longer_time_on_first_word);

    double longer_time_on_first_word_value = gtk_spin_button_get_value(global_SpinnButtonLongerTimeOnFirstWord);
    config_setting_t *longer_time_on_first_word_value_setting = config_setting_add(root, "longer_time_on_first_word_value", CONFIG_TYPE_FLOAT);
    config_setting_set_float(longer_time_on_first_word_value_setting, longer_time_on_first_word_value);

    config_setting_t *current_word_index_setting = config_setting_add(root, "current_word_index", CONFIG_TYPE_INT);
    config_setting_set_int(current_word_index_setting, current_word_index);

    if (!config_write_file(&cfg, config_file_path)) {
        fprintf(stderr, _("Error while writing file.\n"));
    }


    config_destroy(&cfg);
}
void load_settings() {
    config_t cfg;
    config_init(&cfg);

    if (!config_read_file(&cfg, config_file_path)) {
        fprintf(stderr, _("Error while reading file: %s:%d - %s\n"),
                config_error_file(&cfg),
                config_error_line(&cfg),
                config_error_text(&cfg));
        fprintf(stderr, _("Trye to corrct\n"));
        save_settings();
        config_destroy(&cfg);
        return;
    }

    const char *bg_color_str;
    if (config_lookup_string(&cfg, "background_color", &bg_color_str)) {
        GdkRGBA bg_color;
        if (gdk_rgba_parse(&bg_color, bg_color_str)) {
            gtk_color_dialog_button_set_rgba(global_labelBackgroundColor, &bg_color);
        }
    }


    const char *fg_color_str;
    if (config_lookup_string(&cfg, "foreground_color", &fg_color_str)) {
        GdkRGBA fg_color;
        if (gdk_rgba_parse(&fg_color, fg_color_str)) {
            gtk_color_dialog_button_set_rgba(global_labelForgroudColor, &fg_color);
        }
    }

    int bg_color_switch;
    if (config_lookup_bool(&cfg, "background_color_switch", &bg_color_switch)) {
        gtk_switch_set_active(global_labelBackgroundColorSwitch, bg_color_switch);
    }


    int fg_color_switsh;
    if (config_lookup_bool(&cfg, "foreground_color_switsh", &fg_color_switsh)) {
        gtk_switch_set_active(global_labelForgroudColorSwitch, fg_color_switsh);
    }


    const char *font_str;
    if (config_lookup_string(&cfg, "font", &font_str)) {
        // Aktuell eingestellte Schriftart speichern
        char *previous_font = pango_font_description_to_string(gtk_font_dialog_button_get_font_desc(global_labelTextButton));

        PangoFontDescription *font_desc = pango_font_description_from_string(font_str);

        if (font_desc != NULL) {
            // Versuche die Schriftart auf die Komponente anzuwenden
            gtk_font_dialog_button_set_font_desc(GTK_FONT_DIALOG_BUTTON(global_labelTextButton), font_desc);

            // Überprüfe, ob die Schriftart erfolgreich gesetzt wurde
            char *current_font = pango_font_description_to_string(gtk_font_dialog_button_get_font_desc(global_labelTextButton));
            if (g_strcmp0(current_font, font_str) != 0) {
                // Setze die vorherige Schriftart wieder ein
                PangoFontDescription *previous_font_desc = pango_font_description_from_string(previous_font);
                gtk_font_dialog_button_set_font_desc(GTK_FONT_DIALOG_BUTTON(global_labelTextButton), previous_font_desc);
                pango_font_description_free(previous_font_desc);
            }
            g_free(current_font);
            pango_font_description_free(font_desc);
        } else {
            // Falls die Font-Beschreibung fehlschlägt, setze die vorherige Schriftart wieder ein
            PangoFontDescription *previous_font_desc = pango_font_description_from_string(previous_font);
            gtk_font_dialog_button_set_font_desc(GTK_FONT_DIALOG_BUTTON(global_labelTextButton), previous_font_desc);
            pango_font_description_free(previous_font_desc);
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

        if (last_text)
            g_free(last_text);

        last_text = g_strdup(text_str);
    }

    int make_statistics;
    if (config_lookup_bool(&cfg, "make_statistics", &make_statistics)) {
        gtk_switch_set_active(global_StatisticsSwitch, make_statistics);
    }


    int longer_time_on_long_word;
    if (config_lookup_bool(&cfg, "longer_time_on_long_word", &longer_time_on_long_word)) {
        gtk_switch_set_active(global_SwitchLongerTimeOnLongWord, longer_time_on_long_word);
    }

    double longer_time_on_long_word_value;
    if (config_lookup_float(&cfg, "longer_time_on_long_word_value", &longer_time_on_long_word_value)) {
        gtk_spin_button_set_value(global_SpinnButtonLongerTimeOnLongWord, longer_time_on_long_word_value);
    }

    double longer_time_on_long_word_multyplayer;
    if (config_lookup_float(&cfg, "longer_time_on_long_word_multyplayer", &longer_time_on_long_word_multyplayer)) {
        gtk_spin_button_set_value(global_SpinnButtonLongerTimeOnLongWordMultyplyer, longer_time_on_long_word_multyplayer);
    }

    int longer_time_on_first_word;
    if (config_lookup_bool(&cfg, "longer_time_on_first_word", &longer_time_on_first_word)) {
        gtk_switch_set_active(global_SwitchLongerTimeFirstWord, longer_time_on_first_word);
    }

    double longer_time_on_first_word_value;
    if (config_lookup_float(&cfg, "longer_time_on_first_word_value", &longer_time_on_first_word_value)) {
        gtk_spin_button_set_value(global_SpinnButtonLongerTimeOnFirstWord, longer_time_on_first_word_value);
    }

    int index;
    if (config_lookup_int(&cfg, "current_word_index", &index)) {
        current_word_index = index;
    }

    config_destroy(&cfg);
}

void update_button_and_Lable_states() {
    int words_per_time = gtk_spin_button_get_value_as_int(global_labelWortsPerTimeSpinn);

    if (current_word_index <= words_per_time) {
        gtk_widget_set_sensitive(GTK_WIDGET(global_button_previous), FALSE);
    } else {
        gtk_widget_set_sensitive(GTK_WIDGET(global_button_previous), TRUE);
    }

    if (current_word_index >= total_words+make_statistics) {
        gtk_widget_set_sensitive(GTK_WIDGET(global_button_next), FALSE);
        gtk_widget_set_sensitive(GTK_WIDGET(global_pause_button), FALSE);
    } else {
        gtk_widget_set_sensitive(GTK_WIDGET(global_button_next), TRUE);
        gtk_widget_set_sensitive(GTK_WIDGET(global_pause_button), TRUE);

    }

    if (current_word_index >= total_words-1){
        gtk_widget_set_visible(GTK_WIDGET(global_top_right_button),FALSE);
        gtk_button_set_icon_name(GTK_BUTTON(global_pause_button),"gtk-media-pause");
    }

    if (current_word_index >= total_words+1 && make_statistics)
        gtk_widget_set_visible(GTK_WIDGET(global_results_button),TRUE);
    else
        gtk_widget_set_visible(GTK_WIDGET(global_results_button),FALSE);

    int clamped_current_word_index =MIN(current_word_index,total_words);

    int progress_index = (clamped_current_word_index + words_per_time-1) / words_per_time;
    int total_progress_steps = (total_words + words_per_time-1) / words_per_time;
    gchar *progress_text;
    if (words_per_time > 1) {
        progress_text = g_strdup_printf("%d/%d - %d/%d", progress_index, total_progress_steps, clamped_current_word_index, total_words);
    } else {
        progress_text = g_strdup_printf("%d/%d", progress_index, total_progress_steps);
    }
    gtk_label_set_text(global_ProgressLabel, progress_text);
    g_free(progress_text); // Speicher freigeben
    double progress = (double)progress_index / total_progress_steps;
    gtk_progress_bar_set_fraction(global_ProgressBar, progress);

    if(same_text && current_word_index < last_word_index)
        gtk_widget_set_visible(GTK_WIDGET(global_top_right_jump_button),TRUE);
    else
        gtk_widget_set_visible(GTK_WIDGET(global_top_right_jump_button),FALSE);

}

// Funktion zum Überprüfen des Textfelds und zum Aktualisieren des "Lesen"-Buttons
void update_read_and_actions_button_state(GtkTextBuffer *buffer, gpointer user_data) {
    GtkTextIter start, end;
    gtk_text_buffer_get_start_iter(buffer, &start);
    gtk_text_buffer_get_end_iter(buffer, &end);

    if (gtk_text_iter_equal(&start, &end)) {
        gtk_widget_set_sensitive(GTK_WIDGET(global_button_read), FALSE);
        gtk_widget_set_visible(global_copy_button,FALSE);
        gtk_widget_set_visible(global_clear_button,FALSE);
    } else {
        gtk_widget_set_sensitive(GTK_WIDGET(global_button_read), TRUE);
        gtk_widget_set_visible(global_copy_button,TRUE);
        gtk_widget_set_visible(global_clear_button,TRUE);
    }
}

// Funktion zum Aufteilen eines Textes in Wörter
void split_text_into_words(const gchar *text) {

    if (words != NULL) {
        g_strfreev(words);
    }
    words=NULL;

    // Text in Wörter aufteilen, wobei Leerzeichen und neue Zeilen als Trennzeichen verwendet werden
    words = g_regex_split_simple("[ \n]+", text, (GRegexCompileFlags) 0, (GRegexMatchFlags) 0);
    if (words == NULL) {
        // Fehlerbehandlung, z.B. Protokollierung oder Rückgabe eines Fehlers
        return;
    }

    total_words = g_strv_length(words);


    int i = 0;
    while (i < total_words) {
        if (g_strcmp0(words[i], "") == 0) {
            g_free(words[i]);
            for (int j = i; j < total_words - 1; j++) {
                words[j] = words[j + 1];
            }
            words[total_words - 1] = NULL;
            total_words--;
        } else {
            i++;
        }
    }
    update_button_and_Lable_states();

}

GString* list_current_words(){
    int words_per_time = gtk_spin_button_get_value_as_int(global_labelWortsPerTimeSpinn);
    GString *current_words = g_string_new(NULL);

    if (current_words == NULL) {
        // Fehlerbehandlung, z.B. Protokollierung oder Rückgabe eines Fehlers
        return NULL;
    }

    int current_word_index_shifted=MAX(current_word_index-1,0);

    for (int i = 0; i < words_per_time && (current_word_index_shifted + i) < total_words; i++) {
        if (g_strcmp0(words[current_word_index_shifted + i], "") != 0) {
            g_string_append(current_words, words[current_word_index_shifted + i]);
            if (i < words_per_time - 1 && (current_word_index_shifted + i) < total_words - 1) {
                g_string_append(current_words, " ");
            }
        }
    }
    return current_words;
}

void update_displaed_word(){
    int words_per_time = gtk_spin_button_get_value_as_int(global_labelWortsPerTimeSpinn);

    GString *current_words = g_string_new(NULL);
    if (current_words == NULL) {
        return;
    }

    if (current_word_index <= total_words) {
        GString *temp_words = list_current_words();
        if (temp_words == NULL) {
            g_string_free(current_words, TRUE);
            return;
        }
        g_string_append(current_words, temp_words->str);
        g_string_free(temp_words, TRUE);
    } else
        g_string_append(current_words, _("Statistiken:"));

    gtk_label_set_text(global_label, current_words->str);
    g_string_free(current_words, TRUE);
    update_button_and_Lable_states();

}

// Funktion zum Aktualisieren des Labels mit den nächsten Wörtern
void update_label_with_next_word() {
    if (make_statistics && current_word_index < total_words) {
        auto end_time = std::chrono::steady_clock::now();  // Endzeit erfassen
        std::chrono::duration<double> elapsed_seconds = end_time - start_time;

        if (current_word_index > 0) {
            GString *current_words = g_string_new(NULL);

            if (current_words == NULL) {
                return;
            }
            current_words=list_current_words();

            std::string current_words_str(current_words->str);
            word_times.push_back({current_words_str, elapsed_seconds.count()});
            g_string_free(current_words, TRUE);
        }
    }

    if (words != NULL && current_word_index < total_words + make_statistics) {
        if (current_word_index < total_words) {
            int words_per_time = gtk_spin_button_get_value_as_int(global_labelWortsPerTimeSpinn);
            current_word_index += words_per_time;
        } else {
            current_word_index++;
        }

        update_displaed_word();
    }

    if (make_statistics && current_word_index < total_words) {
        start_time = std::chrono::steady_clock::now();  // Startzeit erfassen
    }
}

void update_label_with_previous_word() {
    if (words != NULL && current_word_index > 1) {
        int words_per_time = gtk_spin_button_get_value_as_int(global_labelWortsPerTimeSpinn);
        int removal_words= words_per_time;

        if (current_word_index > total_words) {
            removal_words =total_words % words_per_time;
            if(removal_words==0)
                removal_words= words_per_time;
        }

        current_word_index -= removal_words;
        if (current_word_index < 0) {
            current_word_index = 0;
        }
        update_displaed_word();
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

std::vector<std::pair<double, int>> scores;
GtkWidget *grid;
std::map<int, GtkWidget*> word_labels;
std::map<int, GtkWidget*> time_labels;
std::map<int, GtkWidget*> relative_time_labels;
std::map<int, GtkWidget*> score_labels;
// Identifiziere die drei niedrigsten Scores
std::set<int> lowest_indices;


void update_colors(GtkSwitch *widget, gpointer data) {
    gboolean state = gtk_switch_get_active(widget);

    for (int i = 0; i < scores.size(); ++i) {
        int index = scores[i].second;
        GtkWidget *word = word_labels[index];
        GtkWidget *time = time_labels[index];
        GtkWidget *relative_time_widget = relative_time_labels[index];
        GtkWidget *score_widget = score_labels[index];
        gchar* class_name = g_strdup_printf("score-color-%d", i);
        if (state) {
            if (lowest_indices.find(index) != lowest_indices.end()) {
                gtk_widget_remove_css_class(GTK_WIDGET(word), "low-score-style");
                gtk_widget_remove_css_class(GTK_WIDGET(time), "low-score-style");
                gtk_widget_remove_css_class(GTK_WIDGET(relative_time_widget), "low-score-style");
                gtk_widget_remove_css_class(GTK_WIDGET(score_widget), "low-score-style");
            }
            

            gtk_widget_add_css_class(GTK_WIDGET(word),  class_name);
            gtk_widget_add_css_class(GTK_WIDGET(time),  class_name);
            gtk_widget_add_css_class(GTK_WIDGET(relative_time_widget),  class_name);
            gtk_widget_add_css_class(GTK_WIDGET(score_widget),  class_name);

        } else {
            gtk_widget_remove_css_class(GTK_WIDGET(word),  class_name);
            gtk_widget_remove_css_class(GTK_WIDGET(time),  class_name);
            gtk_widget_remove_css_class(GTK_WIDGET(relative_time_widget),  class_name);
            gtk_widget_remove_css_class(GTK_WIDGET(score_widget),  class_name);
            
            if (lowest_indices.find(index) != lowest_indices.end()) {

                gtk_widget_add_css_class(GTK_WIDGET(word), "low-score-style");
                gtk_widget_add_css_class(GTK_WIDGET(time), "low-score-style");
                gtk_widget_add_css_class(GTK_WIDGET(relative_time_widget), "low-score-style");
                gtk_widget_add_css_class(GTK_WIDGET(score_widget), "low-score-style");
            }
        }
        g_free(class_name);
    }
}

void show_results_window() {
    // Berechne die Gesamtdauer
    double total_time = calculate_total_time(word_times);

    // Erstelle ein neues Fenster
    GtkWidget *results_window = gtk_window_new();
    gtk_window_set_title(GTK_WINDOW(results_window), _("Lese Zeit Ergebnisse"));
    gtk_window_set_default_size(GTK_WINDOW(results_window), 600, 300);
    
    GtkWidget *results = gtk_box_new(GTK_ORIENTATION_VERTICAL, 4);
    gtk_widget_set_margin_start(results, 4);
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
    gtk_grid_set_row_spacing(GTK_GRID(grid),6);
    gtk_grid_set_column_spacing(GTK_GRID(grid),4);

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

    GtkCssProvider *provider = gtk_css_provider_new();
    std::string css;

    for (int i = 0; i < scores.size(); ++i) {
        double percentage = static_cast<double>(i) / scores.size();
        int red = static_cast<int>(255 * (1 - percentage));
        int green = static_cast<int>(255 * percentage);
        css += g_strdup_printf(".score-color-%d { color: rgb(%d,%d,0); }\n", i, red, green);
    }

    // Füge die Regel für die niedrigen Scores hinzu
    css += ".low-score-style { color: red; }\n";

    // Lade die CSS-Regeln
    gtk_css_provider_load_from_string(provider, css.c_str());
    gtk_style_context_add_provider_for_display(
        gdk_display_get_default(),
        GTK_STYLE_PROVIDER(provider),
        GTK_STYLE_PROVIDER_PRIORITY_USER
        );

    // Aufräumen
    g_object_unref(provider);


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

            gtk_widget_add_css_class(GTK_WIDGET(word), "low-score-style");
            
            gtk_widget_add_css_class(GTK_WIDGET(time), "low-score-style");

            gtk_widget_add_css_class(GTK_WIDGET(relative_time_widget), "low-score-style");

            gtk_widget_add_css_class(GTK_WIDGET(score_widget), "low-score-style");
        }
        

        gtk_grid_attach(GTK_GRID(grid), word, 0, row, 1, 1);
        gtk_grid_attach(GTK_GRID(grid), time, 1, row, 1, 1);
        gtk_grid_attach(GTK_GRID(grid), relative_time_widget, 2, row, 1, 1);
        gtk_grid_attach(GTK_GRID(grid), score_widget, 3, row, 1, 1);

        ++row;
    }
    // Connect the switch to the update_colors function
    g_signal_connect(color_switch, "notify::active", G_CALLBACK(update_colors),NULL);
    
    // Zeige alle Widgets an
    gtk_widget_set_visible(results_window,TRUE);

}

void remove_word_timer(){
    if (timebased_next_word) {
        if (current_word_index <= total_words-1)
            gtk_widget_set_visible(GTK_WIDGET(global_top_right_button),TRUE);
        gtk_button_set_icon_name(GTK_BUTTON(global_pause_button),"gtk-media-play");
        if(timer_id > 0){
            g_source_remove(timer_id);
            timer_id = 0;}
    }
}
void update_label_with_word_by_human(gpointer user_data, bool next) {
    if(next)
        update_label_with_next_word();
    if(!next)
        update_label_with_previous_word();

    remove_word_timer();
}
void run_timer(GtkButton *button, gpointer user_data);

gboolean update_label_with_next_word_by_timer(gpointer data) {
    update_label_with_next_word();
    run_timer(NULL ,data);

    return FALSE;
}

double calculate(double m, double w,double addon) {
    return addon + (m * (pow(w * w, 1.0 / 3.0)));
}

double preCalculate(double multiplyer, double textLength){
    double addon = 0;

    if (gtk_switch_get_state(global_SwitchLongerTimeFirstWord) && current_word_index == 1) {
        double firstWordValue = gtk_spin_button_get_value(global_SpinnButtonLongerTimeOnFirstWord);
        if (firstWordValue >= 0) { // Überprüfe, ob der Wert gültig ist
            addon += firstWordValue;
        }
    }

    if (gtk_switch_get_state(global_SwitchLongerTimeOnLongWord) && gtk_spin_button_get_value(global_SpinnButtonLongerTimeOnLongWord) <= textLength) {
        double longWordMultiplier = gtk_spin_button_get_value(global_SpinnButtonLongerTimeOnLongWordMultyplyer);
        if (longWordMultiplier >= 0) { // Überprüfe, ob der Wert gültig ist
            addon += longWordMultiplier; // *textLength; // Wenn du die Multiplikation benötigst, füge sie hier hinzu
        }
    }

    return calculate(multiplyer, textLength, addon);
}

void run_timer(GtkButton *button, gpointer user_data){
    if (user_data == NULL) return;

    GtkStack *stack = GTK_STACK(user_data);
    const gchar *label_text = gtk_label_get_text(GTK_LABEL(global_label));
    if (label_text == NULL) return; // Überprüfe, ob der Text nicht null ist

    int text_length = strlen(label_text);
    int words_per_time = gtk_spin_button_get_value_as_int(global_labelWortsPerTimeSpinn);
    double time_per_word = gtk_spin_button_get_value(global_TimeToNextWordSpinn);

    if (words != NULL && current_word_index < total_words && g_strcmp0(gtk_stack_get_visible_child_name(stack), "page2") == 0 && timebased_next_word){
        gtk_widget_set_visible(GTK_WIDGET(global_top_right_button),FALSE);
        gtk_button_set_icon_name(GTK_BUTTON(global_pause_button),"gtk-media-pause");
        timer_id = g_timeout_add(preCalculate(time_per_word, text_length)*1000, update_label_with_next_word_by_timer, stack);
        if (timer_id == 0) {
            g_warning("Failed to add timer.");
        }
    }else timer_id =0;
}

void update_label_with_next_word_by_human(GtkButton *button, gpointer user_data){update_label_with_word_by_human(user_data,true);}
void update_label_with_previou_word_by_human(GtkButton *button, gpointer user_data){update_label_with_word_by_human(user_data,false);}

void on_pause_button_presed(GtkButton *button, gpointer user_data){
    if(timer_id > 0){
        remove_word_timer();
        gtk_button_set_icon_name(GTK_BUTTON(global_pause_button),"gtk-media-play");
    }else {
        run_timer(button,user_data);
        gtk_button_set_icon_name(GTK_BUTTON(global_pause_button),"gtk-media-pause");
    }

}

// Funktion zum Abrufen der Standard-Schriftart
gchar* get_default_font_name() {
    GtkSettings *settings = gtk_settings_get_default();
    gchar *font_name;
    g_object_get(settings, "gtk-font-name", &font_name, NULL);
    return font_name;
}

// Callback-Funktion, die beim Klicken auf den Button zum Wechseln zu Seite 1 aufgerufen wird
void on_switch_to_page1(GtkWidget *widget, gpointer data) {
    remove_word_timer();
    GtkStack *stack = GTK_STACK(data);
    gtk_stack_set_visible_child_name(stack, "page1");
}

// Funktion zum Anwenden der Label-Stile
void apply_label_styles() {
    const GdkRGBA *bg_color = gtk_color_dialog_button_get_rgba(global_labelBackgroundColor);
    const GdkRGBA *fg_color = gtk_color_dialog_button_get_rgba(global_labelForgroudColor);

    if(!bg_color || !fg_color){
        g_print(_("Fehler beim Abrufen der Farben.\n"));
        return;
    }

    gchar *font_desc_str = pango_font_description_to_string(gtk_font_dialog_button_get_font_desc(global_labelTextButton));
    PangoFontDescription *font_desc = pango_font_description_from_string(font_desc_str);
    const gchar *font_family = pango_font_description_get_family(font_desc);
    gint font_size = pango_font_description_get_size(font_desc) / PANGO_SCALE;

    // Erstellen der CSS-Klasse als String
    gchar *bg_color_str = g_strdup("@theme_bg_color");
    gchar *fg_color_str = g_strdup("@theme_fg_color");

    if (gtk_switch_get_active(GTK_SWITCH(global_labelBackgroundColorSwitch))) {
        g_free(bg_color_str);
        bg_color_str = gdk_rgba_to_string(bg_color);
    }

    if (gtk_switch_get_active(GTK_SWITCH(global_labelForgroudColorSwitch))) {
        g_free(fg_color_str);
        fg_color_str = gdk_rgba_to_string(fg_color);
    }

    gchar *css = g_strdup_printf(
        ".custom-label-style {"
        "  background-color: %s;"
        "  color: %s;"
        "  font-family: '%s';"
        "  font-size: %dpt;"
        "}",
        bg_color_str,
        fg_color_str,
        font_family,
        font_size
        );


    // CSS global laden und anwenden
    GtkCssProvider *provider = gtk_css_provider_new();
    gtk_css_provider_load_from_string(provider, css);
    gtk_style_context_add_provider_for_display(
        gdk_display_get_default(),
        GTK_STYLE_PROVIDER(provider),
        GTK_STYLE_PROVIDER_PRIORITY_USER
        );

    // Entfernen Sie alte Klassen, um sicherzustellen, dass der neue Stil übernommen wird
    gtk_widget_remove_css_class(GTK_WIDGET(global_label), "custom-label-style");
    
    // Wenden Sie die neue CSS-Klasse auf das Label an
    gtk_widget_add_css_class(GTK_WIDGET(global_label), "custom-label-style");

    // Aufräumen
    g_object_unref(provider);
    g_free(css);
    g_free(font_desc_str);
    g_free(bg_color_str); // Freigabe des Hintergrundfarbstrings
    g_free(fg_color_str); // Freigabe des Vordergrundfarbstrings
    pango_font_description_free(font_desc);
}
void update_to_last_word(GtkWidget *widget, gpointer data){
    current_word_index = last_word_index;
    update_displaed_word();
}

void on_choose_response(GObject *source_object, GAsyncResult *res, gpointer user_data) {
    GtkAlertDialog *dialog = GTK_ALERT_DIALOG(source_object);
    int response = gtk_alert_dialog_choose_finish(dialog, res, NULL);

    if (response == 0) {
        update_to_last_word(NULL,0);
    }
    if(timebased_next_word)
        run_timer(NULL,user_data);

    if(make_statistics&&current_word_index < total_words)
        start_time = std::chrono::steady_clock::now();  // Startzeit erfassen


    g_object_unref(dialog);
}

void ask_for_Last_Progress(GtkStack *stack) {
    GtkAlertDialog *dialog = gtk_alert_dialog_new(_("Möchten Sie zur letzten Stelle springen?"));
    const char *buttons[] = { _("Ja"), _("Nein"), NULL };
    gtk_alert_dialog_set_buttons(dialog, buttons);

    gtk_alert_dialog_choose(dialog, GTK_WINDOW(gtk_widget_get_parent(GTK_WIDGET(stack))), NULL, on_choose_response, stack);
}

// Callback-Funktion, die beim Klicken auf den Button zum Wechseln zu Seite 2 aufgerufen wird
void on_switch_to_page2(GtkWidget *widget, gpointer data) {
    GtkStack *stack = GTK_STACK(data);
    GtkTextBuffer *buffer = gtk_text_view_get_buffer(global_text_view);

    if(make_statistics)
        word_times.clear();

    make_statistics = gtk_switch_get_active(global_StatisticsSwitch);

    GtkTextIter start, end;
    gtk_text_buffer_get_start_iter(buffer, &start);
    gtk_text_buffer_get_end_iter(buffer, &end);

    char *text = gtk_text_buffer_get_text(buffer, &start, &end, FALSE);

    if (text == NULL) {
        g_print(_("Fehler beim Abrufen des Textes.\n"));
        return;
    }

    split_text_into_words(text);
    apply_label_styles();

    last_word_index = current_word_index;

    current_word_index = 1;
    if (last_text && text && strcmp(last_text, text) == 0 && last_word_index > 1 && !(last_word_index >= total_words)){
        same_text=true;
        ask_for_Last_Progress(stack);
    }else
        same_text=false;

    if (last_text)
        g_free(last_text);

    last_text = g_strdup(text);

    g_free(text);

    gboolean state = gtk_switch_get_active(global_labelProgressSwitch);
    if (state) {
        gtk_widget_set_visible(GTK_WIDGET(global_ProgressLabel),TRUE);
        gtk_widget_set_visible(GTK_WIDGET(global_ProgressBar),TRUE);
    } else {
        gtk_widget_set_visible(GTK_WIDGET(global_ProgressLabel),FALSE);
        gtk_widget_set_visible(GTK_WIDGET(global_ProgressBar),FALSE);
    }

    timebased_next_word = gtk_switch_get_active(global_TimeToNextWordSwitch);
    gtk_widget_set_visible(GTK_WIDGET(global_top_right_button),FALSE);
    gtk_button_set_icon_name(GTK_BUTTON(global_pause_button),"gtk-media-pause");

    if(timebased_next_word)
        gtk_widget_set_visible(GTK_WIDGET(global_pause_button),TRUE);
    else
        gtk_widget_set_visible(GTK_WIDGET(global_pause_button),FALSE);

    gtk_stack_set_visible_child_name(stack, "page2");


    update_displaed_word();
    if(make_statistics&&current_word_index < total_words)
        start_time = std::chrono::steady_clock::now();  // Startzeit erfassen


    if(timebased_next_word && !same_text)
        run_timer(NULL,stack);

    save_settings();
}

// Tastensignal-Callback-Funktion
gboolean on_key_press(GtkEventControllerKey *controller, guint keyval, guint keycode, GdkModifierType state, gpointer data) {
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
void on_reset_button_clicked(GtkButton *button, gpointer user_data) {
    // Setze die Standard-Hintergrund- und Schriftfarbe zurück
    GdkRGBA bg_color, fg_color;
    gdk_rgba_parse(&bg_color, "#000000");
    gdk_rgba_parse(&fg_color, "#ffffff");

    gtk_color_dialog_button_set_rgba(global_labelBackgroundColor,&bg_color);
    gtk_color_dialog_button_set_rgba(global_labelForgroudColor,&fg_color);
    gtk_switch_set_active(global_labelBackgroundColorSwitch, FALSE);
    gtk_switch_set_active(global_labelForgroudColorSwitch, FALSE);

    // Setze die Schriftart und -größe zurück
    PangoFontDescription *default_font_desc = pango_font_description_from_string(get_default_font_name());
    pango_font_description_set_size(default_font_desc, 50 * PANGO_SCALE);  // Setze Größe auf 50pt
    gtk_font_dialog_button_set_font_desc(global_labelTextButton, default_font_desc);
    pango_font_description_free(default_font_desc);

    // Setze den Fortschrittsschalter zurück
    gtk_switch_set_active(global_labelProgressSwitch, TRUE);

    // Setze die Anzahl der Wörter pro Zeitspanne zurück
    gtk_spin_button_set_value(global_labelWortsPerTimeSpinn, 1);

    // Leere das Textfeld
    GtkTextBuffer *buffer = gtk_text_view_get_buffer(global_text_view);
    if (buffer == NULL) {
        g_print(_("Fehler beim Abrufen des Textpuffers.\n"));
        return;
    }
    gtk_text_buffer_set_text(buffer, "", -1);

    update_read_and_actions_button_state(buffer, NULL); // Aktualisiere den Zustand des "Lesen"-Buttons

    gtk_spin_button_set_value(global_TimeToNextWordSpinn, 0.9);

    gtk_switch_set_active(global_TimeToNextWordSwitch, FALSE);

    gtk_switch_set_active(global_StatisticsSwitch, FALSE);

    gtk_switch_set_active(global_SwitchLongerTimeOnLongWord, FALSE);
    gtk_spin_button_set_value(global_SpinnButtonLongerTimeOnLongWord, 15);
    gtk_switch_set_active(global_SwitchLongerTimeFirstWord, TRUE);
    gtk_spin_button_set_value(global_SpinnButtonLongerTimeOnFirstWord, 0.3);

    gtk_spin_button_set_value(global_SpinnButtonLongerTimeOnLongWordMultyplyer, 0.02);

}

void switchtoggle(GtkSwitch *widget, GParamSpec *pspec, gpointer data){
    if(gtk_switch_get_active(widget)){
        gtk_widget_set_sensitive(GTK_WIDGET(data), TRUE);
    }else{
        gtk_widget_set_sensitive(GTK_WIDGET(data), FALSE);
    }
}

void SwitchLongerTimeOnLongWordToggle(GtkSwitch *widget, gpointer data){
    if(gtk_switch_get_active(global_SwitchLongerTimeOnLongWord)){
        gtk_widget_set_sensitive(GTK_WIDGET(global_SpinnButtonLongerTimeOnLongWord), TRUE);
        gtk_widget_set_sensitive(GTK_WIDGET(global_SpinnButtonLongerTimeOnLongWordMultyplyer), TRUE);
    }else{
        gtk_widget_set_sensitive(GTK_WIDGET(global_SpinnButtonLongerTimeOnLongWord), FALSE);
        gtk_widget_set_sensitive(GTK_WIDGET(global_SpinnButtonLongerTimeOnLongWordMultyplyer), FALSE);
    }

}

void on_copy_button_clicked(GtkButton *button, gpointer data) {
    GtkTextBuffer *buffer = gtk_text_view_get_buffer(global_text_view);
    if (buffer == NULL) {
        g_print(_("Fehler beim Abrufen des Textpuffers.\n"));
        return;
    }
    GtkTextIter start, end;
    gtk_text_buffer_get_start_iter(buffer, &start);
    gtk_text_buffer_get_end_iter(buffer, &end);
    gtk_text_buffer_select_range(buffer, &start, &end);

    GdkClipboard *clipboard = gdk_display_get_clipboard(gdk_display_get_default());
    gtk_text_buffer_copy_clipboard(buffer, clipboard);
}

void on_paste_button_clicked(GtkButton *button, gpointer data) {
    GtkTextBuffer *buffer = gtk_text_view_get_buffer(global_text_view);
    if (buffer == NULL) {
        g_print(_("Fehler beim Abrufen des Textpuffers.\n"));
        return;
    }
    GdkClipboard *clipboard = gdk_display_get_clipboard(gdk_display_get_default());
    gtk_text_buffer_paste_clipboard(buffer, clipboard, NULL, TRUE);
}

void on_clear_button_clicked(GtkButton *button, gpointer data) {
    GtkTextBuffer *buffer = gtk_text_view_get_buffer(global_text_view);
    if (buffer == NULL) {
        g_print(_("Fehler beim Abrufen des Textpuffers.\n"));
        return;
    }
    gtk_text_buffer_set_text(buffer, "", -1);
}

void on_undo_button_clicked(GtkButton *button, gpointer data) {
    GtkTextBuffer *buffer = gtk_text_view_get_buffer(global_text_view);
    if (buffer == NULL) {
        g_print(_("Fehler beim Abrufen des Textpuffers.\n"));
        return;
    }
    gtk_text_buffer_undo(buffer);
}

void on_quit_activate(GSimpleAction *action, GVariant *parameter, gpointer app) {
    gtk_window_close(GTK_WINDOW(app));
}

void on_about_activate(GSimpleAction *action, GVariant *parameter, gpointer user_data) {
    GtkWidget *about_dialog = gtk_about_dialog_new();
    gtk_about_dialog_set_program_name(GTK_ABOUT_DIALOG(about_dialog), "FastReader");
    gtk_about_dialog_set_version(GTK_ABOUT_DIALOG(about_dialog), FAST_READER_VERSION);
    gtk_about_dialog_set_comments(GTK_ABOUT_DIALOG(about_dialog), _("Fast Reader helps you read quickly by displaying only one word at a time"));
    gtk_about_dialog_set_website(GTK_ABOUT_DIALOG(about_dialog), "https://github.com/Quantum-mutnauQ/Fast_Reader_GTK");
    gtk_about_dialog_set_license_type(GTK_ABOUT_DIALOG(about_dialog), GTK_LICENSE_GPL_2_0);

#ifdef ICON_NAME
    gtk_about_dialog_set_logo_icon_name(GTK_ABOUT_DIALOG(about_dialog), ICON_NAME);
#else
    gtk_about_dialog_set_logo_icon_name(GTK_ABOUT_DIALOG(about_dialog), "fastreader");
#endif

    const char *authors[] = {
        "Quantum-mutnauQ",
        NULL
    };
    gtk_about_dialog_set_authors(GTK_ABOUT_DIALOG(about_dialog), authors);

    const char *translators = "albanobattistella";
    gtk_about_dialog_set_translator_credits(GTK_ABOUT_DIALOG(about_dialog), translators);

    const char *documenters[] = {
        "Quaste42",
        NULL
    };
    gtk_about_dialog_set_documenters(GTK_ABOUT_DIALOG(about_dialog), documenters);

    gtk_window_present(GTK_WINDOW(about_dialog));
}

void toggle_fullscreen(GSimpleAction *action, GVariant *parameter, gpointer window) {
    if (gtk_window_is_fullscreen(GTK_WINDOW(window)))
        gtk_window_unfullscreen(GTK_WINDOW(window));
    else
        gtk_window_fullscreen(GTK_WINDOW(window));
}

void reset_Backreound(GSimpleAction *action, GVariant *parameter, gpointer user_data){
    GdkRGBA bg_color;
    gdk_rgba_parse(&bg_color, "#000000");

    gtk_color_dialog_button_set_rgba(global_labelBackgroundColor,&bg_color);
}

void reset_Forground(GSimpleAction *action, GVariant *parameter, gpointer user_data){
    GdkRGBA  fg_color;
    gdk_rgba_parse(&fg_color, "#ffffff");

    gtk_color_dialog_button_set_rgba(global_labelForgroudColor,&fg_color);
}
void reset_Backreound_Switsh(GSimpleAction *action, GVariant *parameter, gpointer user_data){
    gtk_switch_set_active(global_labelBackgroundColorSwitch, FALSE);
}

void reset_Forground_Switsh(GSimpleAction *action, GVariant *parameter, gpointer user_data){
    gtk_switch_set_active(global_labelForgroudColorSwitch, FALSE);
}
void reset_Font(GSimpleAction *action, GVariant *parameter, gpointer user_data){
    PangoFontDescription *default_font_desc = pango_font_description_from_string(get_default_font_name());
    pango_font_description_set_size(default_font_desc, 50 * PANGO_SCALE);  // Setze Größe auf 50pt
    gtk_font_dialog_button_set_font_desc(global_labelTextButton, default_font_desc);
    pango_font_description_free(default_font_desc);
}
void reset_showProgress(GSimpleAction *action, GVariant *parameter, gpointer user_data){
    gtk_switch_set_active(global_labelProgressSwitch, TRUE);
}
void reset_multibleWords(GSimpleAction *action, GVariant *parameter, gpointer user_data){
    gtk_spin_button_set_value(global_labelWortsPerTimeSpinn, 1);
}
void reset_TimeBasetWordPredictions(GSimpleAction *action, GVariant *parameter, gpointer user_data){
    gtk_switch_set_active(global_TimeToNextWordSwitch, FALSE);
}
void reset_TimeBasetWordPredictionsTime(GSimpleAction *action, GVariant *parameter, gpointer user_data){
    gtk_spin_button_set_value(global_TimeToNextWordSpinn, 0.9);
}
void reset_LongLongWortLongerTime(GSimpleAction *action, GVariant *parameter, gpointer user_data){
    gtk_switch_set_active(global_SwitchLongerTimeOnLongWord, FALSE);
}
void reset_LongLongWortLongerTimeLetters(GSimpleAction *action, GVariant *parameter, gpointer user_data){
    gtk_spin_button_set_value(global_SpinnButtonLongerTimeOnLongWord, 15);
}
void reset_LongLongWortLongerTime_Time(GSimpleAction *action, GVariant *parameter, gpointer user_data){
    gtk_spin_button_set_value(global_SpinnButtonLongerTimeOnLongWordMultyplyer, 0.02);
}
void reset_ExtraTimeForFirstWord(GSimpleAction *action, GVariant *parameter, gpointer user_data){
    gtk_switch_set_active(global_SwitchLongerTimeFirstWord, TRUE);
}
void reset_ExtraTimeForFirstWord_Time(GSimpleAction *action, GVariant *parameter, gpointer user_data){
    gtk_spin_button_set_value(global_SpinnButtonLongerTimeOnFirstWord, 0.3);
}
void reset_createStatistics(GSimpleAction *action, GVariant *parameter, gpointer user_data){
    gtk_switch_set_active(global_StatisticsSwitch, FALSE);
}
void reset_TextBox(GSimpleAction *action, GVariant *parameter, gpointer user_data){
    GtkTextBuffer *buffer = gtk_text_view_get_buffer(global_text_view);
    if (buffer == NULL) {
        g_print(_("Fehler beim Abrufen des Textpuffers.\n"));
        return;
    }
    gtk_text_buffer_set_text(buffer, "", -1);
    update_read_and_actions_button_state(buffer, NULL); // Aktualisiere den Zustand des "Lesen"-Buttons
}

void add_action(const gchar* name, GCallback callback, gpointer user_data,GtkApplication *app){
    GSimpleAction *action = g_simple_action_new(name, NULL);
    if (!action) {g_printerr(_("Fehler beim Erstellen der Aktion.\n"));return;}
    g_signal_connect(action, "activate", callback,user_data);
    g_action_map_add_action(G_ACTION_MAP(app), G_ACTION(action));
}

GtkWidget *create_menu_bar(GtkApplication *app, GtkWidget *window) {
    // Actions definieren und mit Callbacks verbinden

    add_action("quit",G_CALLBACK(on_quit_activate),window,app);
    add_action("about", G_CALLBACK(on_about_activate), NULL,app);
    add_action("toggle-fullscreen", G_CALLBACK(toggle_fullscreen), window, app);
    add_action("reset_Backreound",G_CALLBACK(reset_Backreound), window, app);
    add_action("reset_Forground", G_CALLBACK(reset_Forground), window,app);
    add_action("reset_Backreound_Switch",G_CALLBACK(reset_Backreound_Switsh), window,app);
    add_action("reset_Forground_Switch",G_CALLBACK(reset_Forground_Switsh), window,app);
    add_action("reset_Font",G_CALLBACK(reset_Font), NULL,app);
    add_action("reset_showProgress",G_CALLBACK(reset_showProgress), NULL,app);
    add_action("reset_multibleWords",G_CALLBACK(reset_multibleWords), NULL,app);
    add_action("reset_TimeBasetWordPredictions", G_CALLBACK(reset_TimeBasetWordPredictions), NULL, app);
    add_action("reset_TimeBasetWordPredictionsTime",G_CALLBACK(reset_TimeBasetWordPredictionsTime), NULL,app);
    add_action("reset_LongLongWortLongerTime",G_CALLBACK(reset_LongLongWortLongerTime), NULL,app);
    add_action("reset_LongLongWortLongerTimeLetters",G_CALLBACK(reset_LongLongWortLongerTimeLetters), NULL,app);
    add_action("reset_LongLongWortLongerTime_Time",G_CALLBACK(reset_LongLongWortLongerTime_Time), NULL,app);
    add_action("reset_ExtraTimeForFirstWord",G_CALLBACK(reset_ExtraTimeForFirstWord), NULL,app);
    add_action("reset_ExtraTimeForFirstWord_Time",G_CALLBACK(reset_ExtraTimeForFirstWord_Time), NULL,app);
    add_action("reset_createStatistics",G_CALLBACK(reset_createStatistics), NULL,app);
    add_action("reset_TextBox",G_CALLBACK(reset_TextBox), NULL,app);

    const gchar *accels[] = { "F11", NULL };
    gtk_application_set_accels_for_action(app, "app.toggle-fullscreen", accels);

    const gchar *quit_accels[] = { "<Primary>q", NULL };
    gtk_application_set_accels_for_action(app, "app.quit", quit_accels);

    // Hauptmenü
    GMenu *menu_model = g_menu_new();

    // Datei-Menü
    GMenu *file_menu = g_menu_new();

    // Untermenü: Zurücksetzen
    GMenu *reset_menu = g_menu_new();
    GMenu *background_submenu = g_menu_new();
    g_menu_append(background_submenu, _("Schalter"), "app.reset_Backreound_Switch");
    g_menu_append(background_submenu, _("Farbe"), "app.reset_Backreound");

    GMenu *foreground_submenu = g_menu_new();
    g_menu_append(foreground_submenu, _("Schalter"), "app.reset_Forground_Switch");
    g_menu_append(foreground_submenu, _("Farbe"), "app.reset_Forground");

    g_menu_append_submenu(reset_menu, _("Hintergrund:"), G_MENU_MODEL(background_submenu));
    g_menu_append_submenu(reset_menu, _("Schrift:"), G_MENU_MODEL(foreground_submenu));

    g_menu_append(reset_menu, _("Schrift Größe:"), "app.reset_Font");
    g_menu_append(reset_menu, _("Fortschritt Zeigen:"), "app.reset_showProgress");
    g_menu_append(reset_menu, _("Angezeigte wörter:"), "app.reset_multibleWords");

    // Zeitbasiertes Wort – Untermenü
    GMenu *zeitwort_menu = g_menu_new();
    g_menu_append(zeitwort_menu, _("Zeitbassirt nechstes Wort:"), "app.reset_TimeBasetWordPredictions");
    g_menu_append(zeitwort_menu, _("Zeit (sec):"), "app.reset_TimeBasetWordPredictionsTime");
    GMenuItem *zeitwort_item = g_menu_item_new_submenu(_("Zeitbassirt nechstes Wort:"), G_MENU_MODEL(zeitwort_menu));
    g_menu_append_item(reset_menu, zeitwort_item);

    // Längere Zeit bei sehr langen Wörtern – Untermenü
    GMenu *langezeit_menu = g_menu_new();
    g_menu_append(langezeit_menu, _("Längere Zeit Bei sehrlangen Wörtern:"), "app.reset_LongLongWortLongerTime");
    g_menu_append(langezeit_menu, _("→ Wortlänge in Bustaben:"), "app.reset_LongLongWortLongerTimeLetters");
    g_menu_append(langezeit_menu, _("→ + Zeit: "), "app.reset_LongLongWortLongerTime_Time");
    GMenuItem *langezeit_item = g_menu_item_new_submenu(_("Längere Zeit Bei sehrlangen Wörtern:"), G_MENU_MODEL(langezeit_menu));
    g_menu_append_item(reset_menu, langezeit_item);

    // Extrazeit für erstes Wort – Untermenü
    GMenu *extrazeit_menu = g_menu_new();
    g_menu_append(extrazeit_menu, _("Exterzeit für erstes Wort:"), "app.reset_ExtraTimeForFirstWord");
    g_menu_append(extrazeit_menu, _("→ dazu addierte Zeit(Sekunden):"), "app.reset_ExtraTimeForFirstWord_Time");
    GMenuItem *extrazeit_item = g_menu_item_new_submenu(_("Exterzeit für erstes Wort:"), G_MENU_MODEL(extrazeit_menu));
    g_menu_append_item(reset_menu, extrazeit_item);

    g_menu_append(reset_menu, _("Statistiken erheben:"), "app.reset_createStatistics");
    g_menu_append(reset_menu, _("Text Box"), "app.reset_TextBox");

    GMenuItem *reset_item = g_menu_item_new_submenu(_("Zurücksetzen"), G_MENU_MODEL(reset_menu));
    g_menu_append_item(file_menu, reset_item);
    g_menu_append(file_menu, _("Schließen"), "app.quit");

    GMenuItem *file_item = g_menu_item_new_submenu(_("Datei"), G_MENU_MODEL(file_menu));
    g_menu_append_item(menu_model, file_item);

    // Ansicht-Menü
    GMenu *ansicht_menu = g_menu_new();
    g_menu_append(ansicht_menu, _("Vollbild"), "app.toggle-fullscreen");
    GMenuItem *ansicht_item = g_menu_item_new_submenu(_("Ansicht"), G_MENU_MODEL(ansicht_menu));
    g_menu_append_item(menu_model, ansicht_item);

    // Hilfe-Menü
    GMenu *help_menu = g_menu_new();
    g_menu_append(help_menu, _("Über"), "app.about");
    GMenuItem *help_item = g_menu_item_new_submenu(_("Hilfe"), G_MENU_MODEL(help_menu));
    g_menu_append_item(menu_model, help_item);

    // Menüleiste erzeugen
    GtkWidget *menu_bar = gtk_popover_menu_bar_new_from_model(G_MENU_MODEL(menu_model));
    return menu_bar;
}
// Funktion zum Erstellen von Seite 1
GtkWidget *create_page1(GtkStack *stack, GtkWidget *window) {
    GtkWidget *page1 = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);

    GtkWidget *menu_bar = create_menu_bar(gtk_window_get_application(GTK_WINDOW(window)),window);

    gtk_box_append(GTK_BOX(page1), menu_bar);

    GtkWidget *settings_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 4);
    gtk_widget_set_margin_start(settings_box, 4);

    GtkWidget *label = gtk_label_new(_("Einstellungen:"));
    gtk_widget_set_halign(label, GTK_ALIGN_CENTER);

    GtkWidget *Backround = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 4);
    GtkWidget *labelBackground = gtk_label_new(_("Hintergrund:"));
    global_labelBackgroundColorSwitch=GTK_SWITCH(gtk_switch_new());
    gtk_switch_set_active(global_labelBackgroundColorSwitch, FALSE);
    global_labelBackgroundColor = GTK_COLOR_DIALOG_BUTTON(gtk_color_dialog_button_new(gtk_color_dialog_new()));

    GtkWidget *Forgroud = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 4);
    GtkWidget *labelForgroud = gtk_label_new(_("Schrift:"));
    global_labelForgroudColorSwitch=GTK_SWITCH(gtk_switch_new());
    gtk_switch_set_active(global_labelForgroudColorSwitch, FALSE);
    global_labelForgroudColor = GTK_COLOR_DIALOG_BUTTON(gtk_color_dialog_button_new(gtk_color_dialog_new()));

    GtkWidget *Text = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 4);
    GtkWidget *labelTextSize = gtk_label_new(_("Schrift Größe:"));
    global_labelTextButton = GTK_FONT_DIALOG_BUTTON(gtk_font_dialog_button_new(gtk_font_dialog_new()));

    GtkWidget *Progress = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 4);
    GtkWidget *labelProgress = gtk_label_new(_("Fortschritt Zeigen:"));
    global_labelProgressSwitch = GTK_SWITCH(gtk_switch_new());

    GtkAdjustment *adjustment = gtk_adjustment_new(1, 1, DBL_MAX, 1, 10, 1);
    GtkWidget *WortsPerTime = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 2);
    GtkWidget *labelWortsPerTime = gtk_label_new(_("Angezeigte wörter:"));
    global_labelWortsPerTimeSpinn = GTK_SPIN_BUTTON(gtk_spin_button_new(adjustment, 1, 0));

    GtkAdjustment *adjustment2 = gtk_adjustment_new(0.9, 0.001, DBL_MAX, 0.05, 0.5, 1);

    GtkWidget *TimeToNextWord = gtk_box_new(GTK_ORIENTATION_HORIZONTAL,4);
    GtkWidget *labelTimeToNextWord = gtk_label_new(_("Zeitbassirt nechstes Wort:"));
    global_TimeToNextWordSwitch = GTK_SWITCH(gtk_switch_new());
    GtkWidget *label2TimeToNextWord = gtk_label_new(_("Zeit (sec):"));
    global_TimeToNextWordSpinn = GTK_SPIN_BUTTON(gtk_spin_button_new(adjustment2, 1, 3));

    GtkWidget *advacedWordPredictinsSettings=gtk_box_new(GTK_ORIENTATION_VERTICAL,4);
    GtkWidget *TimeToNextWordAdvandedSettings = gtk_expander_new(_("Erweiterte Einstellugen für Zeitbassiertes nächstes Wort"));
    gtk_expander_set_child(GTK_EXPANDER(TimeToNextWordAdvandedSettings),advacedWordPredictinsSettings);
    gtk_widget_set_margin_start(advacedWordPredictinsSettings, 20);

    const char*  labelLongerTimeOnLongWordToolTip=_("Hier kann die gewünschte zusätzliche Zeit angegeben werden, die das Wort länger bleiben soll (in Sekunden)");

    GtkWidget *longWordLongTime = gtk_box_new(GTK_ORIENTATION_HORIZONTAL,4);
    GtkWidget *labelLongerTimeOnLongWord = gtk_label_new(_("Längere Zeit Bei sehrlangen Wörtern:"));
    global_SwitchLongerTimeOnLongWord = GTK_SWITCH(gtk_switch_new());
    gtk_switch_set_active(global_SwitchLongerTimeOnLongWord, FALSE);
    gtk_box_append(GTK_BOX(longWordLongTime), labelLongerTimeOnLongWord);
    gtk_box_append(GTK_BOX(longWordLongTime), GTK_WIDGET(global_SwitchLongerTimeOnLongWord));
    gtk_widget_set_tooltip_text(longWordLongTime,labelLongerTimeOnLongWordToolTip);

    GtkAdjustment *adjustment3 = gtk_adjustment_new(15, 1, DBL_MAX, 1, 10, 1);
    GtkWidget *longWordLongTimeAdvanced = gtk_box_new(GTK_ORIENTATION_HORIZONTAL,4);
    GtkWidget *label2LongerTimeOnLongWord = gtk_label_new(_("→ Wortlänge in Bustaben:"));
    gtk_widget_set_margin_start(label2LongerTimeOnLongWord, 10);
    global_SpinnButtonLongerTimeOnLongWord = GTK_SPIN_BUTTON(gtk_spin_button_new(adjustment3, 1, 0));
    gtk_box_append(GTK_BOX(longWordLongTimeAdvanced),label2LongerTimeOnLongWord);
    gtk_box_append(GTK_BOX(longWordLongTimeAdvanced), GTK_WIDGET(global_SpinnButtonLongerTimeOnLongWord));
    gtk_widget_set_tooltip_text(longWordLongTimeAdvanced,labelLongerTimeOnLongWordToolTip);

    GtkAdjustment *adjustment4 = gtk_adjustment_new(0.02, 0.001, DBL_MAX, 0.05, 0.5, 1);
    GtkWidget *longWordLongTimeMultiplyer = gtk_box_new(GTK_ORIENTATION_HORIZONTAL,4);
    GtkWidget *label3LongerTimeOnLongWord = gtk_label_new(_("→ + Zeit: "));
    gtk_widget_set_margin_start(longWordLongTimeMultiplyer, 10);
    global_SpinnButtonLongerTimeOnLongWordMultyplyer = GTK_SPIN_BUTTON(gtk_spin_button_new(adjustment4, 1, 3));
    gtk_box_append(GTK_BOX(longWordLongTimeMultiplyer),label3LongerTimeOnLongWord);
    gtk_box_append(GTK_BOX(longWordLongTimeMultiplyer), GTK_WIDGET(global_SpinnButtonLongerTimeOnLongWordMultyplyer));
    gtk_widget_set_tooltip_text(longWordLongTimeMultiplyer,labelLongerTimeOnLongWordToolTip);

    const char* longerTimeOnFirstWord = _("Wartezeit, bis das Lese Programm startet. Ermöglicht so, dass das erste kurze Worte nicht zu schnell verschwinden.");
    GtkWidget *LongWordFistTime = gtk_box_new(GTK_ORIENTATION_HORIZONTAL,4);
    GtkWidget *labelLongerTimeFirstWord = gtk_label_new(_("Exterzeit für erstes Wort:"));
    global_SwitchLongerTimeFirstWord = GTK_SWITCH(gtk_switch_new());
    gtk_switch_set_active(global_SwitchLongerTimeFirstWord, TRUE);
    gtk_box_append(GTK_BOX(LongWordFistTime), labelLongerTimeFirstWord);
    gtk_box_append(GTK_BOX(LongWordFistTime), GTK_WIDGET(global_SwitchLongerTimeFirstWord));
    gtk_widget_set_tooltip_text(LongWordFistTime,longerTimeOnFirstWord);

    GtkAdjustment *adjustment5= gtk_adjustment_new(0.3, 0.001, DBL_MAX, 0.05, 0.5, 1);
    GtkWidget *LongWordFistTimeAdvanced = gtk_box_new(GTK_ORIENTATION_HORIZONTAL,4);
    GtkWidget *labelLongerTimeFirstWordSetting = gtk_label_new(_("→ dazu addierte Zeit(Sekunden):"));
    global_SpinnButtonLongerTimeOnFirstWord = GTK_SPIN_BUTTON(gtk_spin_button_new(adjustment5, 1, 3));
    gtk_widget_set_margin_start(LongWordFistTimeAdvanced, 10);
    gtk_box_append(GTK_BOX(LongWordFistTimeAdvanced), labelLongerTimeFirstWordSetting);
    gtk_box_append(GTK_BOX(LongWordFistTimeAdvanced), GTK_WIDGET(global_SpinnButtonLongerTimeOnFirstWord));
    gtk_widget_set_tooltip_text(LongWordFistTimeAdvanced,longerTimeOnFirstWord);

    GtkWidget *Statistics = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 4);
    GtkWidget *labelStatistics = gtk_label_new(_("Statistiken erheben:"));
    global_StatisticsSwitch = GTK_SWITCH(gtk_switch_new());
    gtk_switch_set_active(global_StatisticsSwitch, FALSE);

    gtk_widget_set_tooltip_text(TimeToNextWord, _("Die Zeit wird berechnet durch:\nZeit−Pro−Wort×(₃√Wort−Länge )"));
    GtkWidget *ResetButton = gtk_button_new_with_label(_("Zurücksetzen"));

    gtk_switch_set_active(GTK_SWITCH(global_labelProgressSwitch), TRUE);

    // Erstelle eine Schriftartbeschreibung mit Standardgröße 50
    PangoFontDescription *default_font_desc = pango_font_description_from_string(get_default_font_name());
    pango_font_description_set_size(default_font_desc, 50 * PANGO_SCALE);  // Setze Größe auf 50pt
    gtk_font_dialog_button_set_font_desc(global_labelTextButton, default_font_desc);

    pango_font_description_free(default_font_desc);

    global_button_read = GTK_BUTTON(gtk_button_new_with_label(_("Lesen")));
    global_text_view = GTK_TEXT_VIEW(gtk_text_view_new());
    GtkWidget *scrolled_window = gtk_scrolled_window_new();
    gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(scrolled_window), GTK_WIDGET(global_text_view));
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolled_window), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);

    // Standard-Hintergrund- und Schriftfarbe abrufen und setzen
    GdkRGBA bg_color, fg_color;
    gdk_rgba_parse(&bg_color, "#000000");
    gdk_rgba_parse(&fg_color, "#ffffff");
    gtk_color_dialog_button_set_rgba(global_labelBackgroundColor, &bg_color);
    gtk_color_dialog_button_set_rgba(global_labelForgroudColor, &fg_color);

    gtk_box_append(GTK_BOX(settings_box), label);
    gtk_box_append(GTK_BOX(Backround), labelBackground);
    gtk_box_append(GTK_BOX(Backround), GTK_WIDGET(global_labelBackgroundColorSwitch));
    gtk_box_append(GTK_BOX(Backround), GTK_WIDGET(global_labelBackgroundColor));
    gtk_box_append(GTK_BOX(settings_box), Backround);

    gtk_box_append(GTK_BOX(Forgroud), labelForgroud);
    gtk_box_append(GTK_BOX(Forgroud), GTK_WIDGET(global_labelForgroudColorSwitch));
    gtk_box_append(GTK_BOX(Forgroud), GTK_WIDGET(global_labelForgroudColor));
    gtk_box_append(GTK_BOX(settings_box), Forgroud);

    gtk_box_append(GTK_BOX(Text), labelTextSize);
    gtk_box_append(GTK_BOX(Text), GTK_WIDGET(global_labelTextButton));
    gtk_box_append(GTK_BOX(settings_box), Text);

    gtk_box_append(GTK_BOX(Progress), labelProgress);
    gtk_box_append(GTK_BOX(Progress), GTK_WIDGET(global_labelProgressSwitch));
    gtk_box_append(GTK_BOX(settings_box), Progress);

    gtk_box_append(GTK_BOX(WortsPerTime), labelWortsPerTime);
    gtk_box_append(GTK_BOX(WortsPerTime), GTK_WIDGET(global_labelWortsPerTimeSpinn));
    gtk_box_append(GTK_BOX(settings_box), WortsPerTime);

    gtk_box_append(GTK_BOX(TimeToNextWord), labelTimeToNextWord);
    gtk_box_append(GTK_BOX(TimeToNextWord), GTK_WIDGET(global_TimeToNextWordSwitch));
    gtk_box_append(GTK_BOX(TimeToNextWord), label2TimeToNextWord);
    gtk_box_append(GTK_BOX(TimeToNextWord), GTK_WIDGET(global_TimeToNextWordSpinn));
    gtk_box_append(GTK_BOX(settings_box), TimeToNextWord);

    gtk_box_append(GTK_BOX(advacedWordPredictinsSettings), GTK_WIDGET(longWordLongTime));
    gtk_box_append(GTK_BOX(advacedWordPredictinsSettings), GTK_WIDGET(longWordLongTimeAdvanced));
    gtk_box_append(GTK_BOX(advacedWordPredictinsSettings), GTK_WIDGET(longWordLongTimeMultiplyer));
    gtk_box_append(GTK_BOX(advacedWordPredictinsSettings), GTK_WIDGET(LongWordFistTime));
    gtk_box_append(GTK_BOX(advacedWordPredictinsSettings), GTK_WIDGET(LongWordFistTimeAdvanced));

    gtk_box_append(GTK_BOX(settings_box), TimeToNextWordAdvandedSettings);

    gtk_box_append(GTK_BOX(Statistics), labelStatistics);
    gtk_box_append(GTK_BOX(Statistics), GTK_WIDGET(global_StatisticsSwitch));
    gtk_box_append(GTK_BOX(settings_box), Statistics);

    gtk_box_append(GTK_BOX(page1), settings_box);

    gtk_box_append(GTK_BOX(page1), ResetButton);


    // Box für die vier Buttons erstellen
    GtkWidget *button_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 4);

    const gchar *css =
        ".custom-button {"
        "    background-color: transparent;"
        "    border: 1px solid transparent;"
        "}"
        ".custom-button:hover {"
        "    border: 1px solid;"
        "}";

    GtkCssProvider *provider = gtk_css_provider_new();
    gtk_css_provider_load_from_string(provider, css); // Der vierte Parameter ist in GTK-4 nicht mehr notwendig
    gtk_style_context_add_provider_for_display(gdk_display_get_default(),
                                               GTK_STYLE_PROVIDER(provider),
                                               GTK_STYLE_PROVIDER_PRIORITY_USER);
    g_object_unref(provider);


    global_copy_button = gtk_button_new_from_icon_name("edit-copy");
    gtk_widget_set_tooltip_text(global_copy_button, _("Copy"));
    gtk_widget_add_css_class(global_copy_button, "custom-button");

    GtkWidget *paste_button = gtk_button_new_from_icon_name("edit-paste");
    gtk_widget_set_tooltip_text(paste_button, _("Paste"));
    gtk_widget_add_css_class(paste_button, "custom-button");

    global_clear_button = gtk_button_new_from_icon_name("edit-clear");
    gtk_widget_set_tooltip_text(global_clear_button, _("Clear"));
    gtk_widget_add_css_class(global_clear_button, "custom-button");

    GtkWidget *undo_button = gtk_button_new_from_icon_name("edit-undo");
    gtk_widget_set_tooltip_text(undo_button, _("Undo"));
    gtk_widget_add_css_class(undo_button, "custom-button");


    gtk_box_append(GTK_BOX(button_box), global_copy_button);
    gtk_box_append(GTK_BOX(button_box), paste_button);
    gtk_box_append(GTK_BOX(button_box), global_clear_button);
    gtk_box_append(GTK_BOX(button_box), undo_button);

    // Overlay erstellen
    GtkWidget *overlay = gtk_overlay_new();
    gtk_overlay_set_child(GTK_OVERLAY(overlay), scrolled_window);

    // Die Buttons im Overlay platzieren
    gtk_overlay_add_overlay(GTK_OVERLAY(overlay), button_box);

    // Buttons am rechten unteren Rand positionieren
    gtk_widget_set_valign(button_box, GTK_ALIGN_END);
    gtk_widget_set_halign(button_box, GTK_ALIGN_END);

    gtk_widget_set_vexpand(scrolled_window, TRUE);

    gtk_box_append(GTK_BOX(page1), overlay);
    gtk_widget_set_hexpand(overlay, TRUE);
    gtk_widget_set_vexpand(overlay, TRUE);

    gtk_box_append(GTK_BOX(page1), GTK_WIDGET(global_button_read));
    gtk_widget_set_hexpand(GTK_WIDGET(global_button_read), FALSE);
    gtk_widget_set_vexpand(GTK_WIDGET(global_button_read), FALSE);

    update_read_and_actions_button_state(gtk_text_view_get_buffer(GTK_TEXT_VIEW(global_text_view)), NULL);

    g_signal_connect(GTK_WIDGET(global_button_read), "clicked", G_CALLBACK(on_switch_to_page2), stack);
    g_signal_connect(gtk_text_view_get_buffer(GTK_TEXT_VIEW(global_text_view)), "changed", G_CALLBACK(update_read_and_actions_button_state), NULL);
    g_signal_connect(ResetButton, "clicked", G_CALLBACK(on_reset_button_clicked), window);
    g_signal_connect(global_TimeToNextWordSwitch, "notify::active", G_CALLBACK(switchtoggle), global_TimeToNextWordSpinn);
    g_signal_connect(global_SwitchLongerTimeOnLongWord, "notify::active", G_CALLBACK(SwitchLongerTimeOnLongWordToggle), NULL);
    g_signal_connect(global_SwitchLongerTimeFirstWord, "notify::active", G_CALLBACK(switchtoggle), global_SpinnButtonLongerTimeOnFirstWord);
    g_signal_connect(global_labelBackgroundColorSwitch, "notify::active", G_CALLBACK(switchtoggle), global_labelBackgroundColor);
    g_signal_connect(global_labelForgroudColorSwitch, "notify::active", G_CALLBACK(switchtoggle), global_labelForgroudColor);

    // Signalhandler für die neuen Buttons hinzufügen
    g_signal_connect(global_copy_button, "clicked", G_CALLBACK(on_copy_button_clicked), global_copy_button);
    g_signal_connect(paste_button, "clicked", G_CALLBACK(on_paste_button_clicked), paste_button);
    g_signal_connect(global_clear_button, "clicked", G_CALLBACK(on_clear_button_clicked), global_clear_button);
    g_signal_connect(undo_button, "clicked", G_CALLBACK(on_undo_button_clicked), undo_button);

    return page1;
}

// Funktion zum Erstellen von Seite 2
GtkWidget *create_page2(GtkStack *stack, GtkWidget *window) {
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

    global_pause_button = gtk_button_new_from_icon_name("gtk-media-pause");

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
    gtk_widget_set_visible(GTK_WIDGET(global_results_button),FALSE);

    GtkWidget *spacer2 = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_widget_set_vexpand(spacer2, TRUE);
    gtk_box_append(GTK_BOX(page2), spacer2);

    gtk_box_append(GTK_BOX(ProgressBox), GTK_WIDGET(global_ProgressLabel));
    gtk_box_append(GTK_BOX(ProgressBox), GTK_WIDGET(global_ProgressBar));
    gtk_box_append(GTK_BOX(page2), ProgressBox);
    gtk_widget_set_hexpand(GTK_WIDGET(global_ProgressBar), TRUE);

    gtk_box_append(GTK_BOX(hbox), GTK_WIDGET(global_button_previous));
    gtk_box_append(GTK_BOX(hbox), GTK_WIDGET(global_pause_button));
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

    // Lade dynamisch eine CSS-Datei
    GtkCssProvider *provider = gtk_css_provider_new();
    gtk_css_provider_load_from_string(provider, ".transparent-button { background-color: transparent; border: none; box-shadow: none; }");
    gtk_style_context_add_provider_for_display(gdk_display_get_default(), GTK_STYLE_PROVIDER(provider), GTK_STYLE_PROVIDER_PRIORITY_USER);
    g_object_unref(provider);

    GtkWidget *top_right_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 2);
    // Button für die rechte obere Ecke
    global_top_right_jump_button = GTK_BUTTON(gtk_button_new_with_label("⟼"));
    gtk_widget_add_css_class(GTK_WIDGET(global_top_right_jump_button), "transparent-button");
    gtk_box_append(GTK_BOX(top_right_box), GTK_WIDGET(global_top_right_jump_button));


    global_top_right_button = GTK_BUTTON(gtk_button_new_with_label("▶️"));
    gtk_widget_add_css_class(GTK_WIDGET(global_top_right_button), "transparent-button");
    gtk_box_append(GTK_BOX(top_right_box), GTK_WIDGET(global_top_right_button));

    g_signal_connect(GTK_WIDGET(global_top_right_button), "clicked", G_CALLBACK(run_timer), stack);
    g_signal_connect(GTK_WIDGET(global_top_right_jump_button), "clicked", G_CALLBACK(update_to_last_word), stack);

    g_signal_connect(GTK_WIDGET(global_pause_button), "clicked", G_CALLBACK(on_pause_button_presed), stack);


    gtk_widget_set_visible(GTK_WIDGET(global_top_right_button),FALSE);
    gtk_widget_set_visible(GTK_WIDGET(global_top_right_jump_button),FALSE);


    gtk_overlay_add_overlay(GTK_OVERLAY(overlay), GTK_WIDGET(top_right_box));
    gtk_widget_set_halign(GTK_WIDGET(top_right_box), GTK_ALIGN_END);
    gtk_widget_set_valign(GTK_WIDGET(top_right_box), GTK_ALIGN_START);

    return overlay;
}

void setup_shortcuts(GtkWindow *window) {
    GtkShortcutController *controller = GTK_SHORTCUT_CONTROLLER(gtk_shortcut_controller_new());
    if (!controller) {
        return;
    }
    gtk_widget_add_controller(GTK_WIDGET(window), GTK_EVENT_CONTROLLER(controller));

    GtkShortcut *shortcut = gtk_shortcut_new(
        gtk_shortcut_trigger_parse_string("F11"),
        gtk_shortcut_action_parse_string("app.toggle-fullscreen")
        );

    if (!shortcut) {
        g_object_unref(controller);
        return;
    }

    gtk_shortcut_controller_add_shortcut(controller, shortcut);
}
void on_close_request(GtkWidget *widget, gpointer data) {
    save_settings();
}

void on_activate(GtkApplication *app, gpointer user_data) {
#ifndef ICON_NAME
    GtkIconTheme *icon_theme = gtk_icon_theme_get_for_display(gdk_display_get_default());
    gtk_icon_theme_add_search_path(icon_theme, "assets/");
    gtk_icon_theme_add_resource_path(icon_theme, "assets/");

    if (gtk_icon_theme_has_icon(icon_theme, "fastreader")) {
        g_print(_("Icon mit standart Namen gefunden.\n"));
    } else {
        g_print(_("Icon nicht gefunden.\n"));
    }
#else
    g_print(_("ICON_NAME ist definiert: %s\n"), ICON_NAME);
#endif
    GSettings *settings;

#ifdef HANDLE_GSHEMATIG_DIR
    const gchar *schema_dir = "assets/glib-2.0/schemas";
    GError *error = NULL;

    GSettingsSchemaSource *source = g_settings_schema_source_new_from_directory(
        schema_dir,
        NULL,     // kein Parent-Schema-Source nötig
        TRUE,     // trusted, weil lokal kompiliert
        &error
        );

    if (!source) {
        g_printerr("Fehler beim Laden der Schema-Quelle: %s\n", error->message);
        g_error_free(error);
        return;
    }

    GSettingsSchema *schema = g_settings_schema_source_lookup(source, "io.github.quantum_mutnauq.fast_reader_gtk.State", TRUE);
    if (!schema) {
        g_printerr(_("Schema nicht gefunden!\n"));
        return;
    }
    g_free(source);
    settings = g_settings_new_full(schema, NULL, NULL);
#else
    settings = g_settings_new ("io.github.quantum_mutnauq.fast_reader_gtk.State");
#endif
    GtkWidget *window = gtk_application_window_new(app);
    if (!window) {
        g_printerr(_("Fehler beim Erstellen des Fensters.\n"));
        return;
    }

#ifdef USE_ADWAITA
    AdwStyleManager* styleManager =adw_style_manager_get_default();
    adw_style_manager_set_color_scheme(styleManager,ADW_COLOR_SCHEME_DEFAULT);
#endif
    const gchar *css =
        ".hintergrund {"
        "    background-color: @theme_bg_color;"
        "    color: @theme_fg_color;"
        "}";

    GtkCssProvider *provider = gtk_css_provider_new();
    gtk_css_provider_load_from_string(provider, css); // Der vierte Parameter ist in GTK-4 nicht mehr notwendig
    gtk_style_context_add_provider_for_display(gdk_display_get_default(),
                                               GTK_STYLE_PROVIDER(provider),
                                               GTK_STYLE_PROVIDER_PRIORITY_USER);
    gtk_widget_add_css_class(window, "hintergrund");
    g_object_unref(provider);

    gtk_window_set_title(GTK_WINDOW(window), _("Fast Reader"));
    gtk_window_set_default_size(GTK_WINDOW(window), 400, 620);
    if(settings){
        g_settings_bind (settings, "width",
                        window, "default-width",
                        G_SETTINGS_BIND_DEFAULT);
        g_settings_bind (settings, "height",
                        window, "default-height",
                        G_SETTINGS_BIND_DEFAULT);
        g_settings_bind (settings, "is-maximized",
                        window, "maximized",
                        G_SETTINGS_BIND_DEFAULT);
        g_settings_bind (settings, "is-fullscreen",
                        window, "fullscreened",
                        G_SETTINGS_BIND_DEFAULT);
        g_object_unref(settings);
    }
    GtkWidget *stack = gtk_stack_new();
    gtk_stack_set_transition_type(GTK_STACK(stack), GTK_STACK_TRANSITION_TYPE_SLIDE_LEFT_RIGHT);
    gtk_stack_add_named(GTK_STACK(stack), create_page1(GTK_STACK(stack), window), "page1");
    gtk_stack_add_named(GTK_STACK(stack), create_page2(GTK_STACK(stack), window), "page2");

    load_settings();

    if (gtk_switch_get_active(global_TimeToNextWordSwitch))
        gtk_widget_set_sensitive(GTK_WIDGET(global_TimeToNextWordSpinn), TRUE);
    else
        gtk_widget_set_sensitive(GTK_WIDGET(global_TimeToNextWordSpinn), FALSE);

    if (gtk_switch_get_active(global_SwitchLongerTimeOnLongWord)) {
        gtk_widget_set_sensitive(GTK_WIDGET(global_SpinnButtonLongerTimeOnLongWord), TRUE);
        gtk_widget_set_sensitive(GTK_WIDGET(global_SpinnButtonLongerTimeOnLongWordMultyplyer), TRUE);
    } else {
        gtk_widget_set_sensitive(GTK_WIDGET(global_SpinnButtonLongerTimeOnLongWord), FALSE);
        gtk_widget_set_sensitive(GTK_WIDGET(global_SpinnButtonLongerTimeOnLongWordMultyplyer), FALSE);
    }

    if (gtk_switch_get_active(global_SwitchLongerTimeFirstWord))
        gtk_widget_set_sensitive(GTK_WIDGET(global_SpinnButtonLongerTimeOnFirstWord), TRUE);
    else
        gtk_widget_set_sensitive(GTK_WIDGET(global_SpinnButtonLongerTimeOnFirstWord), FALSE);

    if (gtk_switch_get_active(global_labelBackgroundColorSwitch))
        gtk_widget_set_sensitive(GTK_WIDGET(global_labelBackgroundColor), TRUE);
    else
        gtk_widget_set_sensitive(GTK_WIDGET(global_labelBackgroundColor), FALSE);

    if (gtk_switch_get_active(global_labelForgroudColorSwitch))
        gtk_widget_set_sensitive(GTK_WIDGET(global_labelForgroudColor), TRUE);
    else
        gtk_widget_set_sensitive(GTK_WIDGET(global_labelForgroudColor), FALSE);


    GtkEventController *controller = gtk_event_controller_key_new();
    g_signal_connect(controller, "key-pressed", G_CALLBACK(on_key_press), stack);
    gtk_widget_add_controller(window, controller);
    setup_shortcuts(GTK_WINDOW(window));

    g_signal_connect(window, "close-request", G_CALLBACK(on_close_request), NULL);

    gtk_window_set_child(GTK_WINDOW(window), stack);
    gtk_widget_set_visible(window, TRUE);
}

int main(int argc, char *argv[]) {
    setlocale(LC_ALL, "");
    bindtextdomain("FastReader", LOCALEDIRR);
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
    g_free(fastreader_dir);

    GtkApplication *app = gtk_application_new("io.github.quantum_mutnauq.fast_reader_gtk", G_APPLICATION_DEFAULT_FLAGS);
    if (!app) {
        printf(_("Fehler beim Erstellen der Anwendung.\n"));
        return 1;
    }
    g_signal_connect(app, "activate", G_CALLBACK(on_activate), NULL);

#ifdef ICON_NAME
    gtk_window_set_default_icon_name(ICON_NAME);
#else
    gtk_window_set_default_icon_name("fastreader");
#endif

    int status = g_application_run(G_APPLICATION(app), argc, argv);

    g_object_unref(app);
    g_free(config_file_path);
    return status;
}
