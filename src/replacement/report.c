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
    replacement_table(c, report);
    fprintf(report, "\\newpage\n");
    fprintf(report, "\n");

    /* Write analisis */
    fprintf(report, "\\subsection{%s}\n", "Analisis");
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

void replacement_table(replacement_context* c, FILE* stream)
{
    float* mc = c->minimum_cost;
    for(int i = 0; i <= c->years_plan; i++) {
           fprintf(stream, "G(%d): %4.2f \n", i, mc[i]);
           fprintf(stream, "\n");
    }
    fprintf(stream, "\n");
}

void replacement_data(replacement_context* c, FILE* stream)
{
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
}
