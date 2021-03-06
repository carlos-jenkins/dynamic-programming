/*
 * Copyright (C) 2012 Carolina Aguilar <caroagse@gmail.com>
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

#include "knapsack.h"
#include "latex.h"
#include "dialogs.h"
#include <gtk/gtk.h>

/* GUI */
GtkWindow* window;
GtkTreeView* items_view;
GtkListStore* items_model;
GtkSpinButton* capacity;
GtkEntry* unit;

GtkFileChooser* load_dialog;
GtkFileChooser* save_dialog;

/* Context */
knapsack_context* c = NULL;

/* Functions */
void add_row(GtkToolButton *toolbutton, gpointer user_data);
void remove_row(GtkToolButton *toolbutton, gpointer user_data);
void edit_started_cb(GtkCellRenderer* renderer, GtkCellEditable* editable,
                     gchar* path, gpointer user_data);
void cell_edited_cb(GtkCellRendererText* renderer, gchar* path,
                    gchar* new_text, gpointer user_data);
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
    if(!gtk_builder_add_from_file(builder, "gui/knapsack.glade", &error)) {
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
    items_view = GTK_TREE_VIEW(gtk_builder_get_object(builder, "items_view"));
    items_model = GTK_LIST_STORE(gtk_builder_get_object(builder, "items_model"));
    capacity = GTK_SPIN_BUTTON(gtk_builder_get_object(builder, "capacity"));
    unit = GTK_ENTRY(gtk_builder_get_object(builder, "unit"));

    load_dialog = GTK_FILE_CHOOSER(gtk_builder_get_object(builder, "load_dialog"));
    save_dialog = GTK_FILE_CHOOSER(gtk_builder_get_object(builder, "save_dialog"));

    GtkFileFilter* file_filter = gtk_file_filter_new();
    gtk_file_filter_set_name(file_filter, "Custom data file (*.knapsack)");
    gtk_file_filter_add_pattern(file_filter, "*.knapsack");
    gtk_file_chooser_add_filter(load_dialog, file_filter);
    gtk_file_chooser_add_filter(save_dialog, file_filter);

    /* Configure cell renderers callback */
    GtkCellRenderer* name_renderer = GTK_CELL_RENDERER(
                            gtk_builder_get_object(builder, "name_renderer"));
    g_signal_connect(G_OBJECT(name_renderer),
                         "edited", G_CALLBACK(cell_edited_cb),
                         GINT_TO_POINTER(0));

    GtkCellRenderer* value_renderer = GTK_CELL_RENDERER(
                            gtk_builder_get_object(builder, "value_renderer"));
    g_signal_connect(G_OBJECT(value_renderer),
                         "edited", G_CALLBACK(cell_edited_cb),
                         GINT_TO_POINTER(1));
    edit_started_cb(value_renderer, NULL, NULL, NULL);

    GtkCellRenderer* weight_renderer = GTK_CELL_RENDERER(
                            gtk_builder_get_object(builder, "weight_renderer"));
    g_signal_connect(G_OBJECT(weight_renderer),
                         "edited", G_CALLBACK(cell_edited_cb),
                         GINT_TO_POINTER(2));
    edit_started_cb(weight_renderer, NULL, NULL, NULL);

    GtkCellRenderer* amount_renderer = GTK_CELL_RENDERER(
                            gtk_builder_get_object(builder, "amount_renderer"));
    g_signal_connect(G_OBJECT(amount_renderer),
                         "edited", G_CALLBACK(cell_edited_cb),
                         GINT_TO_POINTER(3));
    edit_started_cb(amount_renderer, NULL, NULL, NULL);

    /* Connect signals */
    gtk_builder_connect_signals(builder, NULL);

    /* Initialize interface */
    add_row(NULL, NULL);
    add_row(NULL, NULL);

    g_object_unref(G_OBJECT(builder));
    gtk_widget_show(GTK_WIDGET(window));
    gtk_main();

    return(0);
}

void add_row(GtkToolButton *toolbutton, gpointer user_data)
{
    int rows = gtk_tree_model_iter_n_children(
                                    GTK_TREE_MODEL(items_model), NULL);

    GtkTreeIter iter;
    gtk_list_store_append(items_model, &iter);
    gtk_list_store_set(items_model, &iter,
                        0, sequence_name(rows),
                        1, 1,
                        2, 1,
                        3, 1,
                        4, "1",
                        -1);

    GtkTreePath* model_path = gtk_tree_model_get_path(
                                GTK_TREE_MODEL(items_model), &iter);
    gtk_tree_view_set_cursor(items_view, model_path,
                             gtk_tree_view_get_column(items_view, 0),
                             true);
    gtk_tree_path_free(model_path);

    return;
}

