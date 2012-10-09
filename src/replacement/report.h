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

#ifndef H_REPLACEMENT_REPORT
#define H_REPLACEMENT_REPORT

#include "replacement.h"

/**
 * Write a report about the execution of the algorithm.
 *
 * @param replacement_context, the replacement's context data structure after
 *        successful execution.
 * @return if report creation was successful.
 */
bool replacement_report(replacement_context* c);
void replacement_table(replacement_context* c, FILE* stream, matrix* m, bool is_c, char* msj);
void replacement_analisis(replacement_context* c, FILE* stream);
void replacement_data(replacement_context* c, FILE* stream);
void replacement_mincost(replacement_context* c, FILE* stream);
void find_path(matrix* m, int i, FILE* stream);
void replacement_path(replacement_context* c, FILE* stream);


#endif
