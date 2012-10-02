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

#include "optbst.h"
#include "latex.h"
#include "dialogs.h"
#include <gtk/gtk.h>

/* GUI */
GtkWindow* window;

GtkTreeView* nodes_view;
GtkListStore* nodes_model;

/* Context */
optbst_context* c = NULL;

/* Functions */
void add_row(GtkToolButton *toolbutton, gpointer user_data);
void remove_row(GtkToolButton *toolbutton, gpointer user_data);
void edit_started_cb(GtkCellRenderer* renderer, GtkCellEditable* editable,
                     gchar* path, gpointer user_data);
void cell_edited_cb(GtkCellRendererText* renderer, gchar* path,
                    gchar* new_text, gpointer user_data);
void process(GtkButton* button, gpointer user_data);

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

    /* Configure cell renderers callback */
    GtkCellRenderer* name_renderer = GTK_CELL_RENDERER(
                            gtk_builder_get_object(builder, "name_renderer"));
    g_signal_connect(G_OBJECT(name_renderer),
                         "edited", G_CALLBACK(cell_edited_cb),
                         GINT_TO_POINTER(0));

    GtkCellRenderer* prob_renderer = GTK_CELL_RENDERER(
                            gtk_builder_get_object(builder, "prob_renderer"));
    g_signal_connect(G_OBJECT(prob_renderer),
                         "edited", G_CALLBACK(cell_edited_cb),
                         GINT_TO_POINTER(1));
    edit_started_cb(prob_renderer, NULL, NULL, NULL);

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
                                    GTK_TREE_MODEL(nodes_model), NULL);

    GtkTreeIter iter;
    gtk_list_store_append(nodes_model, &iter);
    gtk_list_store_set(nodes_model, &iter,
                        0, sequence_name(rows),
                        1, 0.001,
                        -1);

    GtkTreePath* model_path = gtk_tree_model_get_path(
                                GTK_TREE_MODEL(nodes_model), &iter);
    gtk_tree_view_set_cursor(nodes_view, model_path, NULL, false);
    gtk_tree_path_free(model_path);

    gtk_widget_grab_focus(GTK_WIDGET(nodes_view));
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

void cell_edited_cb(GtkCellRendererText* renderer, gchar* path,
                    gchar* new_text, gpointer user_data)
{
    int row = atoi(path);
    int column = GPOINTER_TO_INT(user_data);
    printf("%s at (%i, %i)\n", new_text, row, column);

    /* Get reference to model */
    GtkTreePath* model_path = gtk_tree_path_new_from_string(path);
    GtkTreeIter iter;
    gtk_tree_model_get_iter(GTK_TREE_MODEL(nodes_model), &iter, model_path);
    gtk_tree_path_free(model_path);

    GValue value = G_VALUE_INIT;

    if(column == 0) {
        g_value_init(&value, G_TYPE_STRING);
        g_value_set_string(&value, new_text);
        gtk_list_store_set_value(nodes_model, &iter, column, &value);
    } else if(!is_empty_string(new_text)) {
        char* end;
        float v = strtof(new_text, &end);
        if((end != new_text) && (*end == '\0') && (v > 0.0)) {
            g_value_init(&value, G_TYPE_FLOAT);
            g_value_set_float(&value, v);
            gtk_list_store_set_value(nodes_model, &iter, column, &value);
        }
    }
}

void process(GtkButton* button, gpointer user_data)
{
    printf("BOOM!\n");
}
