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

int main(int argc, char **argv)
{
    printf("Testing Probabilities to become champion...\n\n");

    /* Create context */
    probwin_context* c = probwin_context_new(7);
    if(c == NULL) {
        printf("Unable to create Probabilities to become champion context... exiting.\n");
        return(-1);
    }

    matrix* w = c->table_w;
    bool* f = c->game_format;

    /* Set format game */
    f[0] = true;
    f[1] = true;
    f[2] = false;
    f[3] = false;
    f[4] = false;
    f[5] = true;
    f[6] = true;

    /*Set probabilities */
    c->ph = 0.57;
    c->pr = 0.49;

    /* Run algorithm */
    bool success = probwin(c);
    if(!success) {
        printf("Probabilities to become champion algorithm was unable to complete... exiting.\n");
        return(-2);
    }

    /* Show table */
    printf("-----------------------------------\n");
    matrix_print(w);

    /* Free resources */
    probwin_context_free(c);
    return(0);
}
