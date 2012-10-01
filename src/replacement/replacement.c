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

replacement_context* replacement_context_new(int years_plan, int lifetime)
{

    /* Check input is correct */
    if(years_plan < 1 || lifetime < 1) {
        return NULL;
    }

    /* Allocate structures */
    replacement_context* c = (replacement_context*) malloc(sizeof(replacement_context));
    if(c == NULL) {
        return NULL;
    }

    /* Try to allocate manteinance, sale cost and minimum cost arrays */
    c->manteinance = (float*) malloc(lifetime * sizeof(float));
    if(c->manteinance == NULL) {
        return NULL;
    }
    c->sale_cost = (float*) malloc(lifetime * sizeof(float));
    if(c->sale_cost == NULL) {
        free(c->manteinance);
        return NULL;
    }
    c->equipment_cost  = (float*) malloc(lifetime * sizeof(float));
     if(c->equipment_cost == NULL) {
        free(c->manteinance);
        free(c->equipment_cost);
        return NULL;
    }
    int size = years_plan + 1;
    c->minimum_cost = (float*) malloc(size * sizeof(float));
    if(c->minimum_cost == NULL) {
        free(c->manteinance);
        free(c->sale_cost);
        free(c->equipment_cost );
        return NULL;
    }

    c->equipment = (char*) malloc( sizeof(char*));
    c->years_plan = years_plan;
    c->lifetime = lifetime;
    

    /* Try to allocate matrices */
    c->table_c = matrix_new(years_plan, years_plan, 0.0);
    if(c->table_c == NULL) {
        free(c->manteinance);
        free(c->sale_cost);
        free(c->equipment_cost);
        free(c->minimum_cost);
        return NULL;
    }

    /* Initialize values */
    for(int i = 0; i < size; i++) {
        c->minimum_cost[i] = PLUS_INF;
    }

    c->minimum_cost[size - 1] = 0;

    c->status = -1;
    c->execution_time = 0;
    c->memory_required = matrix_sizeof(c->table_c) +
                         (3 * lifetime * sizeof(float)) +
                         (size * sizeof(float))+
                         sizeof(replacement_context);
    c->report_buffer = tmpfile();
    if(c->report_buffer == NULL) {
        matrix_free(c->table_c);
        free(c->manteinance);
        free(c->sale_cost);
        free(c->minimum_cost);
        free(c->equipment_cost);
        free(c);
        return NULL;
    }

    return c;
}

void replacement_context_free(replacement_context* c)
{
    matrix_free(c->table_c);
    fclose(c->report_buffer);
    free(c->manteinance);
    free(c->sale_cost);
    free(c->equipment_cost);
    free(c->minimum_cost);
    free(c);
    return;
}

bool replacement(replacement_context* c)
{

    /* Start counting time */
    GTimer* timer = g_timer_new();

    /* Filling the Costs table */
    for(int j = 0; j < c->lifetime; j++) {
        for(int i = 1; i <= c->years_plan - j; i++) {
            float cost = c->equipment_cost[j] - c->sale_cost[j];
            /* Calculating the accumulated cost */
            for(int k = 0; k <= j; k++) {
                    cost += c->manteinance[k];
            }
            c->table_c->data[i -1][j +i - 1] = cost;
        }
    }
    /* Run the equipment replacement algorithm */

    /* Calculate the minimun cost */
    for(int i = c->years_plan; 0 <= i; i--) {
        for(int j = i + 1; j <= c->years_plan; j++) {
            float min = c->table_c->data[i][j - 1] + c->minimum_cost[j];
            if(min < c->minimum_cost[i] && j - i <= c->lifetime) {
                c->minimum_cost[i] = min;
            }
        }
    }

    /* Stop counting time */
    g_timer_stop(timer);
    c->execution_time = g_timer_elapsed(timer, NULL);
    g_timer_destroy(timer);
    return true;
}
