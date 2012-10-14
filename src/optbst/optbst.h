/*
 * Copyright (C) 2012 Carolina Aguilar <caroagse@gmail.com>
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

#ifndef H_OPTBST
#define H_OPTBST

#include "utils.h"
#include "matrix.h"

/**
 * Optimal Binary Search Tree algorithm context data structure.
 */
typedef struct{

    /* Common */
    int status;
    double execution_time;
    unsigned int memory_required;
    FILE* report_buffer;

    /* Tables */
    matrix* table_a;
    matrix* table_r;

    /*Number of Keys*/
    int keys;
    char** names;

    /* Probabilities */
    float* keys_probabilities;

} optbst_context;

optbst_context* optbst_context_new(int keys);
void optbst_context_free(optbst_context* c);

/**
 * Perform Optimal Binary Search Tree algorithm with given context.
 *
 * @param optbst_context, the Optimal Binary Search Tree's context data structure.
 * @return TRUE if execution was successful or FALSE if and error ocurred. Check
 *         'status' flag in context to know what went wrong.
 */
bool optbst(optbst_context* c);

#include "report.h"

#endif
