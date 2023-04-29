#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#ifndef VALIDATIONS_H
#define VALIDATIONS_H

/*
 * Función que valida los parámetros de entrada para una operación con una matriz.
 * Las filas y columnas deben ser mayores que cero y el número de hilos debe ser mayor que cero.
 */
void validate_data_operation_with_one_matrix(int rows, int columns, int n){
    if(rows <= 0 || columns <= 0){
        printf("El número de filas y columnas debe ser mayor que 0\n");
        exit(EXIT_FAILURE);
    }
    if(n <= 0){
        printf("El número de hilos debe ser mayor que 0\n");
        exit(EXIT_FAILURE);
    }
}
/*
 * Función que valida que la matriz tenga más de una fila.
 * Si la matriz tiene una sola fila, no se puede realizar la operación.
 */
void validate_data_operation_with_one_matrix_rows(int rows){
    if(rows <= 1){
        printf("El número de filas debe ser mayor que 1\n");
        exit(EXIT_FAILURE);
    }
}

/*
 * Función que valida los parámetros de entrada para una operación con dos matrices.
 * Las filas y columnas de ambas matrices deben ser mayores que cero y el número de hilos debe ser mayor que cero.
 */
void validate_data_operation_with_two_matrices(int m1_rows, int m1_cols, int m2_rows, int m2_cols, int n){
    if(m1_rows <= 0 || m1_cols <= 0 || m2_rows <= 0 || m2_cols <= 0){
        printf("El número de filas y columnas debe ser mayor que 0\n");
        exit(EXIT_FAILURE);
    }
    if(n <= 0){
        printf("El número de hilos debe ser mayor que 0\n");
        exit(EXIT_FAILURE);
    }
}

#endif