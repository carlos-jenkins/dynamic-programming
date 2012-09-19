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

#include "utils.h"
#include <gtk/gtk.h>

/* Helpers */
GObject* go(GtkBuilder* builder, const gchar *name)
{
    return gtk_builder_get_object(builder, name);
}

/* Callbacks */
void floyd_clicked_cb(GtkButton *button, gpointer data)
{
    system("./bin/floyd");
}

void knapsack_clicked_cb(GtkButton *button, gpointer data)
{
    system("./bin/knapsack");
}

void optbst_clicked_cb(GtkButton *button, gpointer data)
{
    system("./bin/optbst");
}

void probwin_clicked_cb(GtkButton *button, gpointer data)
{
    system("./bin/probwin");
}

void replacement_clicked_cb(GtkButton *button, gpointer data)
{
    system("./bin/replacement");
}

/* Main */
int main(int argc, char **argv)
{

    GtkBuilder *builder;
    GError     *error = NULL;

    /* Starts Gtk+ subsystem */
    gtk_init(&argc, &argv);

    /* Load GUI interface */
    builder = gtk_builder_new();
    if(!gtk_builder_add_from_file(builder, "gui/main.glade", &error)) {
        if(error) {
            g_warning("%s", error->message);
            g_error_free(error);
        } else {
            g_warning("Unknown error.");
        }
        return(1);
    }

    /* Get pointers to objects */
    GtkWidget* window = GTK_WIDGET(go(builder, "window"));

    GtkTextBuffer* floyd_buff       =
        GTK_TEXT_BUFFER(go(builder, "floyd_buff"));
    GtkTextBuffer* knapsack_buff    =
        GTK_TEXT_BUFFER(go(builder, "knapsack_buff"));
    GtkTextBuffer* optbst_buff      =
        GTK_TEXT_BUFFER(go(builder, "optbst_buff"));
    GtkTextBuffer* probwin_buff     =
        GTK_TEXT_BUFFER(go(builder, "probwin_buff"));
    GtkTextBuffer* replacement_buff =
        GTK_TEXT_BUFFER(go(builder, "replacement_buff"));

    /* Load text in buffer */
    gtk_text_buffer_set_text(floyd_buff,
        read_file("./latex/floyd.txt"), -1);
    gtk_text_buffer_set_text(knapsack_buff,
        read_file("./latex/knapsack.txt"), -1);
    gtk_text_buffer_set_text(optbst_buff,
        read_file("./latex/optbst.txt"), -1);
    gtk_text_buffer_set_text(probwin_buff,
        read_file("./latex/probwin.txt"), -1);
    gtk_text_buffer_set_text(replacement_buff,
        read_file("./latex/replacement.txt"), -1);

    /* Connect signals */
    gtk_builder_connect_signals(builder, NULL);

    g_object_unref(G_OBJECT(builder));
    gtk_widget_show(window);
    gtk_main();

    return(0);
}

