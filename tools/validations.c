#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#ifndef VALIDATIONS_H
#define VALIDATIONS_H
//función que valida que los datos de entrada sean correctos
void validate_data_operation_with_one_matrix(int rows, int columns, int n){
    //validar que las filas y columnas sean mayores a 0
    if(rows <= 0 || columns <= 0){
        printf("The number of rows and columns must be greater than 0\n");
        exit(EXIT_FAILURE);
    }
    //validar que el número de hilos sea mayor a 0
    if(n <= 0){
        printf("Number of threads must be greater than 0\n");
        exit(EXIT_FAILURE);
    }
}
//Función que valida que haya más de una fila en la matriz
void validate_data_operation_with_one_matrix_rows(int rows){
    if(rows <= 1){
        printf("The number of rows must be greater than 1\n");
        exit(EXIT_FAILURE);
    }
}


void validate_data_operation_with_two_matrices(int m1_rows, int m1_cols, int m2_rows, int m2_cols, int n){
    //validar que las filas y columnas sean mayores a 0
    if(m1_rows <= 0 || m1_cols <= 0 || m2_rows <= 0 || m2_cols <= 0){
        printf("The number of rows and columns must be greater than 0\n");
        exit(EXIT_FAILURE);
    }
    //validar que el número de hilos sea mayor a 0
    if(n <= 0){
        printf("Number of threads must be greater than 0\n");
        exit(EXIT_FAILURE);
    }
}

#endif