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

#include "optbst.h"
#include "latex.h"

int main(int argc, char **argv)
{
    printf("Testing Optimal binary search tree...\n\n");

    /* Create context */
    optbst_context* c = optbst_context_new(9);
    if(c == NULL) {
        printf("Unable to create Optimal Binary Search Tree's "
               "context... exiting.\n");
        return(-1);
    }

    float* p = c->keys_probabilities;
    char** n = c->names;

    /* Set probabilities */
    p[0] = 0.05;
    p[1] = 0.07;
    p[2] = 0.01;
    p[3] = 0.35;
    p[4] = 0.09;
    p[5] = 0.23;
    p[6] = 0.15;
    p[7] = 0.04;
    p[8] = 0.01;

    /* Set names */
    n[0] = "Carpenter";
    n[1] = "Cash";
    n[2] = "Cobain";
    n[3] = "Harrison";
    n[4] = "Hendrix";
    n[5] = "Joplin";
    n[6] = "Lennon";
    n[7] = "Morrison";
    n[8] = "Presley";

    /* Run algorithm */
    bool success = optbst(c);
    if(!success) {
        printf("Optimal Binary Search Tree's algorithm was "
               "unable to complete... exiting.\n");
        return(-2);
    }

    /* Show tables */
    matrix* a = c->table_a;
    matrix* r = c->table_r;
    printf("-----------------------------------\n");
    matrix_print(a);

    printf("-----------------------------------\n");
    matrix_print(r);

    /* Generate report */
    bool report_created = optbst_report(c);
    if(!report_created) {
        printf("ERROR: Report could not be created.\n");
    } else {
        printf("Report created at reports/optbst.tex\n");

        int as_pdf = latex2pdf("optbst", "reports");
        if(as_pdf == 0) {
            printf("PDF version available at reports/optbst.pdf\n");
        } else {
            printf("ERROR: Unable to convert report to PDF. Status: %i.\n",
                   as_pdf);
        }
    }

    /* Free resources */
    optbst_context_free(c);

    return(0);
}

