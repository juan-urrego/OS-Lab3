#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "./tools/matrix.c"
#include "./tools/parametros.c"
#include "./tools/file.c"
#include "./tools/validations.c"
#include "./operations/1_mean_column.c"
#include "./operations/2_variance_column.c"
#include "./operations/5_sum_matrix.c"
#include "./operations/6_dot_matrix.c"
#include "./operations/7_scalar_matrix.c"
#include "./operations/8_normalize_formula1.c"
#include "./operations/9_normalize_formula2.c"
#include "./tools/minorValue.c"

int main(int argc, char *argv[]) {
    int o = 0, f = 0, c = 0, r = 0, s = 0, n = 0, file = 0;
    char p[200] = "";
    double e = 0;
    get_params(argc, argv, &o, &f, &c, &e, &r, &s, p, &n);
    if (strcmp(p, "") == 0)
    {
        select_operation(o, f, c, r, s, e, n, file);
    }
    else
    {
        file = 1;
        read_file(&o, &f, &c, &e, &r, &s, p, &n);
        select_operation(o, f, c, r, s, e, n, file);
    }
    return 0;
}

int select_operation(int o, int f, int c, int r, int s, double e, int n, int file) {
    switch (o)
    {
    case 1:
        calculate_column_matrix_mean(f, c, n, file);
        return 0;
    case 2:
        calculate_column_matrix_variance(f, c, n, file);
        return 0;
    case 3:
        calculate_matrix_columns_standard_deviation(f, c, n, file);
        return 0;
    case 4:
        calculate_matrix_columns_min_max(f, c, n, file);
        return 0;
    case 5:
        calculate_sum_two_matrix(f, c, r, s, n, file);
        return 0;
    case 6:
        calculate_dot_two_matrix(f, c, r, s, n, file);
        return 0;
    case 7:
        calculate_matrix_by_scalar(f, c, e, n, file);
        return 0;
    case 8:
        calculate_normalize_formula1(f, c, n, file);
        return 0;
    case 9:
        calculate_normalize_formula2(f, c, n, file);
        return 0;
    default:
        printf("Operacion no valida %d ", o);
        exit(EXIT_FAILURE);
    }
}
