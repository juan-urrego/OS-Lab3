#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/time.h>
#include <pthread.h>
#include "../tools/validations.c"s
#include "../tools/minorValue.c"

//Estrucutra para pasar los argumentos a los hilos para normalizar una matriz
typedef struct {
    int start_row;
    int end_row;
    Matrix* M;
    Vector* vrz;
    Vector* std;
    pthread_mutex_t* mutex;
} Args_matrix_t;


void calculate_normalize_formula2(int rows, int cols, int n, int file);
void calculate_formula2_without_parallel(Matrix* M, Vector* vrz, Vector* std);
void calculate_formula2_with_parallel(Matrix* M, Vector* vrz, Vector* std, int n);
void* parallel_formula2_method(void* args);


void calculate_normalize_formula2(int rows, int cols, int n, int file) {

    Matrix* M=NULL; 
    if(file==1){
        M = create_matrix_from_file("op1.txt",rows, cols);
    }
    else {
        M = create_matrix(rows, cols);
        init_matrix_rand(M);
    }
    print_matrix(M);
    printf("La media de las columnas es: \n");
    Vector* vrz = matrix_col_vrz(M);
    print_vector(vrz);
    printf("La desviacion estandar de las columnas es: \n");
    Vector* std = matrix_col_std(M);
    print_vector(std);
    Matrix* M1 = create_matrix(rows, cols);
    copy_matrix(M1,M);
    calculate_formula2_without_parallel(M1, vrz,std);
    Matrix* M2 = create_matrix(rows, cols);
    copy_matrix(M2,M);
    calculate_formula2_with_parallel(M2, vrz,std,n);
    free_matrix(M);
}


void calculate_formula2_without_parallel(Matrix* M, Vector* vrz, Vector* std) {
    printf("\nCalculo de normalizacion con formula 2 SIN paralelismo\n");

    struct timeval start, end;
    gettimeofday(&start, 0);

    normalize_matrix_column_formula_2(M, vrz,std);

    gettimeofday(&end, 0);
    double elapsed_time = ((double)(end.tv_usec - start.tv_usec) /1000000 + (double)(end.tv_sec - start.tv_sec));
    printf("Tiempo de ejecución: %f microsegundos\n", elapsed_time);

    print_matrix(M);
    free_matrix(M);
}

// Función para normalizar una matriz con paralelismo
void calculate_formula2_with_parallel(Matrix* M, Vector* vrz, Vector* std, int n) {
    printf("\nCalculo de normalizacion con formula 2 SIN paralelismo\n");

    struct timeval start, end;
    gettimeofday(&start, 0);

    int num_threads = minor_value(n, M->rows);
    pthread_t threads[num_threads];
    Args_matrix_t thread_args[num_threads];
    pthread_mutex_t mutex;
    pthread_mutex_init(&mutex, NULL);
    int chunk_size = M->rows / num_threads;
    int extra_rows = M->rows % num_threads;
    int start_row = 0;
    for (int i = 0; i < num_threads; ++i) {
        int rows = chunk_size + (i < extra_rows);
        thread_args[i].start_row = start_row;
        thread_args[i].end_row = start_row + rows;
        thread_args[i].M = M;
        thread_args[i].vrz = vrz;
        thread_args[i].std = std;
        thread_args[i].mutex = &mutex;
        start_row += rows;
        pthread_create(&threads[i], NULL, parallel_formula2_method, &thread_args[i]);
    }
    for (int i = 0; i < num_threads; ++i) {
        pthread_join(threads[i], NULL);
    }
    pthread_mutex_destroy(&mutex);

    gettimeofday(&end, 0);
    double elapsed_time = ((double)(end.tv_usec - start.tv_usec) /1000000 + (double)(end.tv_sec - start.tv_sec));
    printf("Tiempo de ejecución: %f microsegundos\n", elapsed_time);
    
    print_matrix(M);
    free_matrix(M);
}

//Función para normalizar una matriz usando paralelismo
void* parallel_formula2_method(void* args) {
    Args_matrix_t* thread_args = (Args_matrix_t*) args;
    for (int i = thread_args->start_row; i < thread_args->end_row; ++i) {
        for (int j = 0; j < thread_args->M->cols; ++j) {
            pthread_mutex_lock(thread_args->mutex);
            thread_args->M->elements[i][j] = (thread_args->M->elements[i][j] - thread_args->vrz->elements[j]) / thread_args->std->elements[j];
            pthread_mutex_unlock(thread_args->mutex);
        }
    }
    pthread_exit(NULL);
}