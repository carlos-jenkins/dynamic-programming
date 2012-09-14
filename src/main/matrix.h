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

#ifndef H_MATRIX
#define H_MATRIX

#include <stdlib.h>
#include <stdio.h>

#define MATRIX_DATATYPE int

typedef struct {
        int rows;
        int columns;
        MATRIX_DATATYPE **data;
} matrix;

/**
 * Fill a matrix with the value given.
 *
 * @param matrix, a matrix structure (by reference)
 * @return nothing
 */
void matrix_fill(matrix *m, MATRIX_DATATYPE value);

/**
 * Print a matrix to the standard output.
 *
 * @param matrix, a matrix structure (by reference)
 * @return nothing
 */
void matrix_print(matrix *m);

/**
 * Create a matrix of given size.
 *
 * @param rows, the number of rows
 * @param columns, the number of columns
 * @param value, value to initialize the matrix.
 * @return a pointer to the matrix structure or NULL if enough memory could
 *         not be allocated.
 */
matrix* matrix_new(int rows, int columns, MATRIX_DATATYPE fill);

/**
 * Free resources associated with a matrix.
 *
 * @return nothing
 * @param matrix, a matrix structure (by reference)
 */
void matrix_free(matrix *m);

#endif
