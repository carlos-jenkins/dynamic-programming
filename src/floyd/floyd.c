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

floyd_context* floyd_context_new(int nodes)
{
    /* Check input is correct */
    if(nodes < 2) {
        return NULL;
    }

    /* Allocate structure */
    floyd_context* c = (floyd_context*) malloc(sizeof(floyd_context));
    if(c == NULL) {
        return NULL;
    }

    /* Try to allocate matrices */
    c->table_d = matrix_new(nodes, nodes, PLUS_INF);
    if(c->table_d == NULL) {
        free(c);
        return NULL;
    }
    c->table_p = matrix_new(nodes, nodes, 0.0);
    if(c->table_p == NULL) {
        matrix_free(c->table_d);
        free(c);
        return NULL;
    }

    /* Try to allocate items array */
    c->names = (char**) malloc(nodes * sizeof(char*));
    if(c->names == NULL) {
        matrix_free(c->table_d);
        matrix_free(c->table_p);
        free(c);
        return NULL;
    }

    /* Initialize values */
    for(int i = 0; i < nodes; i++) {
        c->table_d->data[i][i] = 0.0;
        c->names[i] = "";
    }

    c->nodes = nodes;

    c->status = -1;
    c->execution_time = 0.0;
    c->memory_required = (matrix_sizeof(c->table_d) * 2) +
                         (nodes * sizeof(char*)) +
                         sizeof(floyd_context);
    c->report_buffer = tmpfile();
    if(c->report_buffer == NULL) {
        matrix_free(c->table_d);
        matrix_free(c->table_p);
        free(c->names);
        free(c);
        return NULL;
    }

    return c;
}

void floyd_context_clear(floyd_context* c)
{
    matrix_fill(c->table_p, 0.0);
    fclose(c->report_buffer);
    c->report_buffer = tmpfile();
}

void floyd_context_free(floyd_context* c)
{
    matrix_free(c->table_d);
    matrix_free(c->table_p);
    fclose(c->report_buffer);
    free(c->names);
    free(c);
    return;
}

bool floyd(floyd_context *c)
{
    /* Create graph */
    floyd_graph(c);

    /* Start counting time */
    GTimer* timer = g_timer_new();

    /* Run the Floyd Warshall algorithm */
    matrix* d = c->table_d;
    matrix* p = c->table_p;
    int nodes = d->rows;

    for(int k = 0; k < nodes; k++) {
        for(int i = 0; i < nodes; i++) {
            for(int j = 0; j < nodes; j++) {
                float minimum = fminf(d->data[i][j],
                                      d->data[i][k] + d->data[k][j]);
                if(minimum < d->data[i][j]) {
                    p->data[i][j] = k + 1;
                    d->data[i][j] = minimum;
                }
            }
        }
        /* Log execution */
        floyd_execution(c, k + 1);
    }

    /* Stop counting time */
    g_timer_stop(timer);
    c->execution_time = g_timer_elapsed(timer, NULL);
    return true;
}
