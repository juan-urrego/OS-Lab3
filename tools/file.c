#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifndef READ_FILE_H
#define READ_FILE_H


#define MAX_LINE_LENGTH 40000


void read_file(int *c, double *e, int *f, int *n, int *o, char *p, int *r, int *s ){
    float **matrix;
    char variable[MAX_LINE_LENGTH];
    char line[MAX_LINE_LENGTH];

    /*Se crean los archivos de salida de los operadores*/
    FILE *output_file_op1 = fopen("op1.txt", "w");
    if (output_file_op1 == NULL) {
        printf("Error creating file op1.txt\n");
        exit(1);
    }

    FILE *output_file_op2 = fopen("op2.txt", "w");
    if (output_file_op2 == NULL) {
        printf("Error creando el archivo op2.txt\n");
        exit(1);
    }

    //Se abre el archivo de entrada
    FILE *data=fopen(p,"r");
    if (data == NULL) {
        printf("Error abriendo el archivo de entrada %s\n", p);
        exit(1);
    }
    //Se lee el archivo linea por linea
    while (fgets(line, MAX_LINE_LENGTH, data)) {
        sscanf(line, "%s", variable);
        if (strcmp(variable, "s") == 0) {
            sscanf(line, "s %lf", e);
        } else if (strcmp(variable, "o") == 0) {
             sscanf(line, "o %d", o);
        }else if (strcmp(variable, "n") == 0) {
             sscanf(line, "n %d", n);
        }else  if (strcmp(variable, "op1") == 0) {
            sscanf(line, "op1 %d %d", f, c);
            matrix = (float **)malloc((*f) * sizeof(float *));
            for (int i = 0; i < *f; i++) {
                matrix[i] = (float *)malloc((*c) * sizeof(float));
                fgets(line, MAX_LINE_LENGTH, data);
                char *token = strtok(line, " ");
                for (int j = 0; j < *c; j++) {
                    sscanf(token, "%f", &matrix[i][j]);
                    token = strtok(NULL, " ");
                    fprintf(output_file_op1, "%f ", matrix[i][j]);
                }
                fprintf(output_file_op1, "\n");
            }
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
    fclose(data);
    fclose(output_file_op1);
    fclose(output_file_op2);
}

#endif 