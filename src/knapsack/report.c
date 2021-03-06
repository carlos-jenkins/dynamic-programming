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

#include "report.h"

bool knapsack_report(knapsack_context* c)
{
    /* Create report file */
    FILE* report = fopen("reports/knapsack.tex", "w");
    if(report == NULL) {
        return false;
    }

    /* Write preamble */
    bool success = insert_file("latex/header.tex", report);
    if(!success) {
        return false;
    }
    fprintf(report, "\n");

    /* Write header */
    fprintf(report, "\\section{%s}\n\n", "Bounded Knapsack");
    fprintf(report, "\\noindent{\\huge %s.} \\\\[0.4cm]\n",
                    "Dynamic programming");
    fprintf(report, "{\\LARGE %s.}\\\\[0.4cm]\n", "Operation Research");
    fprintf(report, "\\HRule \\\\[0.5cm]\n");

    /* Write description */
    fprintf(report, "\\indent ");
    success = insert_file("latex/knapsack.txt", report);
    if(!success) {
        return false;
    }

    /* Write first section */
    fprintf(report, "\\\\[0.5cm]\n\\noindent{\\Large Details:}\n");
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

    /* Write items */
    fprintf(report, "\\subsection{%s}\n", "Items");
    knapsack_items(c, report);
    fprintf(report, "\\newpage\n");
    fprintf(report, "\n");

    /* TOC */
    fprintf(report, "\\tableofcontents\n\\newpage\n");
    fprintf(report, "\n");

    /* Write execution */
    fprintf(report, "\\subsection{%s}\n", "Execution");
    knapsack_table(c, report);
    fprintf(report, "\\newpage\n");
    fprintf(report, "\n");

    /* Write analisis */
    fprintf(report, "\\subsection{%s}\n", "Analisis");
    fprintf(report, "\\begin{compactitem}\n");

    int capacity_left = c->capacity;
    int total_items = 0;
    matrix* ti = c->table_items;

    for(int at_item = ti->columns - 1; at_item > -1; at_item--) {

        int put_items = (int)ti->data[capacity_left][at_item];
        if(put_items == 0) {
            continue;
        }
        item* citem = c->items[at_item];
        int capacity_took = (int)ceilf((float)put_items * citem->weight);
        float value_added = put_items * citem->value;

        fprintf(report, "\\item %s {\\Large %s} : %s {\\Large %i}. \n",
                        "Item", citem->name, "put", put_items);

        fprintf(report, "    \\begin{compactitem}\n");
        if(floorf(value_added) == value_added) {
            fprintf(report, "    \\item %s : {\\Large %.0f}. \n",
                            "Accumulated value", value_added);
        } else {
            fprintf(report, "    \\item %s : {\\Large %.2f}. \n",
                            "Accumulated value", value_added);
        }
        fprintf(report, "    \\item %s : {\\Large %i}. \n",
                        "Accumulated weight", capacity_took);
        fprintf(report, "    \\end{compactitem}\n");

        fprintf(report, "\n");

        capacity_left -= capacity_took;
        total_items += put_items;
    }
    fprintf(report, "\\end{compactitem}\n");
    fprintf(report, "\n");

    /* Write digest */
    matrix* tv = c->table_values;
    fprintf(report, "\\subsection{%s}\n", "Digest");
    fprintf(report, "\\begin{compactitem}\n");
    fprintf(report, "    \\item %s : {\\Large %i}. \n",
                    "Total accumulated value",
                    (int)tv->data[tv->rows - 1][tv->columns - 1]);
    fprintf(report, "    \\item %s : {\\Large %i/%i}. \n",
                    "Total accumulated weight",
                    c->capacity - capacity_left, c->capacity);
    fprintf(report, "    \\item %s : {\\Large %i}. \n",
                    "Total items", total_items);
    fprintf(report, "\\end{compactitem}\n");
    fprintf(report, "\n");

    /* End document */
    fprintf(report, "\\end{document}\n");
    fprintf(report, "\n");

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

void knapsack_items(knapsack_context* c, FILE* stream)
{
    /* Table preamble */
    fprintf(stream, "\\begin{table}[!ht]\n");
    fprintf(stream, "\\begin{adjustwidth}{-3cm}{-3cm}\n");
    fprintf(stream, "\\centering\n");
    fprintf(stream, "\\begin{tabular}{c||c|c|c|c|");
    fprintf(stream, "}\n\\cline{2-5}\n");

    /* Table headers */
    fprintf(stream, " & \\cellcolor{gray90}\\textbf{%s}"
                    " & \\cellcolor{gray90}\\textbf{%s}"
                    " & \\cellcolor{gray90}\\textbf{%s}"
                    " & \\cellcolor{gray90}\\textbf{%s} \\\\\n",
                    "Name", "Value", "Weight", "Amount");
    fprintf(stream, "\\hline\\hline\n");

    /* Table body */
    for(int i = 0; i < c->num_items; i++) {
        fprintf(stream, "\\multicolumn{1}{|c||}"
                        "{\\cellcolor{gray90}\\textbf{%i}} & ", i + 1);
        fprintf(stream, "%s & ", c->items[i]->name);

        for(int j = 0; j < 3; j++) {
            float value;
            if(j == 0) {
                value = c->items[i]->value;
            } else if(j == 1) {
                value = c->items[i]->weight;
            } else {
                value = c->items[i]->amount;
            }

            if(value == FLT_MAX) {
                fprintf(stream, "$\\infty$");
            } else {
                if(ceilf(value) == value) {
                    fprintf(stream, "%.0f", value);
                } else {
                    fprintf(stream, "%.2f", value);
                }
            }

            if(j < 2) {
                fprintf(stream, " & ");
            }
        }
        fprintf(stream, " \\\\ \\hline\n");
    }

    fprintf(stream, "\\end{tabular}\n");
    fprintf(stream, "\\caption{%s.}\n", "Input items table");
    fprintf(stream, "\\end{adjustwidth}\n");
    fprintf(stream, "\\end{table}\n");
    fprintf(stream, "\n");
}

void knapsack_table(knapsack_context* c, FILE* stream)
{
    matrix* m = c->table_values;

    /* Table preamble */
    fprintf(stream, "\\begin{table}[!ht]\n");
    fprintf(stream, "\\begin{adjustwidth}{-3cm}{-3cm}\n");
    fprintf(stream, "\\centering\n");
    fprintf(stream, "\\begin{tabular}{c||");
    for(int cl = 0; cl < m->columns; cl++) {
        fprintf(stream, "c|");
    }
    fprintf(stream, "}\n\\cline{2-%i}\n", m->columns + 1);

    /* Table headers */
    fprintf(stream, " & ");
    for(int j = 0; j < m->columns; j++) {
        /* FIXME : Do not hardwire int output, consider PLUS_INF and float */
        fprintf(stream,
            "\\cellcolor{gray90}\\textbf{%s} \\subscript{(%i, %i, %i)}",
            c->items[j]->name,
            (int)c->items[j]->value,
            (int)c->items[j]->weight,
            (int)c->items[j]->amount);
        if(j < m->columns - 1) {
            fprintf(stream, " & ");
        }
    }
    fprintf(stream, " \\\\\n\\hline\\hline\n");

    /* Table body */
    for(int i = 0; i < m->rows; i++) {
        fprintf(stream, "\\multicolumn{1}{|c||}"
                        "{\\cellcolor{gray90}\\textbf{%i}} & ", i);
        for(int j = 0; j < m->columns; j++) {

            float cell = m->data[i][j];
            float amount = c->table_items->data[i][j];

            /* Value */
            if(cell == FLT_MAX) {
                fprintf(stream, "$\\infty$");
            } else {
                if(ceilf(cell) == cell) {
                    fprintf(stream, "%.0f", cell);
                } else {
                    fprintf(stream, "%.2f", cell);
                }
            }

            /* Amount */
            if(amount == 0.0) {
                fprintf(stream, "\\textcolor{deepred}{");
            } else {
                fprintf(stream, "\\textcolor{deepgreen}{");
            }
            fprintf(stream, "\\subscript{(");
            if(amount == FLT_MAX) {
                fprintf(stream, "$\\infty$");
            } else {
                if(ceilf(amount) == amount) {
                    fprintf(stream, "%.0f", amount);
                } else {
                    fprintf(stream, "%.2f", amount);
                }
            }
            fprintf(stream, ")}}");

            if(j < m->columns - 1) {
                fprintf(stream, " & ");
            }
        }
        fprintf(stream, " \\\\ \\hline\n");
    }

    fprintf(stream, "\\end{tabular}\n");
    fprintf(stream, "\\caption{%s.}\n", "Final knapsack table");
    fprintf(stream, "\\end{adjustwidth}\n");
    fprintf(stream, "\\end{table}\n");
    fprintf(stream, "\n");


    fprintf(stream, "\\textbf{%s}\n", "Legend");
    fprintf(stream, "\\begin{compactitem}\n");
    fprintf(stream, "\\item %s A (x, y, z) : \\textsc{%s}. \n",
                    "Items", "Name (Value, Weight, Amount)");
    fprintf(stream, "\\item %s x (y) : \\textsc{%s}. \n",
                    "Cell", "Max value (items put)");
    fprintf(stream, "\\end{compactitem}\n");
    fprintf(stream, "\n");

}
