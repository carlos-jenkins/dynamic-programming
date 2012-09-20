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

#ifndef H_KNAPSACK_REPORT
#define H_KNAPSACK_REPORT

#include "knapsack.h"

/**
 * Write a report about the execution of the algorithm.
 *
 * @param knapsack_context, the knapsack's context data structure after
 *        successful execution.
 * @return if report creation was successful.
 */
bool knapsack_report(knapsack_context* c);
void knapsack_items(knapsack_context* c, FILE* stream);
void knapsack_table(knapsack_context* c, FILE* stream);

#endif
