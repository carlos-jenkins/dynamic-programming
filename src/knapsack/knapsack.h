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

#ifndef H_KNAPSACK
#define H_KNAPSACK

#include "utils.h"
#include "matrix.h"

/**
 * Item definition struct.
 */
typedef struct {
    char* name;
    float value;
    float weight;
    float amount;
} item;
void item_new(item* it, char* name, float value, float weight, float amount);

/**
 * Knapsack algorithm context data structure.
 */
typedef struct {

    /* Common */
    int status;
    int execution_time;
    int memory_required;
    FILE* report_buffer;

    /* Tables */
    matrix* table_values;
    matrix* table_items;

    /* Algorithm */
    int num_items;
    item** items;
    int capacity;
    char* unit;

} knapsack_context;

knapsack_context* knapsack_context_new(int capacity, int num_items);
void knapsack_context_free(knapsack_context* c);

/**
 * Perfom Knapsack algorithm with given context.
 *
 * @param knapsack_context, the knapsack's context data structure.
 * @return TRUE if execution was successful or FALSE if and error ocurred. Check
 *         'status' flag in context to know what went wrong.
 */
bool knapsack(knapsack_context* c);

#include "report.h"

#endif
