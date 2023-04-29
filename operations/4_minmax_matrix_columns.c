#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/time.h>
#include <pthread.h>
#include "../tools/matrix.c"
#include "../tools/validations.c"


typedef struct
{
    Vector *R;
    Matrix *M;
    int start_row;
    int end_row;
    pthread_mutex_t *mutex;
} MatricesMinMaxArgs;

void calculate_matrix_columns_min_max(int m_rows, int m_cols, int n, int file);
void* findMinMaxThread(void* arg);
void findMinMaxThreaded(Matrix* M);
void findMinMax(Matrix* M);

void calculate_matrix_columns_min_max(int m_rows, int m_cols, int n, int file) {
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
    findMinMax(M); // Encontrar el mínimo y máximo de cada columna sin usar hilos
    findMinMaxThreaded(M); // Encontrar el mínimo y máximo de cada columna usando hilos
}

void findMinMax(Matrix* M) {
    Vector R;
    R.size = M->cols * 2;
    R.elements = (double*) malloc(R.size * sizeof(double));
    clock_t start_time = clock();
    for (int j = 0; j < M->cols; j++) {
        double minVal = M->elements[0][j];
        double maxVal = M->elements[0][j];
        for (int i = 1; i < M->rows; i++) {
            double val = M->elements[i][j];
            if (val < minVal) {
                minVal = val;
            }
            if (val > maxVal) {
                maxVal = val;
            }
        }
        R.elements[j] = minVal;
        R.elements[M->cols + j] = maxVal;
    }
    clock_t end_time = clock();
    double elapsed_time = ((double) (end_time - start_time)) / CLOCKS_PER_SEC;
    printf("Minimum and Maximum values of each column:\n");
    for (int j = 0; j < M->cols; j++) {
        printf("Column %d: Minimum=%f, Maximum=%f\n", j+1, R.elements[j], R.elements[M->cols+j]);
    }
    printf("Elapsed time: %f seconds\n", elapsed_time);
    free(R.elements);
}

void* findMinMaxThread(void* arg) {
    MatricesMinMaxArgs* args = (MatricesMinMaxArgs*) arg;
    for (int j = 0; j < args->M->cols; j++) {
        double minVal = args->M->elements[0][j];
        double maxVal = args->M->elements[0][j];
        for (int i = args->start_row; i < args->end_row; i++) {
            double val = args->M->elements[i][j];
            if (val < minVal) {
                minVal = val;
            }
            if (val > maxVal) {
                maxVal = val;
            }
        }
        pthread_mutex_lock(args->mutex);
        args->R->elements[j] = minVal;
        args->R->elements[args->M->cols + j] = maxVal;
        pthread_mutex_unlock(args->mutex);
    }
    pthread_exit(NULL);
}

void findMinMaxThreaded(Matrix* M) {
    Vector R;
    R.size = M->cols * 2;
    R.elements = (double*) malloc(R.size * sizeof(double));
    pthread_t threads[M->cols];
    pthread_mutex_t mutex;
    pthread_mutex_init(&mutex, NULL);
    MatricesMinMaxArgs args[M->cols];
    int chunkSize = M->rows / M->cols;
    int remainder = M->rows % M->cols;
    int startRow = 0;
    for (int i = 0; i < M->cols; i++) {
        args[i].R = &R;
        args[i].M = M;
        args[i].start_row = startRow;
        args[i].end_row = startRow + chunkSize;
        if (remainder > 0) {
            args[i].end_row++;
            remainder--;
        }
        args[i].mutex = &mutex;
        pthread_create(&threads[i], NULL, findMinMaxThread, (void*) &args[i]);
        startRow = args[i].end_row;
    }
    for (int i = 0; i < M->cols; i++) {
        pthread_join(threads[i], NULL);
    }
    pthread_mutex_destroy(&mutex);
    printf("Minimum and Maximum values of each column (Threaded):\n");
    for (int j = 0; j < M->cols; j++) {
        printf("Column %d: Minimum=%f, Maximum=%f\n", j+1, R.elements[j], R.elements[M->cols+j]);
    }
    free(R.elements);
}