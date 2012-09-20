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
#include "latex.h"

int main(int argc, char **argv)
{
    printf("Testing Knapsack algorithm...\n\n");

    /* Create context */
    knapsack_context* c = knapsack_context_new(20, 8);
    if(c == NULL) {
        printf("ERROR: Unable to create knapsack context... exiting.\n");
        return(-1);
    }

    /* Fill context */
    item** its = c->items;
    item_new(its[0], "A",  3, 8, 1); /* value, weight, amount */
    item_new(its[1], "B", 10, 2, 2);
    item_new(its[2], "C",  2, 5, 1);
    item_new(its[3], "D",  8, 3, 4);
    item_new(its[4], "E", 11, 4, 3);
    item_new(its[5], "F",  6, 1, 2);
    item_new(its[6], "G",  9, 9, 5);
    item_new(its[7], "H",  4, 3, 1);

    /* Run algorithm */
    bool success = knapsack(c);
    if(!success) {
        printf("ERROR: Knapsack algorithm was unable to complete... exiting.\n");
        return(-2);
    }

    /* Show tables */
    printf("-----------------------------------\n");
    matrix_print(c->table_values);
    printf("-----------------------------------\n");
    matrix_print(c->table_items);

    /* Generate report */
    bool report_created = knapsack_report(c);
    if(!report_created) {
        printf("ERROR: Report could not be created.\n");
    } else {
        printf("Report created at reports/knapsack.tex\n");

        int as_pdf = latex2pdf("knapsack", "reports");
        if(as_pdf == 0) {
            printf("PDF version available at reports/knapsack.pdf\n");
        } else {
            printf("ERROR: Unable to convert report to PDF. Status: %i.\n",
                   as_pdf);
        }
    }

    /* Free resources */
    knapsack_context_free(c);
    return(0);
}
