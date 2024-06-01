#include <gtk/gtk.h>
#include <string.h>

// Globale Variable zur Speicherung des Textfelds
static GtkTextView *global_text_view = NULL;
static GtkLabel *global_label = NULL;
static GtkColorButton *global_labelBackgroundColor = NULL;
static GtkColorButton *global_labelForgroudColor = NULL;
static GtkFontButton *global_labelTextButton = NULL;
static GtkButton *global_button_previous = NULL;
static GtkButton *global_button_next = NULL;
static GtkButton *global_button_read = NULL;

// Globale Variablen zur Verwaltung des Texts und der Wörter
static gchar **words = NULL;
static int current_word_index = 0;
static int total_words = 0;

// Funktion zum Überprüfen und Aktualisieren der Button-Zustände
static void update_button_states() {
    if (current_word_index <= 1) {
        gtk_widget_set_sensitive(GTK_WIDGET(global_button_previous), FALSE);
    } else {
        gtk_widget_set_sensitive(GTK_WIDGET(global_button_previous), TRUE);
    }

    if (current_word_index >= total_words) {
        gtk_widget_set_sensitive(GTK_WIDGET(global_button_next), FALSE);
    } else {
        gtk_widget_set_sensitive(GTK_WIDGET(global_button_next), TRUE);
    }
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

// Funktion zum Aufteilen eines Textes in Wörter
static void split_text_into_words(const gchar *text) {
    g_strfreev(words);  // Alte Wörter freigeben, falls vorhanden
    // Text in Wörter aufteilen, wobei Leerzeichen und neue Zeilen als Trennzeichen verwendet werden
    words = g_regex_split_simple("[ \n]+", text, 0, 0);
    current_word_index = 0;
    total_words = g_strv_length(words);
    update_button_states();
}

// Funktion zum Aktualisieren des Labels mit dem nächsten Wort
static void update_label_with_next_word() {
    if (words != NULL && current_word_index < total_words) {
        if (g_strcmp0(words[current_word_index], "") != 0) {
            gtk_label_set_text(global_label, words[current_word_index]);
        }
        current_word_index++;
        update_button_states();
    }
}

// Funktion zum Aktualisieren des Labels mit dem vorherigen Wort
static void update_label_with_previous_word() {
    if (words != NULL && current_word_index > 1) {
        current_word_index -= 2;
        if (g_strcmp0(words[current_word_index], "") != 0) {
            gtk_label_set_text(global_label, words[current_word_index]);
        }
        current_word_index++;
        update_button_states();
    }
}

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
    GtkStack *stack = GTK_STACK(data);
    gtk_stack_set_visible_child_name(stack, "page1");
}

// Funktion zum Anwenden der Label-Stile
static void apply_label_styles() {
    GdkRGBA bg_color, fg_color;
    gtk_color_chooser_get_rgba(GTK_COLOR_CHOOSER(global_labelBackgroundColor), &bg_color);
    gtk_color_chooser_get_rgba(GTK_COLOR_CHOOSER(global_labelForgroudColor), &fg_color);

    gchar *font_desc_str = gtk_font_chooser_get_font(GTK_FONT_CHOOSER(global_labelTextButton));

    // Extrahiere Schriftartname aus font_desc_str
    PangoFontDescription *font_desc = pango_font_description_from_string(font_desc_str);
    const gchar *font_family = pango_font_description_get_family(font_desc);
    
    // Setze die Schriftgröße explizit auf 50
    gint font_size = 50;

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

    GtkTextIter start, end;
    gtk_text_buffer_get_start_iter(buffer, &start);
    gtk_text_buffer_get_end_iter(buffer, &end);

    char *text = gtk_text_buffer_get_text(buffer, &start, &end, FALSE);

    split_text_into_words(text);
    apply_label_styles();
    update_label_with_next_word();

    g_free(text);

    gtk_stack_set_visible_child_name(stack, "page2");
}

// Callback-Funktion, die beim Schließen des Fensters aufgerufen wird
static void on_window_destroy(GtkWidget *widget, gpointer data) {
    GMainLoop *loop = (GMainLoop *)data;
    g_main_loop_quit(loop);
}

