/*
 * Copyright (C) 2012 Carolina Aguilar <caroagse@gmail.co.cr>
 * Copyright (C) 2012 Carlos Jenkins <carlos@jenkins.co.cr>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "probwin.h"
#include "latex.h"
#include "dialogs.h"
#include <gtk/gtk.h>

/* GUI */
GtkWindow* window;

GtkTreeView* format_view;
GtkListStore* format_model;

GtkEntry* team_a_name;
GtkEntry* team_b_name;

GtkSpinButton* prob_a_home;
GtkSpinButton* prob_a_road;
GtkSpinButton* num_games;

GtkLabel* prob_b_home;
GtkLabel* prob_b_road;

GtkFileChooser* load_dialog;
GtkFileChooser* save_dialog;

/* Context */
probwin_context* c = NULL;

/* Context */
void prob_changed_cb(GtkSpinButton* spinbutton, gpointer user_data);
void games_changed(int g);
void games_changed_cb(GtkSpinButton* spinbutton, gpointer user_data);
void cell_edited_cb(GtkCellRendererToggle *cell_renderer,
                    gchar* path, gpointer user_data);
void process(GtkButton* button, gpointer user_data);

void save_cb(GtkButton* button, gpointer user_data);
void load_cb(GtkButton* button, gpointer user_data);
void save(FILE* file);
void load(FILE* file);

int main(int argc, char **argv)
{
    GtkBuilder* builder;
    GError* error = NULL;

    /* Starts Gtk+ subsystem */
    gtk_init(&argc, &argv);

    /* Load GUI interface */
    builder = gtk_builder_new();
    if(!gtk_builder_add_from_file(builder, "gui/probwin.glade", &error)) {
        if(error) {
            g_warning("%s", error->message);
            g_error_free(error);
        } else {
            g_warning("Unknown error.");
        }
        return(1);
    }

    /* Get pointers to objects */
    window = GTK_WINDOW(gtk_builder_get_object(builder, "window"));

    format_view = GTK_TREE_VIEW(gtk_builder_get_object(builder, "format_view"));
    format_model = GTK_LIST_STORE(gtk_builder_get_object(builder, "format_model"));

    team_a_name = GTK_ENTRY(gtk_builder_get_object(builder, "team_a_name"));
    team_b_name = GTK_ENTRY(gtk_builder_get_object(builder, "team_b_name"));

    prob_a_home = GTK_SPIN_BUTTON(gtk_builder_get_object(builder, "prob_a_home"));
    prob_a_road = GTK_SPIN_BUTTON(gtk_builder_get_object(builder, "prob_a_road"));
    num_games = GTK_SPIN_BUTTON(gtk_builder_get_object(builder, "num_games"));

    prob_b_home = GTK_LABEL(gtk_builder_get_object(builder, "prob_b_home"));
    prob_b_road = GTK_LABEL(gtk_builder_get_object(builder, "prob_b_road"));

    load_dialog = GTK_FILE_CHOOSER(gtk_builder_get_object(builder, "load_dialog"));
    save_dialog = GTK_FILE_CHOOSER(gtk_builder_get_object(builder, "save_dialog"));

    GtkFileFilter* file_filter = gtk_file_filter_new();
    gtk_file_filter_set_name(file_filter, "Custom data file (*.probwin)");
    gtk_file_filter_add_pattern(file_filter, "*.probwin");
    gtk_file_chooser_add_filter(load_dialog, file_filter);
    gtk_file_chooser_add_filter(save_dialog, file_filter);

    /* Connect signals */
    gtk_builder_connect_signals(builder, NULL);

    g_object_unref(G_OBJECT(builder));
    gtk_widget_show(GTK_WIDGET(window));
    gtk_main();

    return(0);
}

void prob_changed_cb(GtkSpinButton* spinbutton, gpointer user_data)
{
    float value = gtk_spin_button_get_value(spinbutton);
    float complement = 1.0 - value;
    if(spinbutton == prob_a_home) {
        gtk_label_set_text(prob_b_road, g_strdup_printf("%.4f", complement));
    } else {
        gtk_label_set_text(prob_b_home, g_strdup_printf("%.4f", complement));
    }
}

