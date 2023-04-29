#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/time.h>
#include <pthread.h>
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

void calculate_normalize_formula1(int rows, int cols, int n, int file);
void calculate_formula1_without_parallel(Matrix* M, Vector* larger_numbers, Vector* minors_numbers);
void calculate_formula1_with_parallel(Matrix* M, Vector* mayores, Vector* menores, int n);
void* parallel_formula1_method(void* args_ptr);



void calculate_normalize_formula1(int rows, int cols, int n, int file){
    validate_one_matrix_operation(rows, cols, n);

    struct Matrix* matrix_pointer=NULL;
    if( file==1) {
        matrix_pointer =create_matrix_from_file("op1.txt",rows,cols);
    }
    else {
        matrix_pointer = create_matrix(rows, cols);
        init_matrix_rand(matrix_pointer);
    }
    Vector* larger_numbers = matrix_col_max(matrix_pointer);
    printf("Vector de numeros mayores: \n");
    print_vector(larger_numbers);
    Vector* minors_numbers = matrix_col_min(matrix_pointer);
    printf("Vector de numeros menores: \n");
    print_vector(minors_numbers);
    print_matrix(matrix_pointer);   
    Matrix* M1 = create_matrix(rows, cols);
    copy_matrix(M1,matrix_pointer);
    calculate_formula1_without_parallel(M1, larger_numbers, minors_numbers);
    Matrix* M2 = create_matrix(rows, cols);
    copy_matrix(M2,matrix_pointer);
    calculate_formula1_with_parallel(M2,larger_numbers, minors_numbers, n);
    free_matrix(matrix_pointer);
}

//Función que realiza la normalización de la matriz por un escalar sin usar paralelismo
void calculate_formula1_without_parallel(Matrix* M, Vector* larger_numbers, Vector* minors_numbers){
    printf("\nCalculo de normalizacion con formula 1 SIN paralelismo\n");

    struct timeval start, end;
    gettimeofday(&start, 0);

    normalize_matrix_column_formula_1(M, larger_numbers, minors_numbers);

    gettimeofday(&end, 0);
    double elapsed_time = ((double)(end.tv_usec - start.tv_usec) /1000000 + (double)(end.tv_sec - start.tv_sec));
    printf("Tiempo de ejecución: %fms\n", elapsed_time);

    print_matrix(M);
    free_matrix(M);
}

//Función que realiza la normalización de una matriz usando paralelismo
void calculate_formula1_with_parallel(Matrix* M, Vector* mayores, Vector* menores, int n) {
    printf("\nCalculo de normalizacion con formula 1 CON paralelismo\n");    

    struct timeval start, end;
    gettimeofday(&start, 0);

    const int num_threads =minor_value(n,M->rows);
    pthread_mutex_t lock;
    pthread_mutex_init(&lock, NULL);
    pthread_t threads[num_threads];
    Args_normalize_form1_m_t args[num_threads];
    int chunk_size = M->rows / num_threads;
    int extra_rows = M->rows % num_threads;
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
        pthread_create(&threads[i], NULL, parallel_formula1_method, &args[i]);
    }
    for (int i = 0; i < num_threads; ++i) {
        pthread_join(threads[i], NULL);
    }
    pthread_mutex_destroy(&lock);

    gettimeofday(&end, 0);
    double elapsed_time = ((double)(end.tv_usec - start.tv_usec) /1000000 + (double)(end.tv_sec - start.tv_sec));
    printf("Tiempo de ejecución: %fms\n", elapsed_time);

    print_matrix(M);
    free_matrix(M);
}


//Función principal que normaliza la matriz
void* parallel_formula1_method(void* args_ptr) {
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
