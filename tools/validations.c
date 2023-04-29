#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#ifndef VALIDATIONS_H
#define VALIDATIONS_H

void error_message(char *message){
    printf("%s\n", message);
    exit(EXIT_FAILURE);
}

//Se validan los datos de entrada para la operación de suma
void validate_one_matrix_operation(int rows_quantity, int columns_quantity, int threads_quantity){
    //Se validan que las filas y columnas sean mayores a 0
    if(rows_quantity <= 0 || columns_quantity <= 0){
        error_message("The quantity of both rows and columns must exceed 0.");
    }
    // Se validan que el número de hilos sea mayor a 0
    if(threads_quantity <= 0){
        error_message("The amount of threads should be higher than zero.");
    }
}

// Se validan los datos de entrada para la operación de suma
void validate_matrix_operation_by_rows(int rows_quantity){
    if(rows_quantity <= 1){
        error_message("The rows quantity must be greater than 1.");
    }
}

void validate_two_matrices_operation(int matrix1_rows, int matrix1_cols, int matrix2_rows, int m2_cols, int threads_quantity){
    //validar que las filas y columnas sean mayores a 0
    if(matrix1_rows <= 0 || matrix1_cols <= 0 || matrix2_rows <= 0 || m2_cols <= 0){
        error_message("The quantity of both rows and columns must exceed 0.");
    }
    //validar que el número de hilos sea mayor a 0
    if(threads_quantity <= 0){
        error_message("The amount of threads should be higher than zero.");
    }
}

#endif