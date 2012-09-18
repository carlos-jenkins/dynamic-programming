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

#include "knapsack.h"

void item_new(item* it, char* name, float value, float weight, float amount)
{
    it->name = name;
    it->value = value;
    it->weight = weight;
    it->amount = amount;
}

knapsack_context* knapsack_context_new(int capacity, int num_items)
{
    /* Check input is correct */
    if((capacity < 1) || (num_items < 1)) {
        return NULL;
    }

    /* Allocate structure */
    knapsack_context* c = (knapsack_context*) malloc(sizeof(knapsack_context));
    if(c == NULL) {
        return NULL;
    }

    /* Try to allocate matrices */
    c->table_values = matrix_new(capacity + 1, num_items, 0.0);
    if(c->table_values == NULL) {
        free(c);
        return NULL;
    }
    c->table_items = matrix_new(capacity + 1, num_items, 0.0);
    if(c->table_items == NULL) {
        matrix_free(c->table_values);
        free(c);
        return NULL;
    }

    /* Try to allocate items array */
    c->items = (item**) malloc(num_items * sizeof(item*));
    if(c->items == NULL) {
        matrix_free(c->table_values);
        matrix_free(c->table_items);
        free(c);
        return NULL;
    }
    for(int i = 0; i < num_items; i++) {

        /* Allocate memory for item i */
        c->items[i] = (item*) malloc(sizeof(item));

        /* Check if allocation could be done */
        if(c->items[i] == NULL) {

            /* Allocation failed, rollback and free memory */
            for(int j = (i - 1); j >= 0; j--) {
                free(c->items[j]);
            }

            /* Free the items array */
            free(c->items);
            matrix_free(c->table_values);
            matrix_free(c->table_items);
            free(c);
            return NULL;
        }
    }

    /* Initialize the items array */
    for(int i = 0; i < num_items; i++) {
        c->items[i]->name = "";
        c->items[i]->value = 0.0;
        c->items[i]->weight = 0.0;
        c->items[i]->amount = 0.0;
    }

    c->num_items = num_items;
    c->capacity = capacity;
    c->unit = "";

    c->status = -1;
    c->execution_time = 0.0;
    c->memory_required = (matrix_sizeof(c->table_values) * 2) +
                         (num_items * sizeof(item)) +
                         (num_items * sizeof(item*)) +
                         sizeof(knapsack_context);
    c->report_buffer = tmpfile();

    return c;
}

void knapsack_context_free(knapsack_context* c)
{
    matrix_free(c->table_values);
    matrix_free(c->table_items);
    for(int i = 0; i < c->num_items; i++) {
        free(c->items[i]);
    }
    free(c->items);
    fclose(c->report_buffer);
    free(c);
    return;
}

bool knapsack(knapsack_context *c)
{
    for(int i = 0; i < c->table_values->rows; i++) {
        for(int j = 0; j < c->table_values->columns; j++) {

            /* Calculate Q and overflow */
            item* it = c->items[j];
            int q = fminf(it->amount, floorf((float)i / it->weight));
            bool y_overflow = (j - 1) < 0;

            /* Default and non putting the item */
            int taken = 0;
            float value = 0.0;
            if(!y_overflow) {
                value = c->table_values->data[i][j - 1];
            }

            /* Calculate if putting the item */
            for(int times = 1; times < q + 1; times++) {
                float pay = (float)times * it->value;
                int prev_row = (int) floorf((float)i - (times * it->weight));
                bool x_overflow = prev_row < 0;
                if((!x_overflow) && (!y_overflow)) {
                    pay += c->table_values->data[prev_row][j - 1];
                }
                if(pay > value) {
                    value = pay;
                    taken = times;
                }
            }

            c->table_values->data[i][j] = value;
            c->table_items->data[i][j] = (float)taken;
        }
    }

    return true;
}
