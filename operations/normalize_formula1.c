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
    //Se castean los argumentos
    Args_normalize_form1_m_t* args = (Args_normalize_form1_m_t*) args_ptr;
    //Se itera sobre el rango asignado al hilo
    for (int i = args->start_row; i < args->end_row; ++i) {
        for (int j = 0; j < args->M->cols; ++j) {
            //Se adquiere el lock para asegurar la exclusión mutua
            pthread_mutex_lock(args->lock);
            //Se realiza el cálculo de la normalización
            args->M->elements[i][j] = (args->M->elements[i][j] - args->menores->elements[j]) / (args->mayores->elements[j] - args->menores->elements[j]);
            //Se libera el lock
            pthread_mutex_unlock(args->lock);
        }
    }

    return NULL;
}

//Función principal que recibe los datos de entrada y llama a las funciones que realizan la normalización
void main_normalize_formula_1(int rows, int cols, int n, int file){
    //Se valida que los datos de entrada sean correctos
    validate_data_operation_with_one_matrix(rows, cols, n);
    //Se valida si el numero de filas es iagual a 1 o no
    if(rows==1){
       printf("It's impossible to normalize a matrix with only one row\n");
        exit(EXIT_FAILURE);
    }else{
        //Se llama a la función que normaliza la matriz
        main_normalize_matrix_formula1(rows, cols, n, file);
    }
    
}

//Función que llama las funciones que realizan la normalización de la matriz
void main_normalize_matrix_formula1(int rows, int cols, int n, int file){

    struct Matrix* M=NULL;
    //Se valida si se va a leer la matriz desde un archivo o no
    if( file==1){
        M =create_matrix_from_file("op1.txt",rows,cols);
    }else{
        //Se crea la matriz
        M = create_matrix(rows, cols);
        //Se inicializa la matriz con numeros aleatorios
        init_matrix_rand(M);
    }
    //Se obtienen los numeros mayores y menores de cada columna
    Vector* larger_numbers = matrix_col_max(M);
    printf("Vector de numeros mayores: \n");
    print_vector(larger_numbers);
    Vector* minors_numbers = matrix_col_min(M);
    printf("Vector de numeros menores: \n");
    print_vector(minors_numbers);
    //Se imprime la matriz
    print_matrix(M);   
    //Creamos otra matriz auxiliar para realizar las operaciones
    Matrix* M1 = create_matrix(rows, cols);
    //Copiamos la matriz original a la matriz que creamos
    copy_matrix(M1,M);
    //Llamamos a la funcion que realiza la normalizacion de la matriz por un escalar sin usar paralelismo
    normalize_matrix_formula1_without_parallel_programming(M1, larger_numbers, minors_numbers);
    Matrix* M2 = create_matrix(rows, cols);
    //Se copia la matriz original a la matriz auxiliar
    copy_matrix(M2,M);
    //Se llama a la funcion que realiza la normalicón de la matriz por un escalar usando paralelismo
    normalize_matrix_formula1_with_parallel_programming(M2,larger_numbers, minors_numbers, n);
    //Se libera la memoria de las matrices
    free_matrix(M);
}

//Función que realiza la normalización de la matriz por un escalar sin usar paralelismo
void normalize_matrix_formula1_without_parallel_programming(Matrix* M, Vector* larger_numbers, Vector* minors_numbers){
    //Se imprime el mensaje de inicio de la normalización
    printf("\nNormalize matrix  formula 1 without Parallel programming\n");
    //Se obtiene el tiempo de inicio
    struct timeval start, end;
    gettimeofday(&start, 0);
    //Se realiza la normalización
    normalize_matrix_column_formula_1(M, larger_numbers, minors_numbers);
    //Se obtiene el tiempo de finalización
    gettimeofday(&end, 0);
    get_execution_time(start, end);
    //Se imprimen los resultados
    print_matrix(M);
    free_matrix(M);
}

//Función que realiza la normalización de una matriz usando paralelismo
void normalize_matrix_formula1_with_parallel_programming(Matrix* M, Vector* mayores, Vector* menores, int n) {
    printf("\nNormalize matrix formula 1 with Parallel programming\n");    
    struct timeval start, end;
    const int num_threads =minor_value(n,M->rows);  // Número de hilos
    pthread_mutex_t lock;
    //Se inicializan los locks
    pthread_mutex_init(&lock, NULL);    
    //Se crean los hilos
    pthread_t threads[num_threads];
    //Se crean los argumentos
    Args_normalize_form1_m_t args[num_threads];
    //Se calcula la cantidad de filas que va a tener cada hilo
    int chunk_size = M->rows / num_threads;
    int extra_rows = M->rows % num_threads;
    //Se obtiene el tiempo de inicio
    gettimeofday(&start, 0);
    //Se ponen los argumentos en cada hilo
    for (int i = 0; i < num_threads; ++i) {
        args[i].M = M;
        args[i].mayores = mayores;
        args[i].menores = menores;
        args[i].lock = &lock;
        //Se calcula el rango de filas que va a tener cada hilo
        int start_row = i * chunk_size;
        int end_row = (i + 1) * chunk_size;
        if (i == num_threads - 1) {
            end_row += extra_rows;
        }
        args[i].start_row = start_row;
        args[i].end_row = end_row;
        //Se llama a la función que realiza la normalización por cada hilo
        pthread_create(&threads[i], NULL, normalize_formula_1, &args[i]);
    }
    //Se espera a que terminen los hilos
    for (int i = 0; i < num_threads; ++i) {
        pthread_join(threads[i], NULL);
    }
    //Se destruyen los locks
    pthread_mutex_destroy(&lock);
    gettimeofday(&end, 0);

    //Se imprime el tiempo de ejecucion y el resultado de la multiplicación
    get_execution_time(start, end);
    //Se imprime la matriz
    print_matrix(M);
    //Se libera la memoria de la matriz
    free_matrix(M);
}