void games_changed(int g)
{
    /* Validate input */
    if(g < 3) {
        return;
    }

    gtk_list_store_clear(format_model);
    GtkTreeIter iter;
    bool swap = true;
    for(int i = 0; i < g; i++) {
        gtk_list_store_append(format_model, &iter);
        gtk_list_store_set(format_model, &iter,
                        0, i + 1,
                        1, swap,
                        -1);
        swap = !swap;
    }

    GtkTreePath* model_path = gtk_tree_model_get_path(
                                GTK_TREE_MODEL(format_model), &iter);
    gtk_tree_view_set_cursor(format_view, model_path, NULL, false);
    gtk_tree_path_free(model_path);
    gtk_widget_grab_focus(GTK_WIDGET(format_view));
}

void games_changed_cb(GtkSpinButton* spinbutton, gpointer user_data)
{
    int value = gtk_spin_button_get_value_as_int(num_games);
    if((value % 2) == 0) {
        show_error(window, "Number of games must be odd.");
        gtk_spin_button_set_value(num_games, 3.0);
        games_changed(3);
        return;
    }
    games_changed(value);
}

void cell_edited_cb(GtkCellRendererToggle *cell_renderer,
                    gchar* path, gpointer user_data)
{
    /* Get reference to model */
    GtkTreePath* model_path = gtk_tree_path_new_from_string(path);
    GtkTreeIter iter;
    gtk_tree_model_get_iter(GTK_TREE_MODEL(format_model), &iter, model_path);
    gtk_tree_path_free(model_path);

    GValue value = G_VALUE_INIT;

    g_value_init(&value, G_TYPE_BOOLEAN);
    g_value_set_boolean(&value,
                        !gtk_cell_renderer_toggle_get_active(cell_renderer));
    gtk_list_store_set_value(format_model, &iter, 1, &value);
}

void process(GtkButton* button, gpointer user_data)
{
    if(c != NULL) {
        g_free(c->a_name);
        g_free(c->b_name);
        probwin_context_free(c);
    }

    /* Create context */
    int g = gtk_spin_button_get_value_as_int(num_games);
    c = probwin_context_new(g);
    if(c == NULL) {
        show_error(window, "Unable to allocate enough memory for "
                           "this problem. Sorry.");
        return;
    }

    /* Fill context */
    /* Names */
    c->a_name = g_strdup(gtk_entry_get_text(team_a_name));
    c->b_name = g_strdup(gtk_entry_get_text(team_b_name));

    /* Probs */
    c->ph = gtk_spin_button_get_value(prob_a_home);
    c->pr = gtk_spin_button_get_value(prob_a_road);

    /* Format */
    GtkTreeIter iter;
    bool was_set = gtk_tree_model_get_iter_first(
                            GTK_TREE_MODEL(format_model), &iter);
    if(!was_set) {
        return;
    }

    GValue value = G_VALUE_INIT;
    bool* f = c->game_format;

    int i = 0;
    do {
        gtk_tree_model_get_value(
                            GTK_TREE_MODEL(format_model), &iter, 1, &value);
        bool w = g_value_get_boolean(&value);
        g_value_unset(&value);

        /* Set values */
        f[i] = w;

        was_set = gtk_tree_model_iter_next(
                            GTK_TREE_MODEL(format_model), &iter);
        i++;
    } while(was_set);

    /* Execute algorithm */
    bool success = probwin(c);
    if(!success) {
        show_error(window, "Error while processing the information. "
                           "Please check your data.");
    }

    /* Generate report */
    bool report_created = probwin_report(c);
    if(!report_created) {
        show_error(window, "Report could not be created. "
                           "Please check your data.");
    } else {
        printf("Report created at reports/probwin.tex\n");

        int as_pdf = latex2pdf("probwin", "reports");
        if(as_pdf == 0) {
            printf("PDF version available at reports/probwin.pdf\n");
        } else {
            char* error = g_strdup_printf("Unable to convert report to PDF. "
                                          "Status: %i.", as_pdf);
            show_error(window, error);
            g_free(error);
        }
    }
}

