#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/time.h>
#include <pthread.h>
#include "../tools/execution_time.c" 
#include "../tools/validations.c"
#include "../tools/minorValue.c"

//Estrucutra para pasar los argumentos a los hilos para normalizar una matriz
typedef struct {
    int start_row;
    int end_row;
    Matrix* M;
    Vector* vrz;
    Vector* std;
    pthread_mutex_t* mutex;
} Args_matrix_t;

//Función para normalizar una matriz usando paralelismo
void* normalize_matrix_column_formula_2_row_thread(void* args) {
    //Se castea el argumento a la estructura que se creo
    Args_matrix_t* thread_args = (Args_matrix_t*) args;
    //Se itera sobre el rango de filas asignado al hilo
    for (int i = thread_args->start_row; i < thread_args->end_row; ++i) {
        for (int j = 0; j < thread_args->M->cols; ++j) {
            //Se adquiere el lock para asegurar la exclusion mutua
            pthread_mutex_lock(thread_args->mutex);
            //Se realiza el calculo de la normalizacion
            thread_args->M->elements[i][j] = (thread_args->M->elements[i][j] - thread_args->vrz->elements[j]) / thread_args->std->elements[j];
            //Se libera el lock
            pthread_mutex_unlock(thread_args->mutex);
        }
    }
    pthread_exit(NULL);
}
//Función que recibe los argumentos para realizar la normalizacion
void main_normalize_formula_2(int rows, int cols, int n,int file){
    //Se valida que los datos ingresados sean correctos
   validate_data_operation_with_one_matrix(rows, cols, n);
   //Se valida que el numero de filas no sea igual a 1
    if(rows==1){
        printf("It's impossible to normalize a matrix with only one row\n");
        exit(EXIT_FAILURE);
    }else{
        //Se llama a la funcion para normalizar una matriz
        main_normalize_matrix_formula2(rows, cols, n, file);
    }

}
//Función usada para normalizar una matriz
void main_normalize_matrix_formula2(int rows, int cols, int n, int file){

    Matrix* M=NULL;
    //Se valida si se va a leer la matriz desde un archivo
    if(file==1){
        M = create_matrix_from_file("op1.txt",rows, cols);
    }else{
        //Se crea una matriz aleatoria
        M = create_matrix(rows, cols);
        //Se inicializa la matriz con valores aleatorios
        init_matrix_rand(M);
    }
    //Se imprime la matriz
    print_matrix(M); 
    //Se calcula la media de las columnas
    printf("La media de las columnas es: \n");
    Vector* vrz = matrix_col_vrz(M);
    print_vector(vrz);
    //Se calcula la desviacion estandar de las columnas
    printf("La desviacion estandar de las columnas es: \n");
    Vector* std = matrix_col_std(M);
    print_vector(std);  
    //Se crea una matriz auxiliar
    Matrix* M1 = create_matrix(rows, cols);
    //Se copia la matriz original en la matriz auxiliar
    copy_matrix(M1,M);
    //Se normaliza la matriz sin usar paralelismo
    normalize_matrix_formula2_without_parallel_programming(M1, vrz,std);
    //Se crea una matriz auxiliar
    Matrix* M2 = create_matrix(rows, cols);
    //Se copia la matriz original en la matriz auxiliar
    copy_matrix(M2,M);
    //Se normaliza la matriz usando paralelismo
    normalize_matrix_formula2_with_parallel_programming(M2, vrz,std,n);
    //Se libera la memoria de la matriz
    free_matrix(M);
}
//función para una matriz sin paralelismo
void normalize_matrix_formula2_without_parallel_programming(Matrix* M, Vector* vrz, Vector* std){
    printf("\nNormalize matrix  formula 2 without Parallel programming\n");
    //Se obtiene el tiempo de inicio
    struct timeval start, end;
    gettimeofday(&start, 0);
    //Se normaliza la matriz
    normalize_matrix_column_formula_2(M, vrz,std);
    gettimeofday(&end, 0);
    //Se imprime los resultados
    get_execution_time(start, end);
    print_matrix(M);
    free_matrix(M);
}

// Función para normalizar una matriz con paralelismo
void normalize_matrix_formula2_with_parallel_programming(Matrix* M, Vector* vrz, Vector* std, int n) {
    printf("\nNormalize matrix formula 2 with Parallel programming\n");
    struct timeval start, end;
    //Se inicializan los threads
    int num_threads = minor_value(n, M->rows);
    pthread_t threads[num_threads];
    Args_matrix_t thread_args[num_threads];
    pthread_mutex_t mutex;
    pthread_mutex_init(&mutex, NULL);
    // Se calcula el tamaño de cada cantidad de filas que se le asignará a cada thread
    int chunk_size = M->rows / num_threads;
    int extra_rows = M->rows % num_threads;
    int start_row = 0;
    // Se obtiene el tiempo de inicio
    gettimeofday(&start, 0);
    // Se crean los threads
    for (int i = 0; i < num_threads; ++i) {
        // Se inicializan los argumentos de cada thread
        int rows = chunk_size + (i < extra_rows);
        thread_args[i].start_row = start_row;
        thread_args[i].end_row = start_row + rows;
        thread_args[i].M = M;
        thread_args[i].vrz = vrz;
        thread_args[i].std = std;
        thread_args[i].mutex = &mutex;
        start_row += rows;
        // Se crea el thread con la función normalize_matrix_column_formula_2_row_thread
        pthread_create(&threads[i], NULL, normalize_matrix_column_formula_2_row_thread, &thread_args[i]);
    }
    // Se espera a que terminen los threads
    for (int i = 0; i < num_threads; ++i) {
        pthread_join(threads[i], NULL);
    }
    pthread_mutex_destroy(&mutex);
    gettimeofday(&end, 0);
    //Se imprimen los resultados
    get_execution_time(start, end);
    print_matrix(M);
    free_matrix(M);
}