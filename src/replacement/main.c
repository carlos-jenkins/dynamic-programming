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
GtkSpinButton* life;
GtkSpinButton* plan;

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
    life = GTK_SPIN_BUTTON(gtk_builder_get_object(builder, "life"));
    plan = GTK_SPIN_BUTTON(gtk_builder_get_object(builder, "plan"));

    /* Configure cell renderers callback */
    GtkCellRenderer* new_renderer = GTK_CELL_RENDERER(
                            gtk_builder_get_object(builder, "new_renderer"));
    g_signal_connect(G_OBJECT(new_renderer),
                         "edited", G_CALLBACK(cell_edited_cb),
                         GINT_TO_POINTER(1));
    edit_started_cb(new_renderer, NULL, NULL, NULL);

    GtkCellRenderer* maint_renderer = GTK_CELL_RENDERER(
                            gtk_builder_get_object(builder, "maint_renderer"));
    g_signal_connect(G_OBJECT(maint_renderer),
                         "edited", G_CALLBACK(cell_edited_cb),
                         GINT_TO_POINTER(2));
    edit_started_cb(maint_renderer, NULL, NULL, NULL);

    GtkCellRenderer* sale_renderer = GTK_CELL_RENDERER(
                            gtk_builder_get_object(builder, "sale_renderer"));
    g_signal_connect(G_OBJECT(sale_renderer),
                         "edited", G_CALLBACK(cell_edited_cb),
                         GINT_TO_POINTER(3));
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
        float v = strtol(new_text, &end, 10);
        if((end != new_text) && (*end == '\0') && (v > 0.0)) {
            g_value_init(&value, G_TYPE_FLOAT);
            g_value_set_float(&value, v);
            gtk_list_store_set_value(costs_model, &iter, column, &value);
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
                        3, 1000.0,
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
     float* mt = c->manteinance;
    float* sc = c->sale_cost;
    float* ec= c->equipment_cost;
    GtkTreeIter iter;
    bool was_set = gtk_tree_model_get_iter_first(
                            GTK_TREE_MODEL(costs_model), &iter);
    if(!was_set) {
        return;
    }

    GValue value = G_VALUE_INIT;

    int i = 0;

    do {
        gtk_tree_model_get_value(
                            GTK_TREE_MODEL(costs_model), &iter, 1, &value);
        int c = g_value_get_int(&value);
        g_value_unset(&value);

        gtk_tree_model_get_value(
                            GTK_TREE_MODEL(costs_model), &iter, 2, &value);
        int m = g_value_get_int(&value);
        g_value_unset(&value);

        gtk_tree_model_get_value(
                            GTK_TREE_MODEL(costs_model), &iter, 3, &value);
        int s = g_value_get_int(&value);
        g_value_unset(&value);

        /* Set values */
        ec[i] = c;
        mt[i] = m;
        sc[i] = s;

        was_set = gtk_tree_model_iter_next(
                            GTK_TREE_MODEL(costs_model), &iter);
        i++;
    } while(was_set);

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
