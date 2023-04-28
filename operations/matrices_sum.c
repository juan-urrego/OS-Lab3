#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/time.h>
#include "../tools/execution_time.c"
#include <pthread.h>
#include "../tools/matrix.c"
#include "../tools/validations.c"
#include "../tools/minorValue.c"

void add_two_matrices(int m1_rows, int m1_cols, int m2_rows, int m2_cols, int n, int file);
void add_matrices_without_paralellism(Matrix *M, Matrix *N);
void add_matrices_with_paralellism(Matrix *M, Matrix *N, int n);

// Estructura de datos para pasar los argumentos a la funcion que realiza la suma de matrices usando paralelismo
typedef struct
{
    Matrix *R;
    Matrix *M;
    Matrix *N;
    int start_row;
    int end_row;
    pthread_mutex_t *mutex;
} MatricesSumArgs;

// Funcion que realiza la suma de las matrices usando paralelismo
void *add_matrices_parallel(void *arg)
{
    MatricesSumArgs *args = (MatricesSumArgs *)arg;
    if (args->M->rows != args->N->rows || args->M->cols != args->N->cols)
    {
        fprintf(stderr, "Invalid size. (%d, %d) and (%d, %d)\n", args->M->rows, args->M->cols, args->N->rows, args->N->cols);
        return NULL;
    }
    for (int i = args->start_row; i < args->end_row; ++i)
    {
        for (int j = 0; j < args->M->cols; ++j)
        {
            pthread_mutex_lock(args->mutex);
            args->R->elements[i][j] = args->M->elements[i][j] + args->N->elements[i][j];
            pthread_mutex_unlock(args->mutex);
        }
    }
    return NULL;
}

void add_two_matrices(int m1_rows, int m1_cols, int m2_rows, int m2_cols, int n, int file)
{
    validate_data_operation_with_two_matrices(m1_rows, m1_cols, m2_rows, m2_cols, n);
    Matrix *M = NULL, *N = NULL;
    // se valida si se va a leer la matriz de un archivo
    if (file == 1)
    {
        M = create_matrix_from_file("op1.txt", m1_rows, m1_cols);
        N = create_matrix_from_file("op2.txt", m2_rows, m2_cols);
    }
    else
    {
        // Se crea la matriz 1
        M = create_matrix(m1_rows, m1_cols);
        // Se inicializa la matriz 1 con numeros aleatorios
        init_matrix_rand(M);
        N = create_matrix(m2_rows, m2_cols);
        // Se inicializa la matriz 2 con numeros aleatorios
        init_matrix_rand(N);
        // Se imprime la matriz 2
    }

    // Se imprime la matriz 1
    print_matrix(M);
    // Se imprime la matriz 2
    print_matrix(N);

    // Suma de matrices sin paralelismo
    add_matrices_without_paralellism(M, N);

    // Suma de matrices con paralelismo
    add_matrices_with_paralellism(M, N, n);

    free_matrix(M);
    free_matrix(N);
}

// Funcion para realizar la suma de la matriz  sin usar paralelismo
void add_matrices_without_paralellism(Matrix *M, Matrix *N)
{
    printf("\nSum of matrices without paralellism\n");
    // Se obtiene el tiempo de inicio
    struct timeval start, end;
    gettimeofday(&start, 0);
    // Suma de matrices sin paralelismo
    Matrix *R = add_matrix(M, N);
    // Se obtiene el tiempo de finalizacion
    gettimeofday(&end, 0);
    // Se imprime el tiempo de ejecucion y el resultado de la suma
    get_execution_time(start, end);
    print_matrix(R);
    free_matrix(R);
}

void add_matrices_with_paralellism(Matrix *M, Matrix *N, int n)
{
    printf("\nParallel sum of matrices with paralellism\n");
    struct timeval start, end;
    int num_threads = minor_value(n, M->rows);               // Número de hilos, que sea menor que el numero de filas (paralelización por filas)
    Matrix *R = create_matrix(M->rows, M->cols); // Matriz con el resultado de la suma
    // Se crea un arreglo de hilos
    pthread_t threads[num_threads];
    // se inicializa la estructura de datos con el número de hilos a utilizar
    MatricesSumArgs thread_args[num_threads];
    // se inicializa el lock
    pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

    // Se obtiene el tiempo de inicio
    gettimeofday(&start, 0);
    // Se divide la matriz en partes iguales para cada hilo
    const int chunk_size = M->rows / num_threads;
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
        thread_args[i] = (MatricesSumArgs){.R = R, .M = M, .N = N, .start_row = start_row, .end_row = end_row, .mutex = &mutex};
        pthread_create(&threads[i], NULL, add_matrices_parallel, &thread_args[i]);
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
    // Se imprime el tiempo de ejecucion y el resultado de la suma
    get_execution_time(start, end);
    // Se imprime la matriz resultado
    print_matrix(R);
    // Se libera la memoria de las matrices
    free_matrix(R);
}