void remove_row(GtkToolButton *toolbutton, gpointer user_data)
{
    int rows = gtk_tree_model_iter_n_children(
                                    GTK_TREE_MODEL(items_model), NULL);
    if(rows < 3) {
        return;
    }

    GtkTreeSelection* selection = gtk_tree_view_get_selection(items_view);
    GtkTreeIter iter;
    if(gtk_tree_selection_get_selected(selection, NULL, &iter)) {

        bool valid = gtk_list_store_remove(items_model, &iter);
        if(!valid) {
            valid = gtk_tree_model_iter_nth_child(GTK_TREE_MODEL(items_model),
                                                  &iter, NULL, rows - 1);
        }

        if(valid) {
            GtkTreePath* model_path = gtk_tree_model_get_path(
                                        GTK_TREE_MODEL(items_model), &iter);
            gtk_tree_view_set_cursor(items_view, model_path, NULL, false);
            gtk_tree_path_free(model_path);
        }
    }
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
                    1.00,           /* the initial value. */
                    1.00,           /* the minimum value. */
                    10000000.00,    /* the maximum value. */
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
    gtk_tree_model_get_iter(GTK_TREE_MODEL(items_model), &iter, model_path);
    gtk_tree_path_free(model_path);

    GValue value = G_VALUE_INIT;

    /* Text */
    if(column == 0) {
        g_value_init(&value, G_TYPE_STRING);
        g_value_set_string(&value, new_text);
        gtk_list_store_set_value(items_model, &iter, column, &value);
        return;
    }

    /* Infinity */
    if(column == 3) {
        int is_inf = strncmp(new_text, "oo", 2);
        if(is_inf == 0 || is_empty_string(new_text)) {
            g_value_init(&value, G_TYPE_INT);
            g_value_set_int(&value, (int) PLUS_INF);
            gtk_list_store_set_value(items_model, &iter, column, &value);
            g_value_unset(&value);

            g_value_init(&value, G_TYPE_STRING);
            g_value_set_string(&value, "oo");
            gtk_list_store_set_value(items_model, &iter, column + 1, &value);
            g_value_unset(&value);

            return;
        }
    }

    /* Number */
    char* end;
    int v = (int) strtol(new_text, &end, 10);
    if((end != new_text) && (*end == '\0') && (v > 0)) {
        g_value_init(&value, G_TYPE_INT);
        g_value_set_int(&value, v);
        gtk_list_store_set_value(items_model, &iter, column, &value);

        if(column == 3) {
            g_value_unset(&value);

            g_value_init(&value, G_TYPE_STRING);
            g_value_set_string(&value, g_strdup_printf("%i", v));
            gtk_list_store_set_value(items_model, &iter, column + 1, &value);
        }
    }
}

