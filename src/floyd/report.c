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

#include "report.h"

bool floyd_report(floyd_context* c)
{
    /* Create report file */
    FILE* report = fopen("reports/floyd.tex", "w");
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
    fprintf(report, "\\section{%s}\n\n", "Floyd's algorithm");
    fprintf(report, "\\noindent{\\huge %s.} \\\\[0.4cm]\n",
                    "Dynamic programming");
    fprintf(report, "{\\LARGE %s.}\\\\[0.4cm]\n", "Operation Research");
    fprintf(report, "\\HRule \\\\[0.5cm]\n");

    /* Write description */
    fprintf(report, "\\indent ");
    success = insert_file("latex/floyd.txt", report);
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

    /* Write graph */
    fprintf(report, "\\subsection{%s}\n", "Input graph");
    if(file_exists("reports/graph.pdf")) {
        fprintf(report, "\\begin{figure}[H]\\centering\n");
        fprintf(report, "\\noindent\\includegraphics[height=210px]"
                        "{reports/graph.pdf}\n");
        fprintf(report, "\\caption{%s.}\n\\end{figure}\n",
                        "Floyd's input directed graph system");
    } else {
        fprintf(report, "ERROR: Graph image could not be generated.\n");
    }
    fprintf(report, "\n");

    /* TOC */
    fprintf(report, "\\newpage\n\\tableofcontents\n\\newpage\n");
    fprintf(report, "\n");

    /* Write execution */
    fprintf(report, "\\subsection{%s}\n", "Execution");
    success = copy_streams(c->report_buffer, report);
    if(!success) {
        return false;
    }
    fprintf(report, "\n");

    /* Write analisis */
    int bumpier = -1;
    int startb, endb;

    float heavier = -1.0;
    int starth, endh;

    fprintf(report, "\\subsection{%s}\n", "Analisis");
    int counter = 0;
    for(int starti = 0; starti < c->nodes; starti++) {
        for(int endi = 0; endi < c->nodes; endi++) {

            /* Ignore same node paths */
            if(starti == endi) {
                continue;
            }

            counter++;
            if(counter % 6 == 0) {
                fprintf(report, "\\clearpage\n");
            }

            /* Preamble */
            fprintf(report, "\\subsubsection{%s: %s $\\longrightarrow$ %s}\n",
                            "Analisis for path",
                            c->names[starti], c->names[endi]);
            fprintf(report, "\\begin{compactitem}\n");
            fprintf(report, "\\item %s : {\\Large %s \\subscript{(%i)}}"
                            "$\\longrightarrow$ ", "Optimal path",
                            c->names[starti], starti + 1);

            /* Follow route */
            int jumps = 1;
            int next = (int)c->table_p->data[starti][endi];
            while(next != 0) {
                fprintf(report, "%s \\subscript{(%i)} $\\longrightarrow$ ",
                                c->names[next - 1], next);
                next = (int)c->table_p->data[next - 1][endi];
                jumps++;
            }

            /* Write subreport */
            fprintf(report, "%s \\subscript{(%i)}.\n",
                            c->names[endi], endi + 1);
            fprintf(report, "\\item %s : {\\Large %i}.\n",
                            "Total jumps", jumps);
            float distance = c->table_d->data[starti][endi];
            if(distance == FLT_MAX) {
                fprintf(report, "\\item %s : {\\Large $\\infty$}.\n",
                                "Total distance");
            } else {
                if(floorf(distance) == distance) {
                    fprintf(report, "\\item %s : {\\Large %.0f}.\n",
                                    "Total distance", distance);
                } else {
                    fprintf(report, "\\item %s : {\\Large %.4f}.\n",
                                    "Total distance", distance);
                }
            }
            fprintf(report, "\\end{compactitem}\n");
            fprintf(report, "\n");

            /* Compare this iteration */
            if(jumps > bumpier) {
                bumpier = jumps;
                startb = starti;
                endb = endi;
            }
            if(distance > heavier) {
                heavier = distance;
                starth = starti;
                endh = endi;
            }
        }
    }
    fprintf(report, "\n");

    /* Digest */
    fprintf(report, "\\subsection{%s}\n", "Digest");
    fprintf(report, "\\begin{compactitem}\n");
    if(bumpier != -1) {
        fprintf(report, "\\item %s : {\\Large %s \\subscript{(%i)} "
                        "$\\longrightarrow$ %s \\subscript{(%i)}} "
                        "with %i jumps.\n",
                        "Bumpier path",
                        c->names[startb], startb + 1,
                        c->names[endb], endb + 1,
                        bumpier);
    }
    if(heavier != -1.0) {
        char* template = "\\item %s : {\\Large %s \\subscript{(%i)} "
                         "$\\longrightarrow$ %s \\subscript{(%i)}} with a "
                         "total distance of {\\Large %.4f}.\n";
        if(floorf(heavier) == heavier) {
            template = "\\item %s : {\\Large %s \\subscript{(%i)} "
                       "$\\longrightarrow$ %s \\subscript{(%i)}} with a "
                       "total distance of {\\Large %.0f}.\n";
        } else if(heavier == PLUS_INF) {
            template = "\\item %s : {\\Large %s \\subscript{(%i)} "
                       "$\\longrightarrow$ %s \\subscript{(%i)}} with a "
                       "total distance of {\\Large $\\infty$}.\n";
        }
        fprintf(report, template, "Longest path",
                        c->names[starth], starth + 1,
                        c->names[endh], endh + 1,
                        heavier);
    }
    fprintf(report, "\\end{compactitem}\n");
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

void floyd_execution(floyd_context* c, int k)
{
    FILE* stream = c->report_buffer;
    fprintf(stream, "\\subsubsection{%s %i}\n", "Iteration", k);
    floyd_table(c->table_d, true, k, stream);
    floyd_table(c->table_p, false, k, stream);
    fprintf(stream, "\\clearpage\n");
}

void floyd_table(matrix* m, bool d, int k, FILE* stream)
{
    /* Table preamble */
    fprintf(stream, "\n");
    fprintf(stream, "\\begin{table}[!ht]\n");
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
                        "{\\cellcolor{gray90}\\textbf{%i}} & ", i + 1);
        for(int j = 0; j < m->columns; j++) {

            float cell = m->data[i][j];
            if(cell == FLT_MAX) {
                fprintf(stream, "$\\infty$");
            } else {
                if(d && !(ceilf(cell) == cell)) {
                    fprintf(stream, "%.2f", cell);
                } else {
                    fprintf(stream, "%.0f", cell);
                }
            }

            if(j < m->columns - 1) {
                fprintf(stream, " & ");
            }
        }
        fprintf(stream, " \\\\ \\hline\n");
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

void floyd_graph(floyd_context* c)
{

    /* Create graph file */
    FILE* graph = fopen("reports/graph.gv", "w");
    if(graph == NULL) {
        return;
    }

    /* Preamble */
    fprintf(graph, "digraph floyd {\n\n");
    fprintf(graph, "    rankdir = LR;\n");
    fprintf(graph, "    node [shape = circle];\n\n");

    /* Labels */
    for(int i = 0; i < c->table_d->rows; i++) {
        char* name = c->names[i];
        fprintf(graph, "    %i [label = \"%s\"];\n", i + 1, name);
    }
    fprintf(graph, "\n");

    /* Vertices */
    for(int i = 0; i < c->table_d->rows; i++) {
        for(int j = 0; j < c->table_d->columns; j++) {
            float weight = c->table_d->data[i][j];
            if((weight != PLUS_INF) && (weight != 0.0)) {
                if(ceilf(weight) == weight) {
                    fprintf(graph, "    %i -> %i [label = \"%.0f\"];\n",
                            i + 1, j + 1, weight);
                } else {
                    fprintf(graph, "    %i -> %i [label = \"%.2f\"];\n",
                            i + 1, j + 1, weight);
                }
            }
        }
        fprintf(graph, "\n");
    }

    fprintf(graph, "}\n");

    /* Close file */
    fclose(graph);

    /* Render graph */
    gv2pdf("graph", "reports");
}
