#ifndef MATRIX_H
#define MATRIX_H
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include <math.h>
#include <float.h>

typedef struct Vector Vector;
struct Vector {
    int size;
    double* elements;
};

typedef struct Matrix Matrix;
struct Matrix {
    int rows;
    int cols;
    double** elements;
};


Vector* create_vector(int size) {
    Vector* v = calloc(1, sizeof(Vector));
    v->size = size;
    v->elements = calloc(size, sizeof(double));
    return v;
}

Vector* create_vector_from_file(const char* file_path, int size) {
    Vector* v = create_vector(size);
    FILE* fp = fopen(file_path, "r");
    if (fp == NULL) {
        fprintf(stderr, "Failed to open \"%s\". Not possible to create vector from file.\n", file_path);
        return NULL;
    }

    double d;
    for (int i = 0; i < v->size; ++i) {
        const int r = fscanf(fp, "%lf", &d);
        if (r != 1) {
            fprintf(stderr, "fscanf failed.\n");
            fclose(fp);
            return NULL;
        }
        v->elements[i] = d;
    }

    fclose(fp);
    return v;
}

Matrix* create_matrix(int rows, int cols) {
    Matrix* M = malloc(sizeof(Matrix));
    M->rows = rows;
    M->cols = cols;
    M->elements = calloc(rows, sizeof(double*));
    for (int i = 0; i < rows; ++i) {
        M->elements[i] = calloc(cols, sizeof(double));
    }

    return M;
}

Matrix* create_matrix_from_file(const char* file_path, int rows, int cols) {
    Matrix* M = create_matrix(rows, cols);
    FILE* fp = fopen(file_path, "r");
    if (fp == NULL) {
        fprintf(stderr, "Failed to open \"%s\". Not possible to create matrix from file.\n", file_path);
        return NULL;
    }

    double d;
    for (int i = 0; i < M->rows; ++i) {
        for (int j = 0; j < M->cols; ++j) {
            const int r = fscanf(fp, "%lf", &d);
            if (r != 1) {
                fprintf(stderr, "fscanf failed.\n");
                fclose(fp);
                return NULL;
            }
            M->elements[i][j] = d;
        }
    }

    fclose(fp);
    return M;
}

void init_matrix_rand(Matrix* M) {
    for (int i = 0; i < M->rows; ++i) {
        for (int j = 0; j < M->cols; ++j) {
            M->elements[i][j] = (double)rand() / (double)RAND_MAX;
        }
    }
}

void init_vector_rand(Vector* V){
    for(int i = 0; i < V->size; ++i){
	V->elements[i] = (double)rand() / (double)RAND_MAX;
    }
}

void copy_matrix(Matrix* dst, const Matrix* src) {
    for (int i = 0; i < src->rows; ++i) {
        for (int j = 0; j < src->cols; ++j) {
            dst->elements[i][j] = src->elements[i][j];
        }
    }
}

void copy_vector(Vector* dst, const Vector* src) {
    for (int i = 0; i < src->size; ++i) {
        dst->elements[i] = src->elements[i];
    }
}

void free_vector(Vector* v) {
    free(v->elements);
    free(v);
}

void free_matrix(Matrix* M) {
    for (int i = 0; i < M->rows; ++i) {
        free(M->elements[i]);
    }
    free(M->elements);
    free(M);
}

Vector* add_vector(const Vector* a, const Vector* b) {
    if (a->size != b->size) {
        fprintf(stderr, "Invalid size. %d and %d\n", a->size, b->size);
        return NULL;
    }

    Vector* r = create_vector(a->size);
    for (int i = 0; i < r->size; ++i) {
        r->elements[i] = a->elements[i] + b->elements[i];
    }

    return r;
}

Vector* dot_vector_matrix(const Vector* v, const Matrix* M) {
    if (v->size != M->rows) {
        fprintf(stderr, "Invalid size. %d and (%d, %d)\n", v->size, M->rows, M->cols);
        return NULL;
    }

    Vector* r = create_vector(M->cols);
    for (int i = 0; i < r->size; ++i) {
        double d = 0.0;
        for (int j = 0; j < M->rows; ++j) {
            d += (v->elements[j] * M->elements[j][i]);
        }
        r->elements[i] = d;
    }

    return r;
}

Matrix* add_matrix(const Matrix* M, const Matrix* N) {
    if (M->rows != N->rows || M->cols != N->cols) {
        fprintf(stderr, "Invalid size. (%d, %d) and (%d, %d)\n", M->rows, M->cols, N->rows, N->cols);
        return NULL;
    }

    Matrix* R = create_matrix(M->rows, M->cols);
    for (int i = 0; i < M->rows; ++i) {
        for (int j = 0; j < M->cols; ++j) {
            R->elements[i][j] = M->elements[i][j] + N->elements[i][j];
        }
    }

    return R;
}

Matrix* dot_matrix(const Matrix* M, const Matrix* N) {
    if (M->cols != N->rows) {
        fprintf(stderr, "Invalid size. (%d, %d) and (%d, %d)\n", M->rows, M->cols, N->rows, N->cols);
        return NULL;
    }

    Matrix* A = create_matrix(M->rows, N->cols);
    for (int i = 0; i < M->rows; ++i) {
        for (int j = 0; j < N->cols; ++j) {
            double d = 0.0;
            for (int k = 0; k < M->cols; ++k) {
                d += M->elements[i][k] * N->elements[k][j];
            }
            A->elements[i][j] = d;
        }
    }

    return A;
}