void process(GtkButton* button, gpointer user_data)
{
    if(c != NULL) {
        g_free(c->unit);
        knapsack_context_free(c);
    }

    /* Create context */
    int cap = gtk_spin_button_get_value_as_int(capacity);
    int num_it = gtk_tree_model_iter_n_children(
                                    GTK_TREE_MODEL(items_model), NULL);
    c = knapsack_context_new(cap, num_it);
    if(c == NULL) {
        show_error(window, "Unable to allocate enough memory for "
                           "this problem. Sorry.");
        return;
    }

    /* Fill context */
    item** its = c->items;
    GtkTreeIter iter;
    bool was_set = gtk_tree_model_get_iter_first(
                            GTK_TREE_MODEL(items_model), &iter);
    if(!was_set) {
        return;
    }

    GValue value = G_VALUE_INIT;

    int i = 0;
    do {
        gtk_tree_model_get_value(
                            GTK_TREE_MODEL(items_model), &iter, 0, &value);
        char* n = g_value_dup_string(&value);
        g_value_unset(&value);

        gtk_tree_model_get_value(
                            GTK_TREE_MODEL(items_model), &iter, 1, &value);
        int v = g_value_get_int(&value);
        g_value_unset(&value);

        gtk_tree_model_get_value(
                            GTK_TREE_MODEL(items_model), &iter, 2, &value);
        int w = g_value_get_int(&value);
        g_value_unset(&value);

        gtk_tree_model_get_value(
                            GTK_TREE_MODEL(items_model), &iter, 3, &value);
        int a = g_value_get_int(&value);
        g_value_unset(&value);

        /* Set values */
        item_new(its[i], n,  v, w, a); /* name, value, weight, amount */

        was_set = gtk_tree_model_iter_next(
                            GTK_TREE_MODEL(items_model), &iter);
        i++;
    } while(was_set);

    c->unit = g_strdup(gtk_entry_get_text(unit));

    /* Execute algorithm */
    bool success = knapsack(c);
    if(!success) {
        show_error(window, "Error while processing the information. "
                           "Please check your data.");
    }

    /* Show tables */
    printf("-----------------------------------\n");
    matrix_print(c->table_values);
    printf("-----------------------------------\n");
    matrix_print(c->table_items);

    /* Generate report */
    bool report_created = knapsack_report(c);
    if(!report_created) {
        show_error(window, "Report could not be created. "
                           "Please check your data.");
    } else {
        printf("Report created at reports/knapsack.tex\n");

        int as_pdf = latex2pdf("knapsack", "reports");
        if(as_pdf == 0) {
            printf("PDF version available at reports/knapsack.pdf\n");
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
    if(!g_str_has_suffix(filename, ".knapsack")) {
        char* new_filename = g_strdup_printf("%s.knapsack", filename);
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
    /* Number of items */
    fprintf(file, "%i\n", gtk_tree_model_iter_n_children(
                                    GTK_TREE_MODEL(items_model), NULL));

    /* Items names */
    GtkTreeIter iter;
    GValue value = G_VALUE_INIT;
    bool was_set = gtk_tree_model_get_iter_first(
                                    GTK_TREE_MODEL(items_model), &iter);
    while(was_set) {

        gtk_tree_model_get_value(
                            GTK_TREE_MODEL(items_model), &iter, 0, &value);
        char* n = g_value_dup_string(&value);
        g_value_unset(&value);

        fprintf(file, "%s\n", n);

        g_free(n);

        /* Next */
        was_set = gtk_tree_model_iter_next(GTK_TREE_MODEL(items_model), &iter);
    }

    /* Items data */
    was_set = gtk_tree_model_get_iter_first(
                                    GTK_TREE_MODEL(items_model), &iter);
    while(was_set) {

        gtk_tree_model_get_value(
                            GTK_TREE_MODEL(items_model), &iter, 1, &value);
        int v = g_value_get_int(&value);
        g_value_unset(&value);

        gtk_tree_model_get_value(
                            GTK_TREE_MODEL(items_model), &iter, 2, &value);
        int w = g_value_get_int(&value);
        g_value_unset(&value);

        gtk_tree_model_get_value(
                            GTK_TREE_MODEL(items_model), &iter, 3, &value);
        int a = g_value_get_int(&value);
        g_value_unset(&value);

        gtk_tree_model_get_value(
                            GTK_TREE_MODEL(items_model), &iter, 4, &value);
        char* as = g_value_dup_string(&value);
        g_value_unset(&value);

        if(strncmp(as, "oo", 2) == 0) {
            fprintf(file, "%i %i oo\n", v, w);
        } else {
            fprintf(file, "%i %i %i\n", v, w, a);
        }
        g_free(as);

        /* Next */
        was_set = gtk_tree_model_iter_next(GTK_TREE_MODEL(items_model), &iter);
    }

    fprintf(file, "%i\n", gtk_spin_button_get_value_as_int(capacity));
    fprintf(file, "%s\n", gtk_entry_get_text(unit));
}

void load(FILE* file)
{
    /* Load number of items */
    int num_items = 0;
    fscanf(file, "%i%*c", &num_items);

    /* Adapt GUI */
    gtk_list_store_clear(items_model);
    for(int i = 0; i < num_items; i++) {
        add_row(NULL, NULL);
    }

    /* Load items names */
    char** names = (char**) malloc(num_items * sizeof(char*));
    for(int i = 0; i < num_items; i++) {
        names[i] = get_line(file);
    }

    /* Load items data */
    GtkTreeIter iter;
    bool has_row = gtk_tree_model_get_iter_first(
                            GTK_TREE_MODEL(items_model), &iter);
    int v = 0;
    int w = 0;
    char buf[12];
    char* a = (char*) &buf;
    for(int i = 0; (i < num_items) && has_row; i++) {

        /* Get values */
        fscanf(file, "%i %i %s%*c", &v, &w, a);

        /* Set values */
        if(strncmp(a, "oo", 2) == 0) {
            gtk_list_store_set(items_model, &iter,
                        0, names[i],
                        1, v,
                        2, w,
                        3, (int) PLUS_INF,
                        4, "oo",
                        -1);
        } else {
            gtk_list_store_set(items_model, &iter,
                        0, names[i],
                        1, v,
                        2, w,
                        3, atoi(a),
                        4, a,
                        -1);
        }

        /* Next */
        has_row = gtk_tree_model_iter_next(GTK_TREE_MODEL(items_model), &iter);
    }

    /* Set capacity */
    int c = 0;
    fscanf(file, "%i%*c", &c);
    gtk_spin_button_set_value(capacity, (gdouble)c);

    /* Set unit */
    char* u = get_line(file);
    gtk_entry_set_text(unit, u);
    free(u);

    /* Free resources */
    for(int i = 0; i < num_items; i++) {
        free(names[i]);
    }
    free(names);
}
