#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/time.h>
#include <pthread.h>
#include "../tools/matrix.c"
#include "../tools/validations.c"

typedef struct {
    Vector *R;
    Matrix *M;
    int start_row;
    int end_row;
    pthread_mutex_t *mutex;
} MatricesStandardDeviationArgs;

void calculate_matrix_columns_standard_deviation(int m_rows, int m_cols, int n, int file);
void* computeStd(void* arg);
void findStdThreaded(Matrix* M);
void findStd(Matrix* M);


void calculate_matrix_columns_standard_deviation(int m_rows, int m_cols, int n, int file) {
    validate_data_operation_with_one_matrix(m_rows, m_cols, n);
    Matrix *M = NULL;
    if (file == 1) {
        M = create_matrix_from_file("op1.txt", m_rows, m_cols);
    }
    else {
        M = create_matrix(m_rows, m_cols);
        init_matrix_rand(M);
    }
    print_matrix(M);
    findStd(M); //Encontrar la desviacion estandar sin usar hilos
    findStdThreaded(M); //Encontrar la desviacion estandar usando hilos
}

void findStd(Matrix* M) {
    Vector R;
    R.size = M->cols;
    R.elements = (double*) malloc(R.size * sizeof(double));
    clock_t start_time = clock();
    for (int j = 0; j < M->cols; j++) {
        double sum = 0.0, sumsq = 0.0;
        for (int i = 0; i < M->rows; i++) {
            double val = M->elements[i][j];
            sum += val;
            sumsq += val*val;
        }
        double mean = sum / M->rows;
        double stddev = sqrt((sumsq / M->rows) - (mean*mean));
        R.elements[j] = stddev;
    }
    clock_t end_time = clock();
    double elapsed_time = ((double) (end_time - start_time)) / CLOCKS_PER_SEC;
    printf("Standard deviation of each column:\n");
    for (int j = 0; j < M->cols; j++) {
        printf("Column %d: Stddev=%f\n", j+1, R.elements[j]);
    }
    printf("Elapsed time: %f seconds\n", elapsed_time);
    free(R.elements);
}

void* computeStd(void* arg) {
    MatricesStandardDeviationArgs* args = (MatricesStandardDeviationArgs*) arg;
    Vector* R = args->R;
    Matrix* M = args->M;
    int start_row = args->start_row;
    int end_row = args->end_row;
    pthread_mutex_t* mutex = args->mutex;
    for (int j = 0; j < M->cols; j++) {
        double sum = 0.0, sumsq = 0.0;
        for (int i = start_row; i < end_row; i++) {
            double val = M->elements[i][j];
            sum += val;
            sumsq += val*val;
        }
        pthread_mutex_lock(mutex);
        double mean = R->elements[j] / M->rows;
        double stddev = sqrt((sumsq / M->rows) - (mean*mean));
        R->elements[j] = stddev;
        pthread_mutex_unlock(mutex);
    }
    pthread_exit(NULL);
}

void findStdThreaded(Matrix* M) {
    Vector R;
    R.size = M->cols;
    R.elements = (double*) malloc(R.size * sizeof(double));
    pthread_t* threads = (pthread_t*) malloc(M->cols * sizeof(pthread_t));
    MatricesStandardDeviationArgs args[M->cols];
    pthread_mutex_t mutex;
    pthread_mutex_init(&mutex, NULL);
    clock_t start_time = clock();
    for (int j = 0; j < M->cols; j++) {
        args[j].R = &R;
        args[j].M = M;
        args[j].start_row = 0;
        args[j].end_row = M->rows;
        args[j].mutex = &mutex;
        pthread_create(&threads[j], NULL, computeStd, (void*) &args[j]);
    }
    for (int j = 0; j < M->cols; j++) {
        pthread_join(threads[j], NULL);
    }
    clock_t end_time = clock();
    double elapsed_time = ((double) (end_time - start_time)) / CLOCKS_PER_SEC;
    printf("Standard deviation of each column:\n");
    for (int j = 0; j < M->cols; j++) {
        printf("Column %d: Stddev=%f\n", j+1, R.elements[j]);
    }
    printf("Elapsed time: %f seconds\n", elapsed_time);
    free(R.elements);
    free(threads);
    pthread_mutex_destroy(&mutex);
}