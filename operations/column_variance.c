#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/time.h>
#include <pthread.h>
#include "../tools/execution_time.c"
#include "../tools/matrix.c"
#include "../tools/validations.c"

typedef struct {
    Vector *R;
    Matrix *M;
    int start_row;
    int end_row;
    pthread_mutex_t *mutex;
} MatricesVarianceArgs;

void columns_variance(Matrix *M) {
    printf("\nVariance of matrices without paralellism\n");
    struct timeval start, end;
    gettimeofday(&start, 0);
    Vector *R = matrix_col_vrz(M);
    gettimeofday(&end, 0);
    get_execution_time(start, end);
    print_vector(R);
    free_vector(R);
}

void *column_variance_parallel(void *arg) {
    MatricesVarianceArgs *args = (MatricesVarianceArgs *)arg;
    Vector *mean = matrix_col_mean(args->M);
    for (int i = args->start_row; i < args->end_row; ++i) {
        double sum = 0.0;
        for (int j = 0; j < args->M->rows; ++j) {
            sum += pow(args->M->elements[j][i] - mean->elements[i], 2);
        }
        pthread_mutex_lock(args->mutex);
        args->R->elements[i] = sum / args->M->rows;
        pthread_mutex_unlock(args->mutex);
    }
    return NULL;
}

void columns_variance_parallel(Matrix *M, int n) {
    printf("\nVariance of matrices with paralellism\n");
    struct timeval start, end;
    const int num_threads = n;
    Vector *R = create_vector(M->cols);
    pthread_t threads[num_threads];
    MatricesVarianceArgs thread_args[num_threads];
    pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
    gettimeofday(&start, 0);
    const int chunk_size = M->cols / num_threads;
    int start_row = 0;
    for (int i = 0; i < num_threads; ++i) {
        int end_row = start_row + chunk_size;
        if (i == num_threads - 1) {
            end_row = M->rows;
        }
        thread_args[i] = (MatricesVarianceArgs){.R = R, .M = M, .start_row = start_row, .end_row = end_row, .mutex = &mutex};
        pthread_create(&threads[i], NULL, column_variance_parallel, &thread_args[i]);
        start_row = end_row;
    }
    for (int i = 0; i < num_threads; ++i) {
        pthread_join(threads[i], NULL);
    }
    pthread_mutex_destroy(&mutex);
    gettimeofday(&end, 0);
    get_execution_time(start, end);
    print_vector(R);
    free_vector(R);
}

void matrix_columns_variance(int m_rows, int m_cols, int n, int file) {   
    validate_data_operation_with_one_matrix(m_rows, m_cols, n);
    validate_data_operation_with_one_matrix_rows(m_rows);
    Matrix *M = NULL;
    if (file == 1) {
        M = create_matrix_from_file("op1.txt", m_rows, m_cols);
    }
    else {
        M = create_matrix(m_rows, m_cols);
        init_matrix_rand(M);
    }
    print_matrix(M);
    columns_variance(M);
    columns_variance_parallel(M, n);
    free_matrix(M);
};