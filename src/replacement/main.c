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

#include "replacement.h"
#include "latex.h"
#include "dialogs.h"
#include <gtk/gtk.h>

/* GUI */
GtkWindow* window;

GtkTreeView* costs_view;
GtkListStore* costs_model;

GtkEntry* name;
GtkSpinButton* new;
GtkSpinButton* life;
GtkSpinButton* plan;

GtkFileChooser* load_dialog;
GtkFileChooser* save_dialog;

/* Context */
replacement_context* c = NULL;

/* Functions */
void edit_started_cb(GtkCellRenderer* renderer, GtkCellEditable* editable,
                     gchar* path, gpointer user_data);
void cell_edited_cb(GtkCellRendererText* renderer, gchar* path,
                    gchar* new_text, gpointer user_data);
void change_life(int y);
void change_life_cb(GtkSpinButton* spinbutton, gpointer user_data);
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
    if(!gtk_builder_add_from_file(builder, "gui/replacement.glade", &error)) {
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
    costs_view = GTK_TREE_VIEW(gtk_builder_get_object(builder, "costs_view"));
    costs_model = GTK_LIST_STORE(gtk_builder_get_object(builder, "costs_model"));
    name = GTK_ENTRY(gtk_builder_get_object(builder, "name"));
    new = GTK_SPIN_BUTTON(gtk_builder_get_object(builder, "new"));
    life = GTK_SPIN_BUTTON(gtk_builder_get_object(builder, "life"));
    plan = GTK_SPIN_BUTTON(gtk_builder_get_object(builder, "plan"));

    load_dialog = GTK_FILE_CHOOSER(gtk_builder_get_object(builder, "load_dialog"));
    save_dialog = GTK_FILE_CHOOSER(gtk_builder_get_object(builder, "save_dialog"));

    GtkFileFilter* file_filter = gtk_file_filter_new();
    gtk_file_filter_set_name(file_filter, "Custom data file (*.replacement)");
    gtk_file_filter_add_pattern(file_filter, "*.replacement");
    gtk_file_chooser_add_filter(load_dialog, file_filter);
    gtk_file_chooser_add_filter(save_dialog, file_filter);

    /* Configure cell renderers callback */
    GtkCellRenderer* maint_renderer = GTK_CELL_RENDERER(
                            gtk_builder_get_object(builder, "maint_renderer"));
    g_signal_connect(G_OBJECT(maint_renderer),
                         "edited", G_CALLBACK(cell_edited_cb),
                         GINT_TO_POINTER(1));
    edit_started_cb(maint_renderer, NULL, NULL, NULL);

    GtkCellRenderer* sale_renderer = GTK_CELL_RENDERER(
                            gtk_builder_get_object(builder, "sale_renderer"));
    g_signal_connect(G_OBJECT(sale_renderer),
                         "edited", G_CALLBACK(cell_edited_cb),
                         GINT_TO_POINTER(2));
    edit_started_cb(sale_renderer, NULL, NULL, NULL);

    /* Connect signals */
    gtk_builder_connect_signals(builder, NULL);

    /* Initialize interface */
    change_life(2);

    g_object_unref(G_OBJECT(builder));
    gtk_widget_show(GTK_WIDGET(window));
    gtk_main();

    return(0);
}

void edit_started_cb(GtkCellRenderer* renderer, GtkCellEditable* editable,
                     gchar* path, gpointer user_data)
{
    GtkAdjustment* adj;

    g_object_get(renderer, "adjustment", &adj, NULL);
    if(adj) {
        g_object_unref(adj);
    }

    adj = gtk_adjustment_new(
                    2.00,           /* the initial value. */
                    2.00,           /* the minimum value. */
                    1000000.00,     /* the maximum value. */
                    1.0,            /* the step increment. */
                    10.0,           /* the page increment. */
                    0.0             /* the page size. */
                );
    g_object_set(renderer, "adjustment", adj, NULL);
}

void cell_edited_cb(GtkCellRendererText* renderer, gchar* path,
                    gchar* new_text, gpointer user_data)
{
    int row = atoi(path);
    int column = GPOINTER_TO_INT(user_data);
    printf("%s at (%i, %i)\n", new_text, row, column);

    /* Get reference to model */
    GtkTreePath* model_path = gtk_tree_path_new_from_string(path);
    GtkTreeIter iter;
    gtk_tree_model_get_iter(GTK_TREE_MODEL(costs_model), &iter, model_path);
    gtk_tree_path_free(model_path);

    GValue value = G_VALUE_INIT;

    if(!is_empty_string(new_text)) {
        char* end;
        float v = strtof(new_text, &end);
        if((end != new_text) && (*end == '\0') && (v >= 2.0)) {
            g_value_init(&value, G_TYPE_FLOAT);
            g_value_set_float(&value, v);
            gtk_list_store_set_value(costs_model, &iter, column, &value);

            g_value_unset(&value);

            g_value_init(&value, G_TYPE_STRING);
            g_value_set_string(&value, g_strdup_printf("%.4f", v));
            gtk_list_store_set_value(costs_model, &iter, column + 2, &value);
        }
    }
}

