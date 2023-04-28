#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/time.h>
#include "../tools/execution_time.c"
#include <pthread.h>
#include "../tools/matrix.c"
#include "../tools/validations.c"

// Estructura de datos para pasar los argumentos a la funcion que realiza la varianza de columnas de una matriz, usando paralelismo
typedef struct
{
    Vector *R;
    Matrix *M;
    int start_row;
    int end_row;
    pthread_mutex_t *mutex;
} MatricesVarianceArgs;

void columns_variance(Matrix *M)
{
    printf("\nVariance of matrices without paralellism\n");
    // Se obtiene el tiempo de inicio
    struct timeval start, end;
    gettimeofday(&start, 0);
    // Media de matrices sin paralelismo
    Vector *R = matrix_col_vrz(M);
    // Se obtiene el tiempo de fin
    gettimeofday(&end, 0);
    // Se imprime el tiempo de ejecucion y el resultado de la media
    get_execution_time(start, end);
    print_vector(R);
    free_vector(R);
}

// Funcion que realiza la media de las matrices usando paralelismo
void *column_variance_parallel(void *arg)
{
    MatricesVarianceArgs *args = (MatricesVarianceArgs *)arg;
    Vector *mean = matrix_col_mean(args->M);
    for (int i = args->start_row; i < args->end_row; ++i)
    {
        double sum = 0.0;
        for (int j = 0; j < args->M->rows; ++j)
        {
            sum += pow(args->M->elements[j][i] - mean->elements[i], 2);
        }
        pthread_mutex_lock(args->mutex);
        args->R->elements[i] = sum / args->M->rows;
        pthread_mutex_unlock(args->mutex);
    }
    return NULL;
}

void columns_variance_parallel(Matrix *M, int n)
{
    printf("\nVariance of matrices with paralellism\n");
    // Se obtiene el tiempo de inicio
    struct timeval start, end;
    // Se asigna el número de hilos a utilizar
    const int num_threads = n;
    Vector *R = create_vector(M->cols); // Vector con el resultado de la varianza
    // Se crea un arreglo de hilos
    pthread_t threads[num_threads];
    // se inicializa la estructura de datos con el número de hilos a utilizar
    MatricesVarianceArgs thread_args[num_threads];
    // se inicializa el lock
    pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
    // Se obtiene el tiempo de inicio
    gettimeofday(&start, 0);
    // Se divide la matriz en partes iguales para cada hilo
    const int chunk_size = M->cols / num_threads;
    // Se le asigna a cada hilo una parte de la matriz
    int start_row = 0;
    for (int i = 0; i < num_threads; ++i)
    {
        int end_row = start_row + chunk_size;
        if (i == num_threads - 1)
        {
            end_row = M->rows;
        }
        // Se inicializan los datos necesarios para cada hilo
        thread_args[i] = (MatricesVarianceArgs){.R = R, .M = M, .start_row = start_row, .end_row = end_row, .mutex = &mutex};
        pthread_create(&threads[i], NULL, column_variance_parallel, &thread_args[i]);
        start_row = end_row;
    }
    // Esperamos a que todos los hilos terminen
    for (int i = 0; i < num_threads; ++i)
    {
        pthread_join(threads[i], NULL);
    }
    pthread_mutex_destroy(&mutex); // Destruimos el mutex
    // Se obtiene el tiempo de finalizacion
    gettimeofday(&end, 0);
    // Se imprime el tiempo de ejecucion y el resultado de la varianza
    get_execution_time(start, end);
    // Se imprime el vector resultado
    print_vector(R);
    // Se libera la memoria del vector resultado
    free_vector(R);
}

void matrix_columns_variance(int m_rows, int m_cols, int n, int file)
{   
    // Valida que el número de columnas sea mayor a 0.
    validate_data_operation_with_one_matrix(m_rows, m_cols, n);
    // Valida que haya más de una fila.
    validate_data_operation_with_one_matrix_rows(m_rows);
    Matrix *M = NULL;
    // Se valida si se ingresa un archivo o se crea una matriz aleatoria
    if (file == 1)
    {
        // Se crea la matriz
        M = create_matrix_from_file("op1.txt", m_rows, m_cols);
    }
    else
    {
        // Se crea la matriz
        M = create_matrix(m_rows, m_cols);
        // Se inicializa la matriz con numeros aleatorios
        init_matrix_rand(M);
    }
    // Se imprime la matriz
    print_matrix(M);
    // Se ejecuta el calculo de la media de las matrices sin paralelismo
    columns_variance(M);
    // Se ejecuta el calculo de la media de las matrices con paralelismo
    columns_variance_parallel(M, n);
    // Se libera la memoria de la matriz
    free_matrix(M);
};