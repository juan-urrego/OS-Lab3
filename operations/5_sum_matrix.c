#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/time.h>
#include <pthread.h>
#include "../tools/matrix.c"
#include "../tools/validations.c"
#include "../tools/minorValue.c"


typedef struct {
    Matrix *R;
    Matrix *M;
    Matrix *N;
    int start_row;
    int end_row;
    pthread_mutex_t *mutex;
} MatricesSumArgs;

void calculate_sum_two_matrix(int m1_rows, int m1_cols, int m2_rows, int m2_cols, int n, int file);
void calculate_sum_without_parallel(Matrix *M, Matrix *N);
void calculate_sum_with_parallel(Matrix *M, Matrix *N, int n);
void *parallel_sum_method(void *arg);


void calculate_sum_two_matrix(int m1_rows, int m1_cols, int m2_rows, int m2_cols, int n, int file) {
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
    calculate_sum_without_parallel(M, N);
    calculate_sum_with_parallel(M, N, n);
    free_matrix(M);
    free_matrix(N);
}

void calculate_sum_without_parallel(Matrix *M, Matrix *N) {
    printf("\nCalculo de suma de matrices SIN paralelismo\n");
    struct timeval start, end;
    gettimeofday(&start, 0);
    Matrix *R = add_matrix(M, N);
    gettimeofday(&end, 0);
    double elapsed_time = ((double)(end.tv_usec - start.tv_usec) /1000000 + (double)(end.tv_sec - start.tv_sec));
     printf("Tiempo de ejecución: %f microsegundos\n", elapsed_time);;
    print_matrix(R);
    free_matrix(R);
}

void calculate_sum_with_parallel(Matrix *M, Matrix *N, int n) {
    printf("\nCalculo de suma de matrices CON paralelismo\n");
    struct timeval start, end;
    int num_threads = minor_value(n, M->rows);
    Matrix *R = create_matrix(M->rows, M->cols);
    pthread_t threads[num_threads];
    MatricesSumArgs thread_args[num_threads];
    pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

    gettimeofday(&start, 0);
    const int chunk_size = M->rows / num_threads;
    int start_row = 0;
    for (int i = 0; i < num_threads; ++i) {
        int end_row = start_row + chunk_size;
        if (i == num_threads - 1) {
            end_row = M->rows;
        }
        thread_args[i] = (MatricesSumArgs){.R = R, .M = M, .N = N, .start_row = start_row, .end_row = end_row, .mutex = &mutex};
        pthread_create(&threads[i], NULL, parallel_sum_method, &thread_args[i]);
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


void *parallel_sum_method(void *arg) {
    MatricesSumArgs *args = (MatricesSumArgs *)arg;
    if (args->M->rows != args->N->rows || args->M->cols != args->N->cols) {
        fprintf(stderr, "Invalid size. (%d, %d) and (%d, %d)\n", args->M->rows, args->M->cols, args->N->rows, args->N->cols);
        return NULL;
    }
    for (int i = args->start_row; i < args->end_row; ++i) {
        for (int j = 0; j < args->M->cols; ++j) {
            pthread_mutex_lock(args->mutex);
            args->R->elements[i][j] = args->M->elements[i][j] + args->N->elements[i][j];
            pthread_mutex_unlock(args->mutex);
        }
    }
    return NULL;
}
