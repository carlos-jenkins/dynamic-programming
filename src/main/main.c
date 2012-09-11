/*
 * Ejemplo de interfaz con GtkBuilder y Glade
 * http://carlos.jenkins.co.cr/
 *
 * Copyright (c) 2011, 2012 Carlos Jenkins
 * Licensed under the MIT license.
 * http://opensource.org/licenses/mit-license.php
 *
 * Date: Sat, 25 Aug 2012 07:10:47 -0600
 * Revision: 03
 */

/*
 * Compilame con:
 *   make
 * Pero si deseas hacerlo manual:
 *   gcc -o main main.c $(pkg-config --cflags --libs gtk+-3.0)
 */
#include <stdio.h>
#include <gtk/gtk.h>

/* Callback del boton */
void button_clicked(GtkButton *button, gpointer data) {
    printf("Click!!\n");
}

/* Inicio de la aplicacion */
int main(int argc, char **argv) {

    GtkBuilder *builder;
    GtkWidget  *window;
    GError     *error = NULL;

    /* Inicia el sub-sistema Gtk+, esto es necesario antes de usar cualquier cosa Gtk+ */
    gtk_init(&argc, &argv);

    /* Crea un nuevo objeto GtkBuilder */
    builder = gtk_builder_new();
    /* Carga el archivo de interfaz grafica. Si un error ocurre, reportelo y salga de la aplicacion. */
    if(!gtk_builder_add_from_file(builder, "gui/main.glade", &error)) {
        if(error) {
            g_warning("%s", error->message);
            g_error_free(error);
        } else {
            g_warning("Unknown error.");
        }
        return(1);
    }

    /* Obtenga el puntero de ventana principal de la interfaz grafica */
    window = GTK_WIDGET(gtk_builder_get_object(builder, "window1"));

    /* Conecte las senales */
    gtk_builder_connect_signals(builder, NULL);

    /* Podemos destuir el GtkBuilder, pues ya no lo necesitaremos */
    g_object_unref(G_OBJECT(builder));

    /* Muestre la ventana. Los demas objetos seran automaticamente mostrados por GtkBuilder
     * Este paso no es necesario si en las propiedades de la ventana en Glade la configuran
     * como visible por defecto (por defecto es invisible) */
    gtk_widget_show(window);

    /* Inicia el loop principal (a la escucha de acciones en la interfaz grafica) */
    gtk_main();

    return(0);
}

