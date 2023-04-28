#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/time.h>
#include <pthread.h>
#include "../tools/execution_time.c" 
#include "../tools/validations.c"
#include "../tools/minorValue.c"
//Estructura para pasar los argumentos a la función que normaliza la matriz
typedef struct {
    Matrix* M;
    Vector* mayores;
    Vector* menores;
    int start_row;
    int end_row;
    pthread_mutex_t* lock;
} Args_normalize_form1_m_t;

//Función principal que normaliza la matriz
void* normalize_formula_1(void* args_ptr) {
    Args_normalize_form1_m_t* args = (Args_normalize_form1_m_t*) args_ptr;
    for (int i = args->start_row; i < args->end_row; ++i) {
        for (int j = 0; j < args->M->cols; ++j) {
            pthread_mutex_lock(args->lock);
            args->M->elements[i][j] = (args->M->elements[i][j] - args->menores->elements[j]) / (args->mayores->elements[j] - args->menores->elements[j]);
            pthread_mutex_unlock(args->lock);
        }
    }

    return NULL;
}

//Función principal que recibe los datos de entrada y llama a las funciones que realizan la normalización
void main_normalize_formula_1(int rows, int cols, int n, int file){
    validate_data_operation_with_one_matrix(rows, cols, n);
    if(rows==1) {
       printf("It's impossible to normalize a matrix with only one row\n");
        exit(EXIT_FAILURE);
    }
    else {
        main_normalize_matrix_formula1(rows, cols, n, file);
    }
    
}

//Función que llama las funciones que realizan la normalización de la matriz
void main_normalize_matrix_formula1(int rows, int cols, int n, int file){

    struct Matrix* M=NULL;
    if( file==1) {
        M =create_matrix_from_file("op1.txt",rows,cols);
    }
    else {
        M = create_matrix(rows, cols);
        init_matrix_rand(M);
    }
    Vector* larger_numbers = matrix_col_max(M);
    printf("Vector de numeros mayores: \n");
    print_vector(larger_numbers);
    Vector* minors_numbers = matrix_col_min(M);
    printf("Vector de numeros menores: \n");
    print_vector(minors_numbers);
    print_matrix(M);   
    Matrix* M1 = create_matrix(rows, cols);
    copy_matrix(M1,M);
    normalize_matrix_formula1_without_parallel_programming(M1, larger_numbers, minors_numbers);
    Matrix* M2 = create_matrix(rows, cols);
    copy_matrix(M2,M);
    normalize_matrix_formula1_with_parallel_programming(M2,larger_numbers, minors_numbers, n);
    free_matrix(M);
}

//Función que realiza la normalización de la matriz por un escalar sin usar paralelismo
void normalize_matrix_formula1_without_parallel_programming(Matrix* M, Vector* larger_numbers, Vector* minors_numbers){
    printf("\nNormalize matrix  formula 1 without Parallel programming\n");
    struct timeval start, end;
    gettimeofday(&start, 0);
    normalize_matrix_column_formula_1(M, larger_numbers, minors_numbers);
    gettimeofday(&end, 0);
    get_execution_time(start, end);
    print_matrix(M);
    free_matrix(M);
}

//Función que realiza la normalización de una matriz usando paralelismo
void normalize_matrix_formula1_with_parallel_programming(Matrix* M, Vector* mayores, Vector* menores, int n) {
    printf("\nNormalize matrix formula 1 with Parallel programming\n");    
    struct timeval start, end;
    const int num_threads =minor_value(n,M->rows);
    pthread_mutex_t lock;
    pthread_mutex_init(&lock, NULL);
    pthread_t threads[num_threads];
    Args_normalize_form1_m_t args[num_threads];
    int chunk_size = M->rows / num_threads;
    int extra_rows = M->rows % num_threads;
    gettimeofday(&start, 0);
    for (int i = 0; i < num_threads; ++i) {
        args[i].M = M;
        args[i].mayores = mayores;
        args[i].menores = menores;
        args[i].lock = &lock;
        int start_row = i * chunk_size;
        int end_row = (i + 1) * chunk_size;
        if (i == num_threads - 1) {
            end_row += extra_rows;
        }
        args[i].start_row = start_row;
        args[i].end_row = end_row;
        pthread_create(&threads[i], NULL, normalize_formula_1, &args[i]);
    }
    for (int i = 0; i < num_threads; ++i) {
        pthread_join(threads[i], NULL);
    }
    pthread_mutex_destroy(&lock);
    gettimeofday(&end, 0);

    get_execution_time(start, end);
    print_matrix(M);
    free_matrix(M);
}

