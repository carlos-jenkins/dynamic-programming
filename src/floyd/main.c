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

#include "floyd.h"
#include "latex.h"
#include <gtk/gtk.h>
#include <gdk-pixbuf/gdk-pixbuf.h>

/* GUI */
GtkWindow  *window;
GtkTreeView* input;
GtkImage* graph;

/* Context */
floyd_context* c = NULL;

/* Functions */
bool change_matrix(int size);
void change_matrix_cb(GtkSpinButton* spinbutton, gpointer user_data);
void update_graph();
void cell_edited_cb(GtkCellRendererText* renderer, gchar* path,
                    gchar* new_text, gpointer user_data);

int main(int argc, char **argv)
{

    GtkBuilder* builder;
    GError* error = NULL;

    /* Starts Gtk+ subsystem */
    gtk_init(&argc, &argv);

    /* Load GUI interface */
    builder = gtk_builder_new();
    if(!gtk_builder_add_from_file(builder, "gui/floyd.glade", &error)) {
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
    input = GTK_TREE_VIEW(gtk_builder_get_object(builder, "input"));
    graph = GTK_IMAGE(gtk_builder_get_object(builder, "graph"));

    /* Connect signals */
    gtk_builder_connect_signals(builder, NULL);

    /* Initialize interface */
    change_matrix(2);

    g_object_unref(G_OBJECT(builder));
    gtk_widget_show(GTK_WIDGET(window));
    gtk_main();

    return(0);
}

bool change_matrix(int size)
{
    /* Validate input */
    if(size < 2) {
        return false;
    }
    int rsize = size + 1;

    /* Try to create the new context */
    if(c != NULL) {
        floyd_context_free(c);
    }
    c = floyd_context_new(size);
    if(c == NULL) {
        return false;
    }

    /* Initialize context */
    for(int i = 0; i < size; i++) {
        c->names[i] = g_strdup_printf("%i", i + 1);
    }

    /* Create the dynamic types array */
    GType* types = (GType*) malloc(3 * rsize * sizeof(GType));
    if(types == NULL) {
        floyd_context_free(c);
        return false;
    }
    for(int i = 0; i < rsize; i++) {
        types[i] = G_TYPE_STRING;
        types[rsize + i] = G_TYPE_BOOLEAN;     /* Editable */
        types[2 * rsize + i] = G_TYPE_BOOLEAN; /* Background set */
    }

    /* Create and fill the new model */
    GtkListStore* model = gtk_list_store_newv(3 * rsize, types);
    GtkTreeIter iter;

    GValue init = G_VALUE_INIT;
    g_value_init(&init, G_TYPE_STRING);

    GValue initb = G_VALUE_INIT;
    g_value_init(&initb, G_TYPE_BOOLEAN);

    for(int i = 0; i < rsize; i++) {
        gtk_list_store_append(model, &iter);

        /* First row */
        if(i == 0) {
            for(int j = 0; j < rsize; j++) {
                /* Set value */
                if(j > 0) {
                    g_value_set_string(&init, g_strdup_printf("%i", j));
                    g_value_set_boolean(&initb, true);
                } else {
                    g_value_set_string(&init, "");
                    g_value_set_boolean(&initb, false);
                }
                gtk_list_store_set_value(model, &iter, j, &init);

                /* Set editable */
                gtk_list_store_set_value(model, &iter, rsize + j, &initb);

                /* Set background set */
                g_value_set_boolean(&initb, true);
                gtk_list_store_set_value(model, &iter, 2 * rsize + j, &initb);
            }
            continue;
        }

        /* Other rows, first cell */
        /* Set value */
        g_value_set_string(&init, g_strdup_printf("%i", i));
        gtk_list_store_set_value(model, &iter, 0, &init);

        /* Set editable */
        g_value_set_boolean(&initb, false);
        gtk_list_store_set_value(model, &iter, rsize, &initb);

        /* Set background set */
        g_value_set_boolean(&initb, true);
        gtk_list_store_set_value(model, &iter, 2 * rsize, &initb);

        /* Other rows, other cells */
        for(int j = 1; j < rsize; j++) {

            /* Set value */
            if(i == j) {
                g_value_set_string(&init, "0");
            } else {
                g_value_set_string(&init, "oo");
            }
            gtk_list_store_set_value(model, &iter, j, &init);

            /* Set editable */
            g_value_set_boolean(&initb, i != j);
            gtk_list_store_set_value(model, &iter, rsize + j, &initb);

            /* Set background set */
            g_value_set_boolean(&initb, false);
            gtk_list_store_set_value(model, &iter, 2 * rsize + j, &initb);
        }
    }

    /* Clear the previous matrix */
    for(int i = gtk_tree_view_get_n_columns(input) - 1; i >= 0; i--) {
        gtk_tree_view_remove_column(input,
                                        gtk_tree_view_get_column(input, i)
                                    );
    }

    /* Create the matrix */
    for(int i = 0; i < rsize; i++) {
        /* Configure cell */
        GtkCellRenderer* cell = gtk_cell_renderer_text_new();
        g_object_set(cell, "cell-background", "Black", NULL);
        g_signal_connect(G_OBJECT(cell),
                         "edited", G_CALLBACK(cell_edited_cb),
                         GINT_TO_POINTER(i));
        /* Configure column */
        GtkTreeViewColumn* column = gtk_tree_view_column_new_with_attributes(
                                        "", cell,  /* Title, renderer */
                                        "text", i,
                                        "editable", rsize + i,
                                        "cell-background-set", 2 * rsize + i,
                                        NULL);
        gtk_tree_view_append_column(input, column);
    }
    GtkTreeViewColumn* close_column = gtk_tree_view_column_new();
    gtk_tree_view_append_column(input, close_column);

    /* Set the new model */
    gtk_tree_view_set_model(input, GTK_TREE_MODEL(model));

    /* Update the graph */
    update_graph();

    /* Free resources */
    g_object_unref(G_OBJECT(model));
    free(types);

    return true;
}

void update_graph()
{
    if(c == NULL) {
        return;
    }

    /* Create graph */
    floyd_graph(c);
    if(gv2png("graph", "reports") < 0) {
        gtk_image_set_from_pixbuf(graph, NULL);
        return;
    }

    /* Load image */
    GError* error = NULL;
    //GdkPixbuf* pixbuf = gdk_pixbuf_new_from_file_at_size(
                            //"reports/graph.png", 300, 300, &error);
    GdkPixbuf* pixbuf = gdk_pixbuf_new_from_file("reports/graph.png", &error);
    if(pixbuf == NULL) {
        if(error) {
            g_warning("%s", error->message);
            g_error_free(error);
        } else {
            g_warning("Unknown while loading the graph.");
        }
        return;
    }
    gtk_image_set_from_pixbuf(graph, pixbuf);

    g_object_unref(pixbuf);
}

void cell_edited_cb(GtkCellRendererText* renderer, gchar* path,
                    gchar* new_text, gpointer user_data)
{
    int row = atoi(path);
    int column = GPOINTER_TO_INT(user_data);
    printf("%s at (%i, %i)\n", new_text, row, column);

    /* Validate row and column just in case */
    if((row == column) || column == 0) {
        return;
    }

    /* Get reference to model */
    GtkTreePath* model_path = gtk_tree_path_new_from_string(path);
    GtkTreeIter iter;
    gtk_tree_model_get_iter(gtk_tree_view_get_model(input), &iter, model_path);
    gtk_tree_path_free(model_path);

    GValue value = G_VALUE_INIT;
    g_value_init(&value, G_TYPE_STRING);

    /* A node is being renamed */
    if(row == 0) {
        if(!is_empty_string(new_text)) {
            /* Set node at first row */
            g_value_set_string(&value, new_text);
            gtk_list_store_set_value(
                            GTK_LIST_STORE(gtk_tree_view_get_model(input)),
                            &iter, column, &value);

            /* Set node at first column */
            char* at_y = g_strdup_printf("%i", column);
            model_path = gtk_tree_path_new_from_string(at_y);
            gtk_tree_model_get_iter(
                    gtk_tree_view_get_model(input), &iter, model_path);
            gtk_tree_path_free(model_path);
            g_free(at_y);
            gtk_list_store_set_value(
                            GTK_LIST_STORE(gtk_tree_view_get_model(input)),
                            &iter, 0, &value);

            g_free(c->names[column - 1]);
            c->names[column - 1] = g_strdup(new_text);

            /* Update the graph */
            update_graph();
        }
        return;
    }

    /* A distance is being set */
    /* INFINITY */
    int is_inf = strncmp(new_text, "oo", 2);
    if(is_inf == 0) {
        g_value_set_string(&value, "oo");
        gtk_list_store_set_value(
                            GTK_LIST_STORE(gtk_tree_view_get_model(input)),
                            &iter, column, &value);

        c->table_d->data[row - 1][column - 1] = PLUS_INF;

        /* Update the graph */
        update_graph();
        return;
    }
    /* Distance */
    char* end;
    int distance = (int) strtol(new_text, &end, 10);
    if((end != new_text) && (*end == '\0') && (distance > 0)) {
        char* distance_as_string = g_strdup_printf("%i", distance);
        g_value_set_string(&value, distance_as_string);
        gtk_list_store_set_value(
                            GTK_LIST_STORE(gtk_tree_view_get_model(input)),
                            &iter, column, &value);
        g_free(distance_as_string);

        c->table_d->data[row - 1][column - 1] = (float) distance;

        /* Update the graph */
        update_graph();
        return;
    }
    return;

}

void change_matrix_cb(GtkSpinButton* spinbutton, gpointer user_data)
{
    /* Get the number of srequested nodes */
    int value = gtk_spin_button_get_value_as_int(spinbutton);
    bool success = change_matrix(value);
    /* FIXME: Do something with success */
    return;
}
