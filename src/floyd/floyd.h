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

#ifndef H_FLOYD
#define H_FLOYD

#include <stdio.h>
#include <stdbool.h>
#include <glib.h>
#include "matrix.h"

/**
 * Floyd's algorithm context data structure.
 */
typedef struct {

        /* Common */
        int status;
        int execution_time;
        int memory_required;

        /* Tables */
        matrix* table_d;
        matrix* table_p;

        /* Report */
        int start;
        int end;
        int* names;

} floyd_context;

/**
 * Perfom Floyd algorithm with given context.
 *
 * @param floyd_context, the floyd's context data structure.
 * @return TRUE if execution was successful or FALSE if and error ocurred. Check
 *         'status' flag in context to know what went wrong.
 */
bool floyd(floyd_context* c);

#endif
