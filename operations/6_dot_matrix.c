#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/time.h>
#include <pthread.h>
#include "../tools/matrix.c"
#include "../tools/validations.c"
#include "../tools/minorValue.c"


// Estructura de datos para pasar los argumentos a la funcion que realiza el producto punto entre dos matrices usando paralelismo
typedef struct {
    Matrix *R;
    Matrix *M;
    Matrix *N;
    int start_row;
    int end_row;
    pthread_mutex_t *mutex;
} MatricesDotArgs;


void calculate_dot_two_matrix(int m1_rows, int m1_cols, int m2_rows, int m2_cols, int n, int file);
void calculate_dot_without_parallel(Matrix *M, Matrix *N);
void calculate_dot_with_parallel(Matrix *M, Matrix *N, int n);
void *parallel_dot_method(void *arg);


void calculate_dot_two_matrix(int m1_rows, int m1_cols, int m2_rows, int m2_cols, int n, int file) {
    validate_two_matrices_operation(m1_rows, m1_cols, m2_rows, m2_cols, n);
    Matrix *M = NULL, *N = NULL;
    if (file == 1) {
        M = create_matrix_from_file("op1.txt", m1_rows, m1_cols);
        N = create_matrix_from_file("op2.txt", m2_rows, m2_cols);
    }
    else {
        M = create_matrix(m1_rows, m1_cols);
        init_matrix_rand(M);
        N = create_matrix(m2_rows, m2_cols);
        init_matrix_rand(N);
    }
    print_matrix(M);
    print_matrix(N);
    calculate_dot_without_parallel(M, N);
    calculate_dot_with_parallel(M, N, n);
    free_matrix(M);
    free_matrix(N);
}

void calculate_dot_without_parallel(Matrix *M, Matrix *N) {
    printf("\nCalculo de producto punto de matrices SIN paralelismo\n");

    struct timeval start, end;
    gettimeofday(&start, 0);

    Matrix *R = dot_matrix(M, N);

    gettimeofday(&end, 0);
    double elapsed_time = ((double)(end.tv_usec - start.tv_usec) /1000000 + (double)(end.tv_sec - start.tv_sec));
    printf("Tiempo de ejecución: %f microsegundos\n", elapsed_time);

    print_matrix(R);
    free_matrix(R);
}

void calculate_dot_with_parallel(Matrix *M, Matrix *N, int n)
{
    printf("\nCalculo de producto punto de matrices CON paralelismo\n");

    struct timeval start, end;
    gettimeofday(&start, 0);

    int num_threads = minor_value(n, M->rows);
    Matrix *R = create_matrix(M->rows, N->cols);
    pthread_t threads[num_threads];
    MatricesDotArgs thread_args[num_threads];
    pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
    const int chunk_size = M->rows / num_threads;
    int start_row = 0;
    for (int i = 0; i < num_threads; ++i) {
        int end_row = start_row + chunk_size;
        if (i == num_threads - 1) {
            end_row = M->rows;
        }
        thread_args[i] = (MatricesDotArgs){.R = R, .M = M, .N = N, .start_row = start_row, .end_row = end_row, .mutex = &mutex};
        pthread_create(&threads[i], NULL, parallel_dot_method, &thread_args[i]);
        start_row = end_row;
    }

    for (int i = 0; i < num_threads; ++i) {
        pthread_join(threads[i], NULL);
    }
    pthread_mutex_destroy(&mutex);

    gettimeofday(&end, 0);
    double elapsed_time = ((double)(end.tv_usec - start.tv_usec) /1000000 + (double)(end.tv_sec - start.tv_sec));
    printf("Tiempo de ejecución: %fms\n", elapsed_time);
    
    print_matrix(R);
    free_matrix(R);
}


void *parallel_dot_method(void *arg) {
    MatricesDotArgs *args = (MatricesDotArgs *)arg;
    if (args->M->cols != args->N->rows) {
        fprintf(stderr, "Invalid size. (%d, %d) and (%d, %d)\n", args->M->rows, args->M->cols, args->N->rows, args->N->cols);
        return NULL;
    }
    for (int i = args->start_row; i < args->end_row; ++i) {
        for (int j = 0; j < args->N->cols; ++j) {
            double d = 0.0;
            for (int k = 0; k < args->M->cols; ++k) {
                pthread_mutex_lock(args->mutex);
                d += args->M->elements[i][k] * args->N->elements[k][j];
                pthread_mutex_unlock(args->mutex);
            }
            args->R->elements[i][j] = d;
        }
    }
    return NULL;
}

