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

bool optbst_report(optbst_context* c)
{
    /* Create report file */
    FILE* report = fopen("reports/optbst.tex", "w");
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
    fprintf(report, "\\section{%s}\n\n", "Optimal Binary Search Trees");
    fprintf(report, "\\noindent{\\huge %s.} \\\\[0.4cm]\n",
                    "Dynamic programming");
    fprintf(report, "{\\LARGE %s.}\\\\[0.4cm]\n", "Operation Research");
    fprintf(report, "\\HRule \\\\[0.5cm]\n");

    /* Write description */
    fprintf(report, "\\indent ");
    success = insert_file("latex/optbst.txt", report);
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

    /* Write nodes */
    fprintf(report, "\\subsection{%s}\n", "Nodes");
    optbst_nodes(c, report);
    fprintf(report, "\\newpage\n");

    /* TOC */
    fprintf(report, "\\newpage\n\\tableofcontents\n\\newpage\n");
    fprintf(report, "\n");

    /* Write tables */
    fprintf(report, "\\subsection{%s}\n", "Tables");
    optbst_execution(c, report);
    fprintf(report, "\n");

    /* Write graphic */
    int levels = optbst_graph(c);
    fprintf(report, "\\subsection{%s}\n", "Analisis");
    if(file_exists("reports/tree.pdf")) {
        fprintf(report, "\\begin{figure}[H]\\centering\n");
        fprintf(report, "\\noindent\\includegraphics"
                        "[height=500px, width=400px, keepaspectratio]"
                        "{reports/tree.pdf}\n");
        fprintf(report, "\\caption{%s.}\n\\end{figure}\n",
                        "Optimal search tree");
    } else {
        fprintf(report, "ERROR: Tree image could not be generated.\n");
    }
    fprintf(report, "\\newpage\n\n");

    /* Digest */
    fprintf(report, "\\subsection{%s}\n", "Digest");
    fprintf(report, "\\begin{compactitem}\n");
    fprintf(report, "\\item %s : {\\Large %i}.\n", "Total nodes", c->keys);
    fprintf(report, "\\item %s : {\\Large %i}.\n", "Levels", levels);
    fprintf(report, "\\item %s : {\\Large %.2f}.\n", "Expected cost",
                    c->table_a->data[0][c->keys]);
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

int find_nodes(matrix* r, int i, int j, FILE* stream)
{
    int levels = max(find_rnodes(r, i, j, stream, 1),
                     find_lnodes(r, i, j, stream, 1));
    return levels;
}

int find_lnodes(matrix* r, int i, int j, FILE* stream, int level)
{
    /* printf("Finding left nodes at (%i, %i).\n", i, j); */

    /* Current node, search node */
    int cn = (int)r->data[i][j];
    int sn;

    /* Search node */
    while(j >= 0) {
        sn = (int)r->data[i][j];
        if(cn != sn) {
            break;
        }
        j--;
    }

    /* No childs */
    if(sn == 0) {
        /* printf("None found.\n"); */
        return level;
    }

    /* Write node found and continue */
    fprintf(stream, "    %i -> %i;\n", cn, sn);
    return max(find_rnodes(r, i, j, stream, level + 1),
               find_lnodes(r, i, j, stream, level + 1));
}

int find_rnodes(matrix* r, int i, int j, FILE* stream, int level)
{
    /* printf("Finding right nodes at (%i, %i).\n", i, j); */

    /* Current node, search node */
    int cn = (int)r->data[i][j];
    i = cn;
    int sn = (int)r->data[i][j];

    /* No childs */
    if(sn == 0) {
        /* printf("None found.\n"); */
        return level;
    }

    /* Write node found and continue */
    fprintf(stream, "    %i -> %i;\n", cn, sn);
    return max(find_rnodes(r, i, j, stream, level + 1),
               find_lnodes(r, i, j, stream, level + 1));
}

int optbst_graph(optbst_context* c)
{
    /* Create tree file */
    FILE* tree = fopen("reports/tree.gv", "w");
    if(tree == NULL) {
        return -1;
    }

    /* Preamble */
    fprintf(tree, "digraph bstree {\n\n");
    fprintf(tree, "    node [shape = circle];\n");
    fprintf(tree, "    graph [ordering=\"out\"];\n\n");

    /* Labels */
    for(int i = 0; i < c->keys; i++) {
        char* name = c->names[i];
        fprintf(tree, "    %i [label = \"%s\"];\n", i + 1, name);
    }
    fprintf(tree, "\n");

    /* Vertices */
    int i = 0;
    int j = c->table_r->columns - 1;
    int levels = find_nodes(c->table_r, i, j, tree);
    fprintf(tree, "\n");
    fprintf(tree, "}\n");

    /* Close file */
    fclose(tree);

    /* Render graph */
    gv2pdf("tree", "reports");

    return levels;
}

void optbst_nodes(optbst_context* c, FILE* stream)
{
    /* Id Name Probabilities */

    /* Table preamble */
    fprintf(stream, "\\begin{table}[!ht]\n");
    fprintf(stream, "\\centering\n");
    fprintf(stream, "\\begin{tabular}{c||c|c|");
    fprintf(stream, "}\n\\cline{2-3}\n");

    /* Table headers */
    fprintf(stream, " & \\cellcolor{gray90}\\textbf{%s}"
                    " & \\cellcolor{gray90}\\textbf{%s} ",
                    "Name", "Probabilities");
    fprintf(stream, " \\\\\n\\hline\\hline\n");

    /* Table body */
    for(int i = 0; i < c->keys; i++) {
        fprintf(stream, "\\multicolumn{1}{|c||}"
                        "{\\cellcolor{gray90}\\textbf{%i}} & ", i + 1);
        fprintf(stream, "%s & %.2f \\\\ \\hline\n",
                        c->names[i], c->keys_probabilities[i]);
    }
    fprintf(stream, "\\end{tabular}\n");

    /* Caption */
    fprintf(stream, "\\caption{%s.}\n", "Nodes probabilities");
    fprintf(stream, "\\end{table}\n");
    fprintf(stream, "\n");
}

void optbst_execution(optbst_context* c, FILE* stream) {
    optbst_table(c->table_a, true, stream);
    optbst_table(c->table_r, false, stream);
}

void optbst_table(matrix* m, bool a, FILE* stream)
{
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
        fprintf(stream, "\\cellcolor{gray90}\\textbf{%i}", j);
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
            if(i <= j) {
                if(a) {
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

    /* Caption */
    if(a) {
        fprintf(stream, "\\caption{%s.}\n", "Table A");
    } else {
        fprintf(stream, "\\caption{%s.}\n", "Table R");
    }
    fprintf(stream, "\\end{adjustwidth}\n");
    fprintf(stream, "\\end{table}\n");
    fprintf(stream, "\n");
}
