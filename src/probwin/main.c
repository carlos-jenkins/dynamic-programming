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

/* Context */
probwin_context* c = NULL;

/* Context */
void prob_changed_cb(GtkSpinButton* spinbutton, gpointer user_data);
void games_changed(int g);
void games_changed_cb(GtkSpinButton* spinbutton, gpointer user_data);
void cell_edited_cb(GtkCellRendererToggle *cell_renderer,
                    gchar* path, gpointer user_data);
void process(GtkButton* button, gpointer user_data);

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
    printf("BOOM!");
}
