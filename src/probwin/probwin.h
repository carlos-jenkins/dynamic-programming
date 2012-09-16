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

#ifndef H_PROBWIN
#define H_PROBWIN

#include "utils.h"
#include "matrix.h"

/**
 * Probabilities to become champion algorithm context data structure.
 */

 typedef struct{
     /* Common */
     int status;
     double execution_time;
     unsigned int memory_required;
     FILE* report_buffer;

     /* Tables*/
     matrix* table_w;

    /* Probabilities */
    float ph;
    float pr;

    /*Game format*/
    bool* game_format;

} probwin_context;

probwin_context* probwin_new(int games);
void probwin_context_free(probwin_context* c);

/**
 * Perfom Probabilities to become champion algorithm with given context.
 *
 * @param probwin_context, the Probabilities to become champion's context data structure.
 * @return TRUE if execution was successful or FALSE if and error ocurred. Check
 *         'status' flag in context to know what went wrong.
 */
bool probwin(probwin_context* c);

/**
 * Write report about the execution of the algorith.
 *
 * @param probwin_context, the probwin's context data structure after success.
 * @return nothing.
 */
void probwin_report(probwin_context* c);

#endif
