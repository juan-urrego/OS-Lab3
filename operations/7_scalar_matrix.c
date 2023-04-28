#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/time.h>
#include <pthread.h>
#include "../tools/validations.c"
#include "../tools/minorValue.c"


//Estructura de datos para pasar los argumentos a la funcion que realiza la multiplicacion de la matriz por un escalar usando paralelismo
typedef struct {
    Matrix* M;
    double k;
    int start_row;
    int end_row;
    pthread_mutex_t* mutex;
} ScalarMatrixArgs;


void calculate_matrix_by_scalar(int rows, int cols, double scalar, int n,int file);
void calculate_scalar_without_parallel(Matrix* M,double scalar);
void calculate_scalar_with_parallel(Matrix* M,double scalar, int n);
void* parallel_scalar_method(void* arg);


void calculate_matrix_by_scalar(int rows, int cols, double scalar, int n,int file){
    validate_data_operation_with_one_matrix(rows, cols, n);
    Matrix* M=NULL;
    if(file==1) {
        M=create_matrix_from_file("op1.txt",rows,cols);
    }else {
        M = create_matrix(rows, cols);
        init_matrix_rand(M);
    }
    printf("La matrix es la siguiente: \n");
    print_matrix(M);
    printf("El escalar es: %f \n",scalar);
    Matrix* M1 = create_matrix(rows, cols);
    copy_matrix(M1,M);
    calculate_scalar_without_parallel(M1,scalar);
    Matrix* M2 = create_matrix(rows, cols);
    copy_matrix(M2,M);
    calculate_scalar_with_parallel(M2,scalar,n);
    free_matrix(M);
    
}

//Funcion para realizar la multiplicacion de la matriz por un escalar sin usar paralelismo
void calculate_scalar_without_parallel(Matrix* M,double scalar){
    printf("\nCalculo de matrix con escalar SIN paralelismo\n");

    struct timeval start, end;
    gettimeofday(&start, 0);

    scalar_matrix(M,scalar);

    gettimeofday(&end, 0);
    double elapsed_time = ((double)(end.tv_usec - start.tv_usec) /1000000 + (double)(end.tv_sec - start.tv_sec));
    printf("Tiempo de ejecución: %fms\n", elapsed_time);

    print_matrix(M);
    free_matrix(M);
}

//Funcion para realizar la multiplicacion de la matriz por un escalar usando paralelismo
void calculate_scalar_with_parallel(Matrix* M,double scalar, int n) {
    printf("\nCalculo de matrix con escalar CON paralelismo\n");

    struct timeval start, end;
    gettimeofday(&start, 0);

    const int num_threads =minor_value(n,M->rows);
    pthread_t threads[num_threads];
    ScalarMatrixArgs thread_args[num_threads];
    pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER; 
    const int chunk_size = M->rows / num_threads;
    int start_row = 0;
    for (int i = 0; i < num_threads; ++i) {
        int end_row = start_row + chunk_size;
        if (i == num_threads - 1) {
            end_row = M->rows;
        }
        thread_args[i] = (ScalarMatrixArgs) { .M = M, .k = scalar, .start_row = start_row, .end_row = end_row, .mutex = &mutex };
        pthread_create(&threads[i], NULL, parallel_scalar_method, &thread_args[i]);
        start_row = end_row;
    }
    for (int i = 0; i < num_threads; ++i) {
        pthread_join(threads[i], NULL);
    }
    pthread_mutex_destroy(&mutex);

    gettimeofday(&end, 0);
    double elapsed_time = ((double)(end.tv_usec - start.tv_usec) /1000000 + (double)(end.tv_sec - start.tv_sec));
    printf("Tiempo de ejecución: %fms\n", elapsed_time);
    
    print_matrix(M);
    free_matrix(M);
}

//Funcion que realiza la multiplicacion de la matriz por un escalar usando paralelismo
void* parallel_scalar_method(void* arg) {
    ScalarMatrixArgs* args = (ScalarMatrixArgs*) arg;
    for (int i = args->start_row; i < args->end_row; ++i) {
        for (int j = 0; j < args->M->cols; ++j) {
            pthread_mutex_lock(args->mutex);
            args->M->elements[i][j] *= args->k;
            pthread_mutex_unlock(args->mutex);
        }
    }
    return NULL;
}