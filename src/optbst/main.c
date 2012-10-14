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

#include "optbst.h"
#include "latex.h"
#include "dialogs.h"
#include <gtk/gtk.h>

/* GUI */
GtkWindow* window;

GtkTreeView* nodes_view;
GtkListStore* nodes_model;

GtkToggleButton* weight_or_prob;
GtkLabel* total_label;

GtkFileChooser* load_dialog;
GtkFileChooser* save_dialog;

/* Context */
optbst_context* c = NULL;

/* Functions */
void enable_sort(bool e);
void add_row(GtkToolButton *toolbutton, gpointer user_data);
void remove_row(GtkToolButton *toolbutton, gpointer user_data);
void edit_started_cb(GtkCellRenderer* renderer, GtkCellEditable* editable,
                     gchar* path, gpointer user_data);
bool is_unique(char* new, int at_row);
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
    if(!gtk_builder_add_from_file(builder, "gui/optbst.glade", &error)) {
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
    nodes_view = GTK_TREE_VIEW(gtk_builder_get_object(builder, "nodes_view"));
    nodes_model = GTK_LIST_STORE(gtk_builder_get_object(builder, "nodes_model"));
    weight_or_prob = GTK_TOGGLE_BUTTON(gtk_builder_get_object(builder, "weight_or_prob"));
    total_label = GTK_LABEL(gtk_builder_get_object(builder, "total_label"));

    load_dialog = GTK_FILE_CHOOSER(gtk_builder_get_object(builder, "load_dialog"));
    save_dialog = GTK_FILE_CHOOSER(gtk_builder_get_object(builder, "save_dialog"));

    GtkFileFilter* file_filter = gtk_file_filter_new();
    gtk_file_filter_set_name(file_filter, "Custom data file (*.optbst)");
    gtk_file_filter_add_pattern(file_filter, "*.optbst");
    gtk_file_chooser_add_filter(load_dialog, file_filter);
    gtk_file_chooser_add_filter(save_dialog, file_filter);

    /* Configure cell renderers callback */
    GtkCellRenderer* name_renderer = GTK_CELL_RENDERER(
                            gtk_builder_get_object(builder, "name_renderer"));
    g_signal_connect(G_OBJECT(name_renderer),
                         "edited", G_CALLBACK(cell_edited_cb),
                         GINT_TO_POINTER(0));

    GtkCellRenderer* weight_renderer = GTK_CELL_RENDERER(
                            gtk_builder_get_object(builder, "weight_renderer"));
    g_signal_connect(G_OBJECT(weight_renderer),
                         "edited", G_CALLBACK(cell_edited_cb),
                         GINT_TO_POINTER(1));
    edit_started_cb(weight_renderer, NULL, NULL, NULL);

    /* Connect signals */
    gtk_builder_connect_signals(builder, NULL);

    /* Initialize interface */
    add_row(NULL, NULL);
    add_row(NULL, NULL);

    /* Sort model */
    enable_sort(true);

    g_object_unref(G_OBJECT(builder));
    gtk_widget_show(GTK_WIDGET(window));
    gtk_main();

    return(0);
}

void enable_sort(bool e)
{
    if(e) {
        gtk_tree_sortable_set_sort_column_id(
            GTK_TREE_SORTABLE(nodes_model), 0, GTK_SORT_ASCENDING);
    } else {
        gtk_tree_sortable_set_sort_column_id(
            GTK_TREE_SORTABLE(nodes_model),
            GTK_TREE_SORTABLE_UNSORTED_SORT_COLUMN_ID,
            GTK_SORT_ASCENDING);
    }
}

void add_row(GtkToolButton *toolbutton, gpointer user_data)
{
    int rows = gtk_tree_model_iter_n_children(
                                    GTK_TREE_MODEL(nodes_model), NULL);

    GtkTreeIter iter;
    gtk_list_store_append(nodes_model, &iter);
    gtk_list_store_set(nodes_model, &iter,
                        0, sequence_name(rows),
                        1, 0.0010,
                        2, g_strdup_printf("%.4f", 0.0010),
                        -1);

    gtk_label_set_text(total_label,
                       g_strdup_printf("Total: %i nodes.", rows + 1));

    GtkTreePath* model_path = gtk_tree_model_get_path(
                                GTK_TREE_MODEL(nodes_model), &iter);
    gtk_tree_view_set_cursor(nodes_view, model_path,
                             gtk_tree_view_get_column(nodes_view, 0),
                             true);
    gtk_tree_path_free(model_path);

    return;
}

