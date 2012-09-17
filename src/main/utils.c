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

#include "utils.h"

bool file_exists(char *fname)
{
    FILE* file = fopen(fname, "r");

    if(file != NULL) {
        fclose(file);
        return true;
    }
    return false;
}

char* get_current_time()
{
    GDateTime* now = g_date_time_new_now_local();
    /* 31/12/2012 03:00AM */
    char* formatted = g_date_time_format(now, "%d/%m/%Y %I:%M%p");
    g_date_time_unref(now);
    return formatted;
}

bool fequal(float a, float b)
{
    return fabs(a-b) < F_EPSILON;
}

bool copy_streams(FILE* input, FILE* output)
{
    /* Rewind input stream so we can start reading from the beginning */
    rewind(input);

    /* Copy streams */
    char ch;
    while(!feof(input)) {

        ch = fgetc(input);

        if(ferror(input)) {
          return false;
        }
        if(!feof(input)) {
            fputc(ch, output);
        }
        if(ferror(output)) {
            return false;
        }
    }
    return true;
}

bool insert_file(char* filename, FILE* output)
{
    FILE* input = fopen(filename, "rt");
    if(input == NULL) {
        return false;
    }

    bool success = copy_streams(input, output);
    if(!success) {
        return false;
    }

    if(fclose(input) == EOF) {
        return false;
    }

    return true;
}
