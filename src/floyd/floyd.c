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
    c->table_d = matrix_new(nodes, nodes, 0.0);
    if(c->table_d == NULL) {
        return NULL;
    }
    c->table_p = matrix_new(nodes, nodes, 0.0);
    if(c->table_p == NULL) {
        matrix_free(c->table_d)
        return NULL;
    }

    /* Initialize values */
    c->status = -1;
    c->execution_time = 0;
    c->memory_required = 0;
    c->report_buffer = tmpfile();

    return c;
}

void floyd_context_free(floyd_context* c)
{
    matrix_free(c->table_d)
    matrix_free(c->table_p)
    fclose(c->report_buffer);
    free(c)
    return;
}

bool floyd(floyd_context *c)
{
    return true;
}
