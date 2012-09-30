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

optbst_context* optbst_context_new(int keys)
{
    /* Check input is correct */
    if(keys < 1) {
        return NULL;
    }

    /* Allocate structures */
    optbst_context* c = (optbst_context*) malloc(sizeof(optbst_context));
    if(c == NULL) {
        return NULL;
    }

    /* Try to allocate keys' probabilities */
    c->keys_probabilities = (float*) malloc(keys * sizeof(float));
    if(c->keys_probabilities == NULL) {
        return NULL;
    }

    /* Try to allocate matrices */
    int size = keys + 1;
    c->table_a = matrix_new(size, size, PLUS_INF);
    if(c->table_a == NULL) {
        free(c->keys_probabilities);
        return NULL;
    }
    c->table_r = matrix_new(size, size, 0.0);
    if(c->table_r == NULL) {
        free(c->keys_probabilities);
        matrix_free(c->table_a);
        return NULL;
    }

    /* Try to allocate names array */
    c->names = (char**) malloc(keys * sizeof(char*));
    if(c->names == NULL) {
        matrix_free(c->table_a);
        matrix_free(c->table_r);
        free(c->keys_probabilities);
        free(c);
        return NULL;
    }
    c->keys = keys;

    /* Initialize values */
    for(int i = 0; i < size; i++) {
        c->table_a->data[i][i] = 0.0;
    }
    for(int i = 0; i < keys; i++) {
        c->names[i] = "";
    }

    c->status = -1;
    c->execution_time = 0;
    c->memory_required = matrix_sizeof(c->table_a) +
                         matrix_sizeof(c->table_r) +
                         (keys * sizeof(float)) +
                         (keys * sizeof(char*)) +
                         sizeof(optbst_context);
    c->report_buffer = tmpfile();
    if(c->report_buffer == NULL) {
        matrix_free(c->table_a);
        matrix_free(c->table_r);
        free(c->keys_probabilities);
        free(c);
        return NULL;
    }

    return c;
}

void optbst_context_free(optbst_context* c)
{
    matrix_free(c->table_a);
    matrix_free(c->table_r);
    fclose(c->report_buffer);
    free(c->keys_probabilities);
    free(c->names);
    free(c);
    return;
}

bool optbst(optbst_context *c)
{
    /* Start counting time */
    GTimer* timer = g_timer_new();

    /* Setting probabilities values */
    for(int i = 0; i < c->keys; i++) {
        c->table_a->data[i][i + 1] = c->keys_probabilities[i];
    }
    /* Setting winning k for given probabilities in R */
    for(int i = 0; i < c->keys; i++) {
        c->table_r->data[i][i + 1] = i + 1;
    }

    /* Run the probabilities to win algorithm */

    /* c->keys-1=Numbers of diagonals to fill */
    for(int j = 1; j <= c->keys - 1; j++) {
        for(int i = 1; i <= c->keys - j; i++) {
            for(int k = i;  k <= i + j; k++) {
                float p = 0.0;

                /* Calculate the probability */
                for(int l = i; l <= i + j; l++) {
                    p += c->keys_probabilities[l - 1];
                }
                float t = c->table_a->data[i - 1][k - 1] +
                          c->table_a->data[k][i + j] + p;

                /* Compare to get the minimun value */
                if(t < c->table_a->data[i - 1][i + j]) {
                    c->table_a->data[i - 1][i + j] = t;
                    c->table_r->data[i - 1][i + j] = k;
                }
            }
        }
    }

    /* Stop counting time */
    g_timer_stop(timer);
    c->execution_time = g_timer_elapsed(timer, NULL);
    g_timer_destroy(timer);
    return true;
}
