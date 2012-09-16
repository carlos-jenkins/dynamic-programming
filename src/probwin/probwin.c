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

probwin_context* probwin_context_new(int games)
{
    /* Check input is correct */
    if(games % 2 == 0) {
        return NULL;
    }

    /* Allocate structures */
    probwin_context* c = (probwin_context*) malloc(sizeof(probwin_context));
    if(c == NULL) {
        return NULL;
    }

    /* Try to allocate game format */
    c->game_format = (bool*) malloc(games * sizeof(bool));
    if(c->game_format == NULL) {
        return NULL;
    }

    /* Calculate games needed to win */
    int games_to_win = (games + 1) / 2;

    /* Try to allocate matrices */
    int size = games_to_win + 1;
    c->table_w = matrix_new(size, size, 0.0);
    if(c->table_w == NULL) {
        free(c->game_format);
        return NULL;
    }

    /* Initialize values */
    c->table_w->data[0][0] = PLUS_INF;
    for(int i = 1; i <= games_to_win; i++) {
        c->table_w->data[0][i] = 1.0;
    }

    c->status = -1;
    c->execution_time = 0;
    c->memory_required = matrix_sizeof(c->table_w) + (games * sizeof(bool)) +
                         sizeof(probwin_context);
    c->report_buffer = tmpfile();

    return c;
}

void probwin_context_free(probwin_context* c)
{
    matrix_free(c->table_w);
    fclose(c->report_buffer);
    free(c->game_format);
    free(c);
    return;
}

bool probwin(probwin_context *c)
{
    /* Start counting time */
    GTimer* timer = g_timer_new();

    /* Run the probabilities to win algorithm */
    matrix* w = c->table_w;
    float ph = c->ph;
    float pr = c->pr;
    int games_to_win = w->rows;

    for(int i = 1; i <= games_to_win; i++) {
        for(int j = 1; j <= games_to_win; j++) {
            int actual_game = (games_to_win + 1 - i - j);
            if(c->game_format[actual_game]) {
                w->data[i][j] = ph * w->data[i-1][j] +
                                (1.0 - ph) * w->data[i][j-1];
            } else {
                w->data[i][j] = pr * w->data[i-1][j] +
                                (1.0 - pr) * w->data[i][j - 1];
            }
        }
    }

    /* Stop counting time */
    g_timer_stop(timer);
    c->execution_time = g_timer_elapsed(timer, NULL);
    return true;
}