void remove_row(GtkToolButton *toolbutton, gpointer user_data)
{
    int rows = gtk_tree_model_iter_n_children(
                                    GTK_TREE_MODEL(nodes_model), NULL);
    if(rows < 3) {
        return;
    }

    GtkTreeSelection* selection = gtk_tree_view_get_selection(nodes_view);
    GtkTreeIter iter;
    if(gtk_tree_selection_get_selected(selection, NULL, &iter)) {

        bool valid = gtk_list_store_remove(nodes_model, &iter);

        gtk_label_set_text(total_label,
                       g_strdup_printf("Total: %i nodes.", rows  - 1));

        if(!valid) {
            valid = gtk_tree_model_iter_nth_child(GTK_TREE_MODEL(nodes_model),
                                                  &iter, NULL, rows - 1);
        }

        if(valid) {
            GtkTreePath* model_path = gtk_tree_model_get_path(
                                        GTK_TREE_MODEL(nodes_model), &iter);
            gtk_tree_view_set_cursor(nodes_view, model_path, NULL, false);
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
                    0.001,          /* the initial value. */
                    0.000,          /* the minimum value. */
                    1.000,          /* the maximum value. */
                    0.001,          /* the step increment. */
                    0.010,          /* the page increment. */
                    0.000           /* the page size. */
                );
    g_object_set(renderer, "adjustment", adj, NULL);
}

bool is_unique(char* new, int at_row)
{
    GtkTreeModel* model = GTK_TREE_MODEL(nodes_model);
    GtkTreeIter iter;
    bool valid = gtk_tree_model_get_iter_first(model, &iter);

    int row = 0;
    bool unique = true;
    while(valid) {
        char* name;
        gtk_tree_model_get(model, &iter, 0, &name, -1);

        if((row != at_row) && (g_strcmp0(new, name) == 0)) {
            unique = false;
            break;
        }

        g_free(name);

        valid = gtk_tree_model_iter_next(model, &iter);
        row++;
    }

    return unique;
}

void cell_edited_cb(GtkCellRendererText* renderer, gchar* path,
                    gchar* new_text, gpointer user_data)
{
    int row = atoi(path);
    int column = GPOINTER_TO_INT(user_data);
    g_strstrip(new_text); /* Strip string in place */
    printf("%s at (%i, %i)\n", new_text, row, column);

    /* Check if unique */
    if(!is_unique(new_text, row)) {
        show_error(window, "Please set a unique name for each node.");
        return;
    }


    /* Get reference to model */
    GtkTreePath* model_path = gtk_tree_path_new_from_string(path);
    GtkTreeIter iter;
    gtk_tree_model_get_iter(GTK_TREE_MODEL(nodes_model), &iter, model_path);
    gtk_tree_path_free(model_path);

    GValue value = G_VALUE_INIT;

    if(column == 0) {

        if(is_empty_string(new_text)) {
            return;
        }

        g_value_init(&value, G_TYPE_STRING);
        g_value_set_string(&value, new_text);
        gtk_list_store_set_value(nodes_model, &iter, column, &value);

    } else if(!is_empty_string(new_text)) {

        char* end;
        float v = strtof(new_text, &end);
        if((end != new_text) && (*end == '\0') && (v > 0.0)) {

            g_value_init(&value, G_TYPE_FLOAT);
            g_value_set_float(&value, v);
            gtk_list_store_set_value(nodes_model, &iter, 1, &value);

            g_value_unset(&value);

            g_value_init(&value, G_TYPE_STRING);
            g_value_set_string(&value, g_strdup_printf("%.4f", v));
            gtk_list_store_set_value(nodes_model, &iter, 2, &value);
        }
    }
}

void process(GtkButton* button, gpointer user_data)
{
    if(c != NULL) {
        optbst_context_free(c);
    }

    int keys = gtk_tree_model_iter_n_children(
                                    GTK_TREE_MODEL(nodes_model), NULL);

    /* Create context */
    c = optbst_context_new(keys);
    if(c == NULL) {
        show_error(window, "Unable to allocate enough memory for "
                           "this problem. Sorry.");
        return;
    }

    /* Fill context */
    char** names = c->names;
    float* probs = c->keys_probabilities;

    GtkTreeIter iter;
    bool was_set = gtk_tree_model_get_iter_first(
                            GTK_TREE_MODEL(nodes_model), &iter);
    if(!was_set) {
        return;
    }

    GValue value = G_VALUE_INIT;

    float total_weights = 0.0;
    int i = 0;
    do {
        gtk_tree_model_get_value(
                            GTK_TREE_MODEL(nodes_model), &iter, 0, &value);
        char* n = g_value_dup_string(&value);
        g_value_unset(&value);

        gtk_tree_model_get_value(
                            GTK_TREE_MODEL(nodes_model), &iter, 1, &value);
        float v = g_value_get_float(&value);
        g_value_unset(&value);

        /* Set values */
        names[i] = n;
        probs[i] = v;

        was_set = gtk_tree_model_iter_next(
                            GTK_TREE_MODEL(nodes_model), &iter);
        total_weights += v;
        i++;
    } while(was_set);

    /* Check if weights or probs */
    if(gtk_toggle_button_get_active(weight_or_prob)) {
        if(fequal(total_weights, 0.0)) {
            show_error(window, "The total weights of the nodes should not be "
                               "zero. Check your data.");
            return;
        }
        for(int i = 0; i < keys; i++) {
            probs[i] = probs[i] / total_weights;
        }
    }

    /* Verify probabilities equality */
    float total = 0.0;
    for(int i = 0; i < keys; i++) {
        total += probs[i];
    }
    if(!fequal(total, 1.0)) {
        printf("Total sum: %1.4f.\n", total_weights);
        show_error(window, g_strdup_printf(
                           "The sum of the probabilities is not 1.0.\n"
                           "Current sum is %1.4f.\n"
                           "Please check your data.", total_weights)
                        );
        return;
    }

    /* Execute algorithm */
    bool success = optbst(c);
    if(!success) {
        show_error(window, "Error while processing the information. "
                           "Please check your data.");
    }

    /* Show tables */
    printf("-----------------------------------\n");
    matrix_print(c->table_a);

    printf("-----------------------------------\n");
    matrix_print(c->table_r);

    /* Generate report */
    bool report_created = optbst_report(c);
    if(!report_created) {
        show_error(window, "Report could not be created. "
                           "Please check your data.");
    } else {
        printf("Report created at reports/optbst.tex\n");

        int as_pdf = latex2pdf("optbst", "reports");
        if(as_pdf == 0) {
            printf("PDF version available at reports/optbst.pdf\n");
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
    if(!g_str_has_suffix(filename, ".optbst")) {
        char* new_filename = g_strdup_printf("%s.optbst", filename);
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
    fprintf(file, "%i\n", gtk_tree_model_iter_n_children(
                                    GTK_TREE_MODEL(nodes_model), NULL));

    GtkTreeIter iter;
    GValue value = G_VALUE_INIT;

    /* Write node names */
    bool was_set = gtk_tree_model_get_iter_first(
                                    GTK_TREE_MODEL(nodes_model), &iter);
    while(was_set) {
        gtk_tree_model_get_value(
                            GTK_TREE_MODEL(nodes_model), &iter, 0, &value);
        char* n = g_value_dup_string(&value);
        g_value_unset(&value);

        fprintf(file, "%s\n", n);
        g_free(n);

        /* Next */
        was_set = gtk_tree_model_iter_next(
                            GTK_TREE_MODEL(nodes_model), &iter);
    }

    /* Write node names */
    was_set = gtk_tree_model_get_iter_first(
                                    GTK_TREE_MODEL(nodes_model), &iter);
    while(was_set) {
        gtk_tree_model_get_value(
                            GTK_TREE_MODEL(nodes_model), &iter, 1, &value);
        float v = g_value_get_float(&value);
        g_value_unset(&value);

        fprintf(file, "%.4f\n", v);

        /* Next */
        was_set = gtk_tree_model_iter_next(
                            GTK_TREE_MODEL(nodes_model), &iter);
    }

    fprintf(file, "%i\n", gtk_toggle_button_get_active(weight_or_prob));
}

void load(FILE* file)
{
    /* Load number of nodes */
    int num_nodes = 0;
    fscanf(file, "%i%*c", &num_nodes);

    /* Adapt GUI */
    enable_sort(false);
    gtk_list_store_clear(nodes_model);
    for(int i = 0; i < num_nodes; i++) {
        add_row(NULL, NULL);
    }

    /* Load nodes names */
    char** names = (char**) malloc(num_nodes * sizeof(char*));
    for(int i = 0; i < num_nodes; i++) {
        names[i] = get_line(file);
    }

    /* Load nodes data */
    GtkTreeIter iter;
    bool has_row = gtk_tree_model_get_iter_first(
                            GTK_TREE_MODEL(nodes_model), &iter);
    float w = 0.0;
    for(int i = 0; (i < num_nodes) && has_row; i++) {

        /* Get values */
        fscanf(file, "%f%*c", &w);

        /* Set values */
        gtk_list_store_set(nodes_model, &iter,
                    0, names[i],
                    1, w,
                    2, g_strdup_printf("%.4f", w),
                    -1);
        /* Next */
        has_row = gtk_tree_model_iter_next(GTK_TREE_MODEL(nodes_model), &iter);
    }

    /* Set capacity */
    int is_w = 0;
    fscanf(file, "%i%*c", &is_w);
    gtk_toggle_button_set_active(weight_or_prob, (bool)is_w);

    /* Free resources */
    for(int i = 0; i < num_nodes; i++) {
        free(names[i]);
    }
    free(names);

    /* Reorder nodes */
    enable_sort(true);
}
