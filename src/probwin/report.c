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

bool probwin_report(probwin_context* c)
{
    /* Create report file */
    FILE* report = fopen("reports/probwin.tex", "w");
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
    fprintf(report, "\\section{%s}\n\n", "Probabilities to win");
    fprintf(report, "\\noindent{\\huge %s.} \\\\[0.4cm]\n",
                    "Dynamic programming");
    fprintf(report, "{\\LARGE %s.}\\\\[0.4cm]\n", "Operation Research");
    fprintf(report, "\\HRule \\\\[0.5cm]\n");

    /* Write description */
    fprintf(report, "\\indent ");
    success = insert_file("latex/probwin.txt", report);
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

    /* Write teams */
    fprintf(report, "\\subsection{%s}\n", "Teams");
    fprintf(report, "\\begin{multicols}{3}\n");
    fprintf(report, "A: {\\Large %s} \\\\ \n\\indent{}", c->a_name);
    fprintf(report, "B: {\\Large %s}\n", c->b_name);
    fprintf(report, "\n");
    fprintf(report, "P\\subscript{h}: {\\Large %1.4f} \\\\ \n\\indent{}", c->ph);
    fprintf(report, "Q\\subscript{h}: {\\Large %1.4f}\n", 1.0 - c->ph);
    fprintf(report, "\n");
    fprintf(report, "P\\subscript{r}: {\\Large %1.4f} \\\\ \n\\indent{}", c->pr);
    fprintf(report, "Q\\subscript{r}: {\\Large %1.4f}\n", 1.0 - c->pr);
    fprintf(report, "\n");
    fprintf(report, "\\end{multicols}\n");
    fprintf(report, "\n");

    //fprintf(report, "\\subsection{%s}\n", "Games");
    //fprintf(report, "\\hspace{1cm}%s: {\\Large \\textbf{%i}}\n",
                    //"Number of games", c->games);
    //fprintf(report, "\n");

    fprintf(report, "\\subsection{%s}\n", "Games");
    probwin_format(c, report);
    fprintf(report, "\\newpage\n");
    fprintf(report, "\n");

    /* TOC */
    fprintf(report, "\\tableofcontents\n\\newpage\n");
    fprintf(report, "\n");

    /* Write execution */
    fprintf(report, "\\subsection{%s}\n", "Execution");
    probwin_table(c, report);
    fprintf(report, "\\newpage\n");
    fprintf(report, "\n");

    /* Write analisis */
    fprintf(report, "\\subsection{%s}\n", "Analisis");
    probwin_analysis(c, report);
    fprintf(report, "\n");


    /* Write digest */
    float prob = c->table_w->data[(c->games + 1) / 2][(c->games + 1) / 2];
    fprintf(report, "\\subsection{%s}\n", "Digest");
    fprintf(report, "At the beginning of the series, {\\Large %s} has a "
                    "probability to win of {\\Large %0.4f}, and therefore "
                    "{\\Large %s} has a probability of {\\Large %0.4f}",
                    c->a_name, prob, c->b_name, 1 - prob);
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

void probwin_format(probwin_context* c, FILE* stream)
{
    /* Game number  A plays at home */

    /* Table preamble */
    fprintf(stream, "\\begin{table}[!ht]\n");
    fprintf(stream, "\\centering\n");
    fprintf(stream, "\\begin{tabular}{|c|c|}\n");
    fprintf(stream, "\\hline\n");

    /* Table headers */
    fprintf(stream, "\\cellcolor{gray90}\\textbf{%s} &"
                    "\\cellcolor{gray90}\\textbf{%s %s}",
                    "Game number", c->a_name, "plays at home");
    fprintf(stream, " \\\\ \n\\hline\\hline\n");

    /* Table body */
    for(int i = 0; i < c->games; i++) {
        if(c->game_format[i]) {
            fprintf(stream, "%i & %s \\\\ \\hline\n", i + 1, "Yes");
        } else {
            fprintf(stream, "%i & %s \\\\ \\hline\n", i + 1, "No");
        }
    }
    fprintf(stream, "\\end{tabular}\n");

    /* Caption */
    fprintf(stream, "\\caption{%s.}\n", "Game format");
    fprintf(stream, "\\end{table}\n");
    fprintf(stream, "\n");
}

void probwin_table(probwin_context* c, FILE* stream)
{
    matrix* m = c->table_w;

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
                        "{\\cellcolor{gray90}\\textbf{%i}} & ", i);
        for(int j = 0; j < m->columns; j++) {

            if(!(i == 0 && j == 0)) {
                fprintf(stream, "%1.4f", m->data[i][j]);
            }

            if(j < m->columns - 1) {
                fprintf(stream, " & ");
            }
        }
        fprintf(stream, " \\\\ \\hline\n");
    }
    fprintf(stream, "\\end{tabular}\n");

    fprintf(stream, "\\caption{%s.}\n", "Table W");
    fprintf(stream, "\\end{adjustwidth}\n");
    fprintf(stream, "\\end{table}\n");
    fprintf(stream, "\n");
}


void probwin_analysis(probwin_context* c, FILE* stream){

    matrix* m = c->table_w;

    for(int i = 0; i < m->rows; i++){
        for(int j = 0; j< m->columns; j++){
            if(i != 0 && j != 0){
                int current_game = c->games + 1 - i - j;
                float prob = m->data[ i ][ j ];
                
                fprintf(stream, " -  {\\Large Playing game %i :}\n %s  needs to win %i games to become champion with a probability to win of  %0.4f,\n"
                "and therefore %s needs to win %i games to become champion with a probability to win of %0.4f\n",
                current_game, c->a_name,  i,  prob,  c->b_name, j, 1 - prob);
                fprintf(stream, "\n\n\n");                    
            }
        }
    }
    

}