// Tastensignal-Callback-Funktion
static gboolean on_key_press(GtkEventControllerKey *controller, guint keyval, guint keycode, GdkModifierType state, gpointer data) {
    GtkStack *stack = GTK_STACK(data);
    const gchar *visible_child_name = gtk_stack_get_visible_child_name(stack);
    if (g_strcmp0(visible_child_name, "page2") != 0)
        return FALSE;

    if (keyval != GDK_KEY_Left && keyval != GDK_KEY_Escape && keyval != GDK_KEY_Down) {
        update_label_with_next_word();
        return TRUE;
    } else if (keyval == GDK_KEY_Left || keyval == GDK_KEY_Down) {
        update_label_with_previous_word();
        return TRUE;
    } else if (keyval == GDK_KEY_Escape) {
        on_switch_to_page1(NULL, stack);
        return TRUE;
    }
    return FALSE;
}

// Funktion zum Erstellen von Seite 1
static GtkWidget *create_page1(GtkStack *stack, GtkWidget *window) {

    GtkWidget *page1 = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);

    GtkWidget *label = gtk_label_new("Einstellungen:");
    gtk_widget_set_halign(label, GTK_ALIGN_CENTER);
    
    GtkWidget *Backround = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 2);
    GtkWidget *labelBackground = gtk_label_new("Hintergrund:");
    GtkWidget *labelBackgroundColor = gtk_color_button_new();

    GtkWidget *Forgroud = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 2);
    GtkWidget *labelForgroud = gtk_label_new("Schrift:");
    GtkWidget *labelForgroudColor = gtk_color_button_new();

    GtkWidget *Text = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 2);
    GtkWidget *labelTextSize = gtk_label_new("Schrift Größe:");
    GtkWidget *labelTextButton = gtk_font_button_new();

    // Erstelle eine Schriftartbeschreibung mit Standardgröße 50
    PangoFontDescription *default_font_desc = pango_font_description_from_string(get_default_font_name());
    pango_font_description_set_size(default_font_desc, 50 * PANGO_SCALE);  // Setze Größe auf 50pt
    gchar *default_font_desc_str = pango_font_description_to_string(default_font_desc);
    gtk_font_chooser_set_font(GTK_FONT_CHOOSER(labelTextButton), default_font_desc_str);

    pango_font_description_free(default_font_desc);
    g_free(default_font_desc_str);

    GtkWidget *button1 = gtk_button_new_with_label("Lesen");
    GtkWidget *text_view = gtk_text_view_new();
    GtkWidget *scrolled_window = gtk_scrolled_window_new();
    gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(scrolled_window), text_view);
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolled_window), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);

    // Standard-Hintergrund- und Schriftfarbe abrufen und setzen
    GdkRGBA bg_color, fg_color;
    get_default_colors(&bg_color, &fg_color, window);
    gtk_color_chooser_set_rgba(GTK_COLOR_CHOOSER(labelBackgroundColor), &bg_color);
    gtk_color_chooser_set_rgba(GTK_COLOR_CHOOSER(labelForgroudColor), &fg_color);

    gtk_box_append(GTK_BOX(page1), label);
    gtk_box_append(GTK_BOX(Backround), labelBackground);
    gtk_box_append(GTK_BOX(Backround), labelBackgroundColor);
    gtk_box_append(GTK_BOX(page1), Backround);

    gtk_box_append(GTK_BOX(Forgroud), labelForgroud);
    gtk_box_append(GTK_BOX(Forgroud), labelForgroudColor);
    gtk_box_append(GTK_BOX(page1), Forgroud);

    gtk_box_append(GTK_BOX(Text), labelTextSize);
    gtk_box_append(GTK_BOX(Text), labelTextButton);
    gtk_box_append(GTK_BOX(page1), Text);

    gtk_box_append(GTK_BOX(page1), scrolled_window);
    gtk_widget_set_vexpand(scrolled_window, TRUE);
    gtk_box_append(GTK_BOX(page1), button1);
    gtk_widget_set_hexpand(button1, FALSE);
    gtk_widget_set_vexpand(button1, FALSE);

    global_text_view = GTK_TEXT_VIEW(text_view);
    global_labelBackgroundColor= GTK_COLOR_BUTTON(labelBackgroundColor);
    global_labelForgroudColor= GTK_COLOR_BUTTON(labelForgroudColor);
    global_labelTextButton= GTK_FONT_BUTTON(labelTextButton);
    global_button_read=GTK_BUTTON(button1);
    
    update_read_button_state(gtk_text_view_get_buffer(GTK_TEXT_VIEW(text_view)), NULL);
        
    g_signal_connect(button1, "clicked", G_CALLBACK(on_switch_to_page2), stack);
    g_signal_connect(gtk_text_view_get_buffer(GTK_TEXT_VIEW(text_view)), "changed", G_CALLBACK(update_read_button_state), NULL);
    return page1;
}