Vector* matrix_col_mean(const Matrix* M) {
    Vector* V = create_vector(M->cols);

    for (int i = 0; i < M->cols; ++i) {
        double sum = 0.0;
        for (int j = 0; j < M->rows; ++j) {
            sum += M->elements[j][i];
        }
        V->elements[i] = sum / M->rows;
    }

    return V;
}

Vector* matrix_col_sum(const Matrix* M) {
    Vector* v = create_vector(M->cols);

    for (int i = 0; i < M->cols; ++i) {
        double sum = 0.0;
        for (int j = 0; j < M->rows; ++j) {
            sum += M->elements[j][i];
        }
        v->elements[i] = sum;
    }

    return v;
}

void scalar_matrix(Matrix* M, double k) {
    for (int i = 0; i < M->rows; ++i) {
       for (int j = 0; j < M->cols; ++j) {
            M->elements[i][j] *= k;
       }
    }
}

void scalar_vector(Vector* V, double k) {
    for (int i = 0; i < V->size; ++i) {
        V->elements[i] *= k;
    }
}

void print_vector(const Vector* v) {
    printf("size=%d, [", v->size);
    for (int i = 0; i < v->size; ++i) {
        printf("%lf ", v->elements[i]);
    }
    printf("]\n");
}

void print_matrix(const Matrix* M) {
    printf("rows=%d,cols=%d,\n[\n", M->rows, M->cols);
    for (int i = 0; i < M->rows; ++i) {
        printf("[");
        for (int j = 0; j < M->cols; ++j) {
            printf("%lf ", M->elements[i][j]);
        }
        printf("]\n");
    }
    printf("]\n");
}

Vector* matrix_col_max(Matrix* M){
    Vector* v = create_vector(M->cols);
    for (int i = 0; i < M->cols; ++i) {
        double max = M->elements[0][i];
        for (int j = 0; j < M->rows; ++j) {
            if (max < M->elements[j][i]) {
                max = M->elements[j][i];
            }
        }
        v->elements[i] = max;
    }
    return v; 
}

Vector* matrix_col_min(Matrix* M){
    Vector* v = create_vector(M->cols);
    for (int i = 0; i < M->cols; ++i) {
        double min = M->elements[0][i];
        for (int j = 0; j < M->rows; ++j) {
            if (min > M->elements[j][i]) {
                min = M->elements[j][i];
            }
        }
        v->elements[i] = min;
    }
    return v; 
}

void normalize_matrix_column_formula_1(Matrix* M, Vector* mayores, Vector* menores){
    for (int i = 0; i < M->rows; ++i) {
        for (int j = 0; j < M->cols; ++j) {
            M->elements[i][j] = (M->elements[i][j] - menores->elements[j]) / (mayores->elements[j] - menores->elements[j]);
        }
    }
}

Vector* matrix_col_vrz(Matrix* M){
    Vector* v = create_vector(M->cols);
    Vector* mean = matrix_col_mean(M);
    for (int i = 0; i < M->cols; ++i) {
        double sum = 0.0;
        for (int j = 0; j < M->rows; ++j) {
            sum += pow(M->elements[j][i] - mean->elements[i], 2);
        }
        v->elements[i] = sum / M->rows;
    }
    return v; 
}

Vector* matrix_col_std(Matrix* M){
    Vector* v = create_vector(M->cols);
    Vector* mean = matrix_col_mean(M);
    for (int i = 0; i < M->cols; ++i) {
        double sum = 0.0;
        for (int j = 0; j < M->rows; ++j) {
            sum += pow(M->elements[j][i] - mean->elements[i], 2);
        }
        v->elements[i] = sqrt(sum / (M->cols-1));
    }
    return v; 
}

void normalize_matrix_column_formula_2(Matrix* M, Vector* vrz, Vector* std){
    for (int i = 0; i < M->rows; ++i) {
        for (int j = 0; j < M->cols; ++j) {
            M->elements[i][j] = (M->elements[i][j] - vrz->elements[j]) / std->elements[j];
        }
    }
}

void normalize_vector_formula_1(Vector* v, float max, float min){
    for (int i = 0; i < v->size; ++i) {
        v->elements[i] = (v->elements[i] - min) / (max - min);
    }
}

float max_vector(const Vector* v){
    float max = v->elements[0];
    for (int i = 0; i < v->size; ++i) {
        if (max < v->elements[i]) {
            max = v->elements[i];
        }
    }
    return max;
}
float min_vector(const Vector* v){
    float min = v->elements[0];
    for (int i = 0; i < v->size; ++i) {
        if (min > v->elements[i]) {
            min = v->elements[i];
        }
    }
    return min;
}
float vrz_vector(const Vector* v){
    float sum = 0.0;
    for (int i = 0; i < v->size; ++i) {
        sum += v->elements[i];
    }
    return sum / v->size;
}
float std_vector(const Vector* v){
    float sum = 0.0;
    float vrz = vrz_vector(v);
    for (int i = 0; i < v->size; ++i) {
        sum += pow(v->elements[i] - vrz, 2);
    }
    return sqrt(sum / v->size);
}

void normalize_vector_formula_2(Vector* v, float vrz, float std){
    for (int i = 0; i < v->size; ++i) {
        v->elements[i] = (v->elements[i] - vrz) / std;
    }
}

#endif
