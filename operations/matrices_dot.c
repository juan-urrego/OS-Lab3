#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/time.h>
#include <pthread.h>
#include "../tools/execution_time.c"
#include "../tools/matrix.c"
#include "../tools/validations.c"
#include "../tools/minorValue.c"

void dot_two_matrices(int m1_rows, int m1_cols, int m2_rows, int m2_cols, int n, int file);
void dot_matrices_without_paralellism(Matrix *M, Matrix *N);
void dot_matrices_with_paralellism(Matrix *M, Matrix *N, int n);

// Estructura de datos para pasar los argumentos a la funcion que realiza el producto punto entre dos matrices usando paralelismo
typedef struct {
    Matrix *R;
    Matrix *M;
    Matrix *N;
    int start_row;
    int end_row;
    pthread_mutex_t *mutex;
} MatricesDotArgs;

// Funcion que realiza el producto punto de las matrices usando paralelismo
void *dot_matrices_parallel(void *arg) {
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

void dot_two_matrices(int m1_rows, int m1_cols, int m2_rows, int m2_cols, int n, int file) {
    validate_data_operation_with_two_matrices(m1_rows, m1_cols, m2_rows, m2_cols, n);
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
    dot_matrices_without_paralellism(M, N);
    dot_matrices_with_paralellism(M, N, n);
    free_matrix(M);
    free_matrix(N);
}

void dot_matrices_without_paralellism(Matrix *M, Matrix *N)
{
    printf("\nDot of matrices without paralellism\n");
    struct timeval start, end;
    gettimeofday(&start, 0);
    Matrix *R = dot_matrix(M, N);
    gettimeofday(&end, 0);
    get_execution_time(start, end);
    print_matrix(R);
    free_matrix(R);
}

void dot_matrices_with_paralellism(Matrix *M, Matrix *N, int n)
{
    printf("\nDot of matrices with paralellism\n");
    struct timeval start, end;
    int num_threads = minor_value(n, M->rows);
    Matrix *R = create_matrix(M->rows, N->cols);
    pthread_t threads[num_threads];
    MatricesDotArgs thread_args[num_threads];
    pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

    gettimeofday(&start, 0);
    const int chunk_size = M->rows / num_threads;
    int start_row = 0;
    for (int i = 0; i < num_threads; ++i) {
        int end_row = start_row + chunk_size;
        if (i == num_threads - 1) {
            end_row = M->rows;
        }
        thread_args[i] = (MatricesDotArgs){.R = R, .M = M, .N = N, .start_row = start_row, .end_row = end_row, .mutex = &mutex};
        pthread_create(&threads[i], NULL, dot_matrices_parallel, &thread_args[i]);
        start_row = end_row;
    }

    for (int i = 0; i < num_threads; ++i) {
        pthread_join(threads[i], NULL);
    }
    pthread_mutex_destroy(&mutex);
    gettimeofday(&end, 0);
    get_execution_time(start, end);
    print_matrix(R);
    free_matrix(R);
}