void change_life(int y)
{
    gtk_list_store_clear(costs_model);
    GtkTreeIter iter;
    for(int i = 0; i < y; i++) {
        gtk_list_store_append(costs_model, &iter);
        gtk_list_store_set(costs_model, &iter,
                        0, i + 1,
                        1, 1000.0,
                        2, 1000.0,
                        3, g_strdup_printf("%.4f", 1000.0),
                        4, g_strdup_printf("%.4f", 1000.0),
                        -1);
    }

    GtkTreePath* model_path = gtk_tree_model_get_path(
                                GTK_TREE_MODEL(costs_model), &iter);
    gtk_tree_view_set_cursor(costs_view, model_path, NULL, false);
    gtk_tree_path_free(model_path);
    gtk_widget_grab_focus(GTK_WIDGET(costs_view));
}

void change_life_cb(GtkSpinButton* spinbutton, gpointer user_data)
{
    int l = gtk_spin_button_get_value_as_int(life);
    change_life(l);
}

void process(GtkButton* button, gpointer user_data)
{
    if(c != NULL) {
        g_free(c->equipment);
        replacement_context_free(c);
    }

    /* Create context */
    int p = gtk_spin_button_get_value_as_int(plan);
    int l = gtk_spin_button_get_value_as_int(life);
    c = replacement_context_new(p, l);

    if(c == NULL) {
        show_error(window, "Unable to allocate enough memory for "
                           "this problem. Sorry.");
        return;
    }

    /* Fill context */
    GtkTreeIter iter;
    bool was_set = gtk_tree_model_get_iter_first(
                            GTK_TREE_MODEL(costs_model), &iter);
    if(!was_set) {
        return;
    }

    GValue value = G_VALUE_INIT;

    float* tc = c->maintenance_cost;
    float* sc = c->sale_cost;

    int i = 0;
    do {
        gtk_tree_model_get_value(
                            GTK_TREE_MODEL(costs_model), &iter, 1, &value);
        float m = g_value_get_float(&value);
        g_value_unset(&value);

        gtk_tree_model_get_value(
                            GTK_TREE_MODEL(costs_model), &iter, 2, &value);
        float s = g_value_get_float(&value);
        g_value_unset(&value);

        /* Set values */
        tc[i] = m;
        sc[i] = s;

        was_set = gtk_tree_model_iter_next(
                            GTK_TREE_MODEL(costs_model), &iter);
        i++;
    } while(was_set);

    c->equipment = g_strdup(gtk_entry_get_text(name));
    c->equipment_cost = gtk_spin_button_get_value(new);


    /* Execute algorithm */
    bool success = replacement(c);
    if(!success) {
        show_error(window, "Error while processing the information. "
                           "Please check your data.");
    }

    /* Generate report */
    bool report_created = replacement_report(c);
    if(!report_created) {
        show_error(window, "Report could not be created. "
                           "Please check your data.");
    } else {
        printf("Report created at reports/replacement.tex\n");

        int as_pdf = latex2pdf("replacement", "reports");
        if(as_pdf == 0) {
            printf("PDF version available at reports/replacement.pdf\n");
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
    if(!g_str_has_suffix(filename, ".replacement")) {
        char* new_filename = g_strdup_printf("%s.replacement", filename);
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
    fprintf(file, "%s\n", gtk_entry_get_text(name));
    fprintf(file, "%.4f\n", gtk_spin_button_get_value(new));
    fprintf(file, "%i\n", gtk_spin_button_get_value_as_int(life));
    fprintf(file, "%i\n", gtk_spin_button_get_value_as_int(plan));

    GtkTreeIter iter;
    GValue value = G_VALUE_INIT;
    bool was_set = gtk_tree_model_get_iter_first(
                            GTK_TREE_MODEL(costs_model), &iter);
    while(was_set) {
        gtk_tree_model_get_value(
                            GTK_TREE_MODEL(costs_model), &iter, 1, &value);
        float m = g_value_get_float(&value);
        g_value_unset(&value);

        gtk_tree_model_get_value(
                            GTK_TREE_MODEL(costs_model), &iter, 2, &value);
        float s = g_value_get_float(&value);
        g_value_unset(&value);

        was_set = gtk_tree_model_iter_next(
                            GTK_TREE_MODEL(costs_model), &iter);

        fprintf(file, "%.4f %.4f\n", m, s);

    }
}

void load(FILE* file)
{
    char* equip;
    int v = 0;
    float c = 0.0;

    /* Load equipment name */
    equip = get_line(file);
    gtk_entry_set_text(name, equip);

    /* Load new equipment cost */
    fscanf(file, "%f[^\n]", &c);
    gtk_spin_button_set_value(new, (gdouble)c);

    /* Load equipment service life */
    fscanf(file, "%i[^\n]", &v);
    gtk_spin_button_set_value(life, (gdouble)v);

    /* Load years plan */
    fscanf(file, "%i[^\n]", &v);
    gtk_spin_button_set_value(plan, (gdouble)v);

    /* Load replacement and sale costs */
    GtkTreeIter iter;
    bool has_row = gtk_tree_model_get_iter_first(
                            GTK_TREE_MODEL(costs_model), &iter);

    float m = 0.0;
    float s = 0.0;
    int i = 0;
    while((fscanf(file, "%f %f[^\n]", &m, &s) == 2) && has_row) {

        gtk_list_store_set(costs_model, &iter,
                        0, i + 1,
                        1, m,
                        2, s,
                        3, g_strdup_printf("%.4f", m),
                        4, g_strdup_printf("%.4f", s),
                        -1);

        /* Next row */
        has_row = gtk_tree_model_iter_next(GTK_TREE_MODEL(costs_model), &iter);
        i++;
    }
}
