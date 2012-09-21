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

#ifndef H_REPLACEMENT
#define H_REPLACEMENT

#include "utils.h"
#include "matrix.h"

/**
 * Equipment replacement algorithm context data structure.
 */
typedef struct {

    /* Common */
    int status;
    double execution_time;
    unsigned int memory_required;
    FILE* report_buffer;

    /*Plan and equipment data*/
    int years_plan;
    float equipment_cost;
    int lifetime;
    float* manteinance; /*sizeof lifetime*/
    float* sale_cost; /*sizeof lifetime*/
    float* minimum_cost;


    /*Tables*/
    matrix* table_c;


} replacement_context;

replacement_context* replacement_context_new(int years_plan, int lifetime);
void replacement_context_free(replacement_context* c);

/**
 * Perform equipment replacement algorithm with given context.
 *
 * @param replacement_context, the equipment replacement context data structure.
 * @return TRUE if execution was successful or FALSE if and error ocurred. Check
 *         'status' flag in context to know what went wrong.
 */
bool replacement(replacement_context* c);

#include "report.h"

#endif