// Funktion zum Erstellen von Seite 2
static GtkWidget *create_page2(GtkStack *stack,GtkWidget *window ) {
    GtkWidget *page2 = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    GtkWidget *label = gtk_label_new("");
    GtkWidget *hbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 2);
    GtkWidget *button_previous = gtk_button_new_with_label("←");
    GtkWidget *button_next = gtk_button_new_with_label("→");
    GtkWidget *button2 = gtk_button_new_with_label("Zurück");

    GtkWidget *spacer1 = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_widget_set_vexpand(spacer1, TRUE);
    gtk_box_append(GTK_BOX(page2), spacer1);
    
    gtk_widget_set_halign(label, GTK_ALIGN_CENTER);
    gtk_box_append(GTK_BOX(page2), label);

    GtkWidget *spacer2 = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_widget_set_vexpand(spacer2, TRUE);
    gtk_box_append(GTK_BOX(page2), spacer2);

    gtk_box_append(GTK_BOX(hbox), button_previous);
    gtk_box_append(GTK_BOX(hbox), button_next);
    gtk_box_append(GTK_BOX(page2), hbox);

    gtk_widget_set_hexpand(button_previous, TRUE);
    gtk_widget_set_hexpand(button_next, TRUE);
    gtk_widget_set_halign(hbox, GTK_ALIGN_FILL);
    gtk_widget_set_hexpand(hbox, TRUE);

    gtk_box_append(GTK_BOX(page2), button2);

    g_signal_connect(button_previous, "clicked", G_CALLBACK(update_label_with_previous_word), NULL);
    g_signal_connect(button_next, "clicked", G_CALLBACK(update_label_with_next_word), NULL);
    g_signal_connect(button2, "clicked", G_CALLBACK(on_switch_to_page1), stack);

    global_label = GTK_LABEL(label);
    global_button_previous =GTK_BUTTON(button_previous);
    global_button_next=GTK_BUTTON(button_next);
    return page2;
}

int main(int argc, char *argv[]) {
    gtk_init();

    GtkWidget *window = gtk_window_new();
    gtk_window_set_title(GTK_WINDOW(window), "Speed Reader");
    gtk_window_set_default_size(GTK_WINDOW(window), 400, 500);

    GMainLoop *loop = g_main_loop_new(NULL, FALSE);
    g_signal_connect(window, "destroy", G_CALLBACK(on_window_destroy), loop);

    GtkWidget *stack = gtk_stack_new();
    gtk_stack_set_transition_type(GTK_STACK(stack), GTK_STACK_TRANSITION_TYPE_SLIDE_LEFT_RIGHT);
    gtk_stack_add_named(GTK_STACK(stack), create_page1(GTK_STACK(stack),window), "page1");
    gtk_stack_add_named(GTK_STACK(stack), create_page2(GTK_STACK(stack),window), "page2");

    gtk_window_set_child(GTK_WINDOW(window), stack);

    GtkEventController *controller = gtk_event_controller_key_new();
    g_signal_connect(controller, "key-pressed", G_CALLBACK(on_key_press), stack);
    gtk_widget_add_controller(window, controller);

    gtk_widget_show(window);
    g_main_loop_run(loop);

    g_main_loop_unref(loop);
    return 0;
}

