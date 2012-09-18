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

int main(int argc, char **argv)
{
    printf("Testing Optimal binary search tree...");

    /* Create context */
    optbst_context* c = optbst_context_new(4);
    if(c == NULL) {
        printf("Unable to create Optimal Binary Search Tree's context... exiting.\n");
        return(-1);
    }

    matrix* a = c->table_a;
    matrix* r = c->table_r;
    float* p = c->keys_probabilities;

    /*Set probabilities */
    p[0] = 0.18;
    p[1] = 0.32;
    p[2] = 0.39;
    p[3] = 0.11;

    /* Run algorithm */
    bool success = optbst(c);
    if(!success) {
        printf("Optimal Binary Search Tree's algorithm was unable to complete... exiting.\n");
        return(-2);
    }



    /* Show table s*/
    printf("-----------------------------------\n");
    matrix_print(a);

    printf("-----------------------------------\n");
    matrix_print(r);

    /* Free resources */
    optbst_context_free(c);
    
    return(0);
}