void save_cb(GtkButton* button, gpointer user_data)
{
    /* Load save dialog */
    int response = gtk_dialog_run(GTK_DIALOG(save_dialog));
    if(response != 0) {
        gtk_widget_hide(GTK_WIDGET(save_dialog));
        return;
    }

    /* Get filename */
    char* filename;
    filename = gtk_file_chooser_get_filename(save_dialog);

    /* Check is not empty */
    printf("Selected file: %s\n", filename);
    if((filename == NULL) || is_empty_string(filename)) {
        show_error(window, "Please select a file.");
        g_free(filename);
        save_cb(button, user_data);
        return;
    }
    gtk_widget_hide(GTK_WIDGET(save_dialog));

    /* Check extension */
    if(!g_str_has_suffix(filename, ".probwin")) {
        char* new_filename = g_strdup_printf("%s.probwin", filename);
        g_free(filename);
        filename = new_filename;
    }

    /* Try to open file for writing */
    FILE* file = fopen(filename, "w");
    if(file == NULL) {
        show_error(window, "An error ocurred while trying to open "
                           "the file. Check you have permissions.");
        g_free(filename);
        return;
    }

    /* Save current context */
    printf("Saving to file %s\n", filename);
    save(file);

    /* Free resources */
    fclose(file);
    g_free(filename);
}

void load_cb(GtkButton* button, gpointer user_data)
{
    /* Load load dialog */
    int response = gtk_dialog_run(GTK_DIALOG(load_dialog));
    if(response != 0) {
        gtk_widget_hide(GTK_WIDGET(load_dialog));
        return;
    }

    /* Get filename */
    char* filename;
    filename = gtk_file_chooser_get_filename(load_dialog);

    /* Check is not empty */
    printf("Selected file: %s\n", filename);
    if((filename == NULL) || is_empty_string(filename)) {
        show_error(window, "Please select a file.");
        g_free(filename);
        load_cb(button, user_data);
        return;
    }
    gtk_widget_hide(GTK_WIDGET(load_dialog));

    /* Try to open file for reading */
    if(!file_exists(filename)) {
        show_error(window, "The selected file doesn't exists.");
        return;
    }
    FILE* file = fopen(filename, "r");
    if(file == NULL) {
        show_error(window, "An error ocurred while trying to open "
                           "the file. Check you have permissions.");
        return;
    }

    /* Load file */
    printf("Loading file %s\n", filename);
    load(file);

    /* Free resources */
    fclose(file);
    g_free(filename);
}

void save(FILE* file)
{
    printf("save()\n");

    fprintf(file, "%s\n", g_strdup(gtk_entry_get_text(team_a_name)));
    fprintf(file, "%s\n", g_strdup(gtk_entry_get_text(team_b_name)));

    fprintf(file, "%1.2f\n", gtk_spin_button_get_value(prob_a_home));
    fprintf(file, "%1.2f\n", gtk_spin_button_get_value(prob_a_road));


    fprintf( file, "%i\n", gtk_spin_button_get_value_as_int(num_games));


    GtkTreeIter iter;
    bool was_set = gtk_tree_model_get_iter_first(
                            GTK_TREE_MODEL(format_model), &iter);
    if(!was_set) {
        return;
    }

    GValue value = G_VALUE_INIT;

    do {
        gtk_tree_model_get_value(
                            GTK_TREE_MODEL(format_model), &iter, 1, &value);
        bool w = g_value_get_boolean(&value);
        g_value_unset(&value);

        was_set = gtk_tree_model_iter_next(
                            GTK_TREE_MODEL(format_model), &iter);

        fprintf( file, "%i\n", w);

    } while(was_set);

}

void load(FILE* file)
{
    printf("load()\n");
    /**
     * FIXME: IMPLEMENT
     **/
}
