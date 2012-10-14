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

bool replacement_report(replacement_context* c)
{
    /* Create report file */
    FILE* report = fopen("reports/replacement.tex", "w");
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
    fprintf(report, "\\section{%s}\n\n", "Equipment Replacement");

    fprintf(report, "\\noindent{\\huge %s.} \\\\[0.4cm]\n",
                    "Dynamic programming");
    fprintf(report, "{\\LARGE %s.}\\\\[0.4cm]\n", "Operation Research");
    fprintf(report, "\\HRule \\\\[0.5cm]\n");

    /* Write description */
    fprintf(report, "\\indent ");
    success = insert_file("latex/replacement.txt", report);
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

    /* Write data */
    fprintf(report, "\\subsection{%s}\n", "Data");
    replacement_data(c, report);
    fprintf(report, "\\newpage\n");


    /* TOC */
    fprintf(report, "\\newpage\n\\tableofcontents\n\\newpage\n");
    fprintf(report, "\n");

     /* Write execution */
    fprintf(report, "\\subsection{%s}\n", "Execution");
    replacement_table(c, report, c->table_c, true,
                      "Cost to purchase the equipment at the"
                      "instant X and sell it at the instant Y");
    replacement_table(c, report, c->table_p, false,
                      "Replacement plans");
    replacement_mincost(c, report);
    fprintf(report, "\\newpage\n");
    fprintf(report, "\n");

    /* Write analisis */
    fprintf(report, "\\subsection{%s}\n", "Analysis");
    replacement_costs(c, report);
    replacement_path(c, report);

    /* Write digest */
    fprintf(report, "\\subsection{%s}\n", "Digest");
    replacement_digest(c, report);

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

void replacement_table(replacement_context* c, FILE* stream, matrix* m,
                       bool is_c, char* msj) {

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
        fprintf(stream, "\\cellcolor{gray90}\\textbf{%i}", j + 1);
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

            if(j >= i) {
                if(is_c) {
                    fprintf(stream, "%.4f", m->data[i][j]);
                } else {
                    fprintf(stream, "%.0f", m->data[i][j]);
                }
            }

            if(j < m->columns - 1) {
                fprintf(stream, " & ");
            }
        }
        fprintf(stream, " \\\\ \\hline\n");
    }
    fprintf(stream, "\\end{tabular}\n");

    fprintf(stream, "\\caption{%s.}\n", msj);
    fprintf(stream, "\\end{adjustwidth}\n");
    fprintf(stream, "\\end{table}\n");
    fprintf(stream, "\n");

}

void replacement_data(replacement_context* c, FILE* stream)
{
    /* Common data */
    fprintf(stream, "\\begin{compactitem}\n");

    fprintf(stream, "\\item %s : \\textsc{%s}. \n",
                    "Equipment", c->equipment);
    fprintf(stream, "\\item %s : \\textsc{%.2f}. \n",
                    "Equipment cost", c->equipment_cost);
    fprintf(stream, "\\item %s : \\textsc{%i %s}. \n",
                    "Equipment Lifetime", c->lifetime, "years");
    fprintf(stream, "\\item %s : \\textsc{%i %s}. \n",
                    "Replacement plan for", c->years_plan, "years");

    fprintf(stream, "\\end{compactitem}\n");
    fprintf(stream, "\n");

    /* Maintenance and sale costs */
    /* Table preamble and headers */
    fprintf(stream, "\\begin{table}[!ht]\n");
    fprintf(stream, "\\centering\n");
    fprintf(stream, "\\begin{tabular}{|c|c|c|}\n\\hline\n");
    fprintf(stream, "\\cellcolor{gray90}\\textbf{%s} & ", "Year");
    fprintf(stream, "\\cellcolor{gray90}\\textbf{%s} & ", "Maintenance");
    fprintf(stream, "\\cellcolor{gray90}\\textbf{%s} "  , "Sale");
    fprintf(stream, "\\\\\n\\hline\\hline\n");
    /* Table body */
    for(int i = 0; i < c->lifetime; i++) {
        fprintf(stream, "%i & %.4f & %.4f \\\\ \\hline\n",
                        i + 1, c->maintenance_cost[i], c->sale_cost[i]);
    }
    /* Table closure */
    fprintf(stream, "\\end{tabular}\n");
    fprintf(stream, "\\caption{%s.}\n", "Maintenance and sale costs");
    fprintf(stream, "\\end{table}\n");
    fprintf(stream, "\n");
}

