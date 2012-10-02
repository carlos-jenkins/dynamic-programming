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
void check_odd_cb(GtkSpinButton* spinbutton, gpointer user_data);
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

void check_odd_cb(GtkSpinButton* spinbutton, gpointer user_data)
{
    int value = gtk_spin_button_get_value_as_int(num_games);
    show_error(window, "Number of games must be odd.");
    gtk_spin_button_set_value(3.0);
}

void cell_edited_cb(GtkCellRendererToggle *cell_renderer,
                    gchar* path, gpointer user_data)
{

}

void process(GtkButton* button, gpointer user_data)
{
    printf("BOOM!");
}
