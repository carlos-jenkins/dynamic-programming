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

#include "floyd.h"

floyd_context* floyd_context_new(int nodes)
{
    /* Check input is correct */
    if(nodes < 2) {
        return NULL;
    }

    /* Allocate structure */
    floyd_context* c = (floyd_context*) malloc(sizeof(floyd_context));
    if(c == NULL) {
        return NULL;
    }

    /* Try to allocate matrices */
    c->table_d = matrix_new(nodes, nodes, PLUS_INF);
    if(c->table_d == NULL) {
        return NULL;
    }
    c->table_p = matrix_new(nodes, nodes, 0.0);
    if(c->table_p == NULL) {
        matrix_free(c->table_d);
        return NULL;
    }

    /* Initialize values */
    for(int i = 0; i < nodes; i++) {
        c->table_d->data[i][i] = 0.0;
    }
    c->status = -1;
    c->execution_time = 0;
    c->memory_required = (matrix_sizeof(c->table_d) * 2) +
                                            sizeof(floyd_context);
    c->report_buffer = tmpfile();
    if(c->report_buffer == NULL) {
        matrix_free(c->table_d);
        matrix_free(c->table_p);
        return NULL;
    }

    return c;
}

void floyd_context_free(floyd_context* c)
{
    matrix_free(c->table_d);
    matrix_free(c->table_p);
    fclose(c->report_buffer);
    free(c);
    return;
}

bool floyd(floyd_context *c)
{
    /* Start counting time */
    GTimer* timer = g_timer_new();

    /* Run the Floyd Warshall algorithm */
    matrix* d = c->table_d;
    matrix* p = c->table_p;
    int nodes = d->rows;

    for(int k = 0; k < nodes; k++) {
        for(int i = 0; i < nodes; i++) {
            for(int j = 0; j < nodes; j++) {
                float minimum = fminf(d->data[i][j],
                                      d->data[i][k] + d->data[k][j]);
                if(minimum < d->data[i][j]) {
                    p->data[i][j] = k + 1;
                    d->data[i][j] = minimum;
                }
            }
        }
        /* Log execution */
        floyd_execution(c, k);
    }

    /* Stop counting time */
    g_timer_stop(timer);
    c->execution_time = g_timer_elapsed(timer, NULL);
    return true;
}

void floyd_table(matrix* m, bool d, int k, FILE* stream)
{
    fprintf(stream, "\n");
    fprintf(stream, "\\begin{table}[!h]\n");
    fprintf(stream, "\\centering\n");
    fprintf(stream, "\\begin{tabular}{|c||");
    for(int c = 0; c < m->columns; c++) {
        fprintf(stream, "c|");
    }
    fprintf(stream, "}\n\\hline\n");

    for(int i = 0; i < m->rows; i++) {
        fprintf(stream, " & ");
        for(int j = 0; j < m->columns; j++) {

            float cell = m->data[i][j];
            if(cell == FLT_MAX) {
                fprintf(stream, "$\\infty$");
            } else {
                if(d) {
                    fprintf(stream, "%4.2f", cell);
                } else {
                    fprintf(stream, "%4.0f", cell);
                }
            }

            if(j == m->columns - 1) {
                fprintf(stream, " \\\\ \\hline\n");
                if(i == 0) {
                    fprintf(stream, "\\hline \n");
                }
            } else {
                fprintf(stream, " & ");
            }
        }
    }

    fprintf(stream, "\\end{tabular}\n");
    if(d) {
        fprintf(stream, "\\caption{%s %i.}\n", "D table at iteration", k);
    } else {
        fprintf(stream, "\\caption{%s %i.}\n", "P table at iteration", k);
    }
    fprintf(stream, "\\end{table}\n");
    fprintf(stream, "\n");
}

void floyd_execution(floyd_context* c, int k)
{
    FILE* stream = c->report_buffer;
    fprintf(stream, "\\subsubsection{%s %i}\n", "Iteration", k);
    floyd_table(c->table_d, true, k, stream);
    floyd_table(c->table_p, false, k, stream);
}

bool floyd_report(floyd_context* c)
{
    /* Create report file */
    FILE* report = fopen("reports/floyd.tex", "w");
    if(report == NULL) {
        return false;
    }

    /* Write header */
    bool success = insert_file("latex/header.tex", report);
    if(!success) {
        return false;
    }

    /* Write first section */
    fprintf(report, "\n");
    fprintf(report, "\\section{%s}\n", "Floyd's algorithm");
    fprintf(report, "\\begin{compactitem}\n");
    fprintf(report, "\\item %s : \\textsc{%s}. \n",
                    "Executed on", get_current_time());
    fprintf(report, "\\item %s : \\textsc{%lf %s}. \n",
                    "Execution time", c->execution_time,
                    "seconds");
    fprintf(report, "\\item %s : \\textsc{%i %s}. \n",
                    "Memory required", c->memory_required,
                    "bytes");
    fprintf(report, "\\end{compactitem}\n");
    fprintf(report, "\n");

    /* Write nodes */
    fprintf(report, "\n");
    fprintf(report, "\\subsection{%s}\n", "Graph");
    fprintf(report, "TODO");
    fprintf(report, "\n");

    /* Write execution */
    fprintf(report, "\n");
    fprintf(report, "\\subsection{%s}\n", "Execution");
    success = copy_streams(c->report_buffer, report);
    if(!success) {
        return false;
    }

    /* Write interpretation */
    fprintf(report, "\n");
    fprintf(report, "\\subsection{%s}\n", "Interpretation");
    fprintf(report, "TODO");
    fprintf(report, "\n");

    /* End document */
    fprintf(report, "\n");
    fprintf(report, "\\end{document}");

    /* Save & swap buffers */
    int success_file = fflush(report);
    if(success_file == EOF) {
        return false;
    }
    success_file = fclose(c->report_buffer);
    if(success_file == EOF) {
        return false;
    }
    c->report_buffer = report;

    return true;
}