void replacement_mincost(replacement_context* c, FILE* stream){

    float* m = c->minimum_cost;
    int size = c->years_plan + 1;

    /* Table preamble */
    fprintf(stream, "\\begin{table}[!ht]\n");
    fprintf(stream, "\\begin{adjustwidth}{-3cm}{-3cm}\n");
    fprintf(stream, "\\centering\n");
    fprintf(stream, "\\begin{tabular}{|");
    for(int cl = 0; cl < size; cl++) {
        fprintf(stream, "c|");
    }
    fprintf(stream, "}\n\\hline\n");

    /* Table headers */
    for(int j = 0; j < size; j++) {
        fprintf(stream, "\\cellcolor{gray90}\\textbf{%i}", j);
        if(j < size - 1) {
            fprintf(stream, " & ");
        }
    }
    fprintf(stream, " \\\\\n\\hline\\hline\n");

    /* Table body */
    for(int j = 0; j < size; j++) {

        fprintf(stream, "%.4f", m[j]);
        if(j < size - 1) {
            fprintf(stream, " & ");
        }
    }
    fprintf(stream, " \\\\ \\hline\n");
    fprintf(stream, "\\end{tabular}\n");

    fprintf(stream, "\\caption{%s.}\n", "Table with the minimum costs");
    fprintf(stream, "\\end{adjustwidth}\n");
    fprintf(stream, "\\end{table}\n");
    fprintf(stream, "\n");
}

void replacement_costs(replacement_context* c, FILE* stream)
{
    float* mc = c->minimum_cost;
    fprintf(stream, "\\noindent{%s:}\n",
                    "Optimal minimum costs \\textsc{G(t)}");
    fprintf(stream, "\n");
    fprintf(stream, "\\begin{compactitem}\n");
    for(int i = 0; i <= c->years_plan; i++) {
        fprintf(stream, "\\item G(%i): \\textsc{%4.2f}.\n", i, mc[i]);
    }
    fprintf(stream, "\\end{compactitem}\n");
    fprintf(stream, "\n");
}

void replacement_path(replacement_context* c, FILE* stream)
{
    fprintf(stream, "\\noindent{%s:}\n", "Optimal replacement plans");
    fprintf(stream, "\\begin{compactitem}\n");
    /* Allocate array for paths */
    int size = c->table_p->rows + 2;
    int* path = (int*) malloc(size * sizeof(int));
    if(path != NULL) {
        /* Initialize array */
        path[0] = 0;
        for(int i = 1; i < size; i++) {
            path[i] = -1;
        }
        /* Find and print paths */
        find_path(c->table_p, 0, path, 1, stream);
        free(path);
    } else {
        fprintf(stream, "\\item %s", "No memory available for path.");
    }
    fprintf(stream, "\\end{compactitem}\n");
}

void find_path(matrix* m, int i, int* path, int c, FILE* stream)
{
    bool is_end = true;
    for(int j = i; j < m->columns; j++) {
        int p = (int) m->data[i][j];
        if(p != 0) {
            is_end = false;
            /* Append current to path */
            path[c] = p;
            /* Continue path search */
            find_path(m, p, path, c + 1, stream);
            /* Clear this branch path */
            for(int k = c + 1; k <= m->rows; k++) {
                path[k] = -1;
            }
        }
    }

    /* Flush route if is end */
    if(is_end) {
        /* Count jumps */
        int jumps = 0;
        for(; path[jumps] != -1; jumps++) {}
        /* Print plan */
        fprintf(stream, "\\item {\\Large \\textsc{");
        for(int k = 0; k < jumps; k++) {
            fprintf(stream, "%i", path[k]);
            if(k < jumps - 1) {
                fprintf(stream, " $\\longrightarrow$ ");
            }
        }
        fprintf(stream, "}}\n");
    }
}

void replacement_digest(replacement_context* c, FILE* stream)
{
    fprintf(stream, "The minimum cost for the whole project "
                    "(%i years) is {\\Large %.4f}.\n",
                    c->years_plan,
                    c->minimum_cost[0]);
    fprintf(stream, "After the initial buy, you may replace your equipment "
                    "at years ");
    for(int i = 0; i < c->table_p->rows;) {
        for(int j = i; i < c->table_p->columns; j++) {
            int p = (int) c->table_p->data[i][j];
            if(p != 0) {
                i = p;
                fprintf(stream, "%i", p);
                break;
            }
        }
        if(i < c->table_p->rows) {
            fprintf(stream, ", ");
        }
    }
    fprintf(stream, "; or using any other of the plans listed in the analysis "
                    "section. Please note that by using any other replacement "
                    "plan not listed there guarantees to have a greater total "
                    "cost.");
    fprintf(stream, "\n");
}
