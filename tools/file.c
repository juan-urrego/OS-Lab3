#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifndef READ_FILE_H
#define READ_FILE_H
//cantidad de lineas que se pueden leer
#define MAX_LINE_LENGTH 40000
//función que lee el archivo de entrada
void read_file(int *o,int *f, int *c, double *e, int *r, int *s, char* p, int *n){
    //variables para leer el archivo
    char line[MAX_LINE_LENGTH];
    //variables para guardar los datos del archivo
    char variable[MAX_LINE_LENGTH];
    //matriz para guardar los datos de las matrices
    float **matrix;

    //Se crea el archivo de salida del operador 1
    FILE *output_file_op1 = fopen("op1.txt", "w");
    if (output_file_op1 == NULL) {
        printf("Error creating file op1.txt\n");
        exit(1);
    }

    //Se crea el archivo de salida del operador 2
    FILE *output_file_op2 = fopen("op2.txt", "w");
    if (output_file_op2 == NULL) {
        printf("Error creating file op2.txt\n");
        exit(1);
    }

    //Se abre el archivo de entrada
    FILE *data=fopen(p,"r");
    if (data == NULL) {
        printf("Error opening input file %s\n", p);
        exit(1);
    }
    //Se lee el archivo linea por linea
    while (fgets(line, MAX_LINE_LENGTH, data)) {
        //Se lee cada linea del archivo y se guarda en la variable 'variable'
        sscanf(line, "%s", variable);
        //Se valida que la variable sea igual a 's'
        if (strcmp(variable, "s") == 0) { // si la variable es 's'
            sscanf(line, "s %lf", e); // extrae el valor del escalar
        } else if (strcmp(variable, "o") == 0) { // si la variable es 'o'
             sscanf(line, "o %d", o); // extrae el valor de 'o'
        }else if (strcmp(variable, "n") == 0) { // si la variable es 'o'
             sscanf(line, "n %d", n); // extrae el valor de 'o'
        }else  if (strcmp(variable, "op1") == 0) { // si la variable es 'op1'
            sscanf(line, "op1 %d %d", f, c); // extrae los valores f y c
            matrix = (float **)malloc((*f) * sizeof(float *));
            //Se lee cada uno se los valores de la matriz
            for (int i = 0; i < *f; i++) {
                matrix[i] = (float *)malloc((*c) * sizeof(float));
                fgets(line, MAX_LINE_LENGTH, data);
                char *token = strtok(line, " ");
                for (int j = 0; j < *c; j++) {
                    //Se guarda cada uno de los valores en la matriz en la variable matrix
                    sscanf(token, "%f", &matrix[i][j]);
                    token = strtok(NULL, " ");
                    //Se escribe en el archivo de salida del operador 1 cada uno de los valores
                    fprintf(output_file_op1, "%f ", matrix[i][j]);
                }
                //Se escribe un salto de linea 
                fprintf(output_file_op1, "\n");
            }
            //se hace lo mismo pero con el operador 2
        }else if (strcmp(variable, "op2") == 0) {
            sscanf(line, "op2 %d %d", r, s);
            matrix = (float **)malloc((*r) * sizeof(float *));
            for (int i = 0; i < *r; i++) {
                matrix[i] = (float *)malloc((*s) * sizeof(float));
                fgets(line, MAX_LINE_LENGTH, data);
                char *token = strtok(line, " ");
                for (int j = 0; j < *s; j++) {
                    sscanf(token, "%f", &matrix[i][j]);
                    token = strtok(NULL, " ");
                    fprintf(output_file_op2, "%f ", matrix[i][j]);
                }
                fprintf(output_file_op2, "\n");
            }
        }
    }
    //Se cierran los archivos
    fclose(data);
    fclose(output_file_op1);
    fclose(output_file_op2);
}

#endif 