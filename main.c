#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "./tools/matrix.c"
#include "./tools/execution_time.c"
#include "./tools/parametros.c"
#include "./operations/matrix_scalar.c"
#include "./operations/normalize_formula1.c"
#include "./operations/normalize_formula2.c"
#include "./tools/file.c"
#include "./operations/matrices_sum.c"
#include "./operations/matrices_dot.c"
#include "./tools/validations.c"
#include "./operations/column_mean.c"
#include "./operations/column_variance.c"
#include "./tools/minorValue.c"

int select_operation(int o, int f, int c, int r, int s, double e, int n, int file);

int main(int argc, char *argv[])
{
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
int select_operation(int o, int f, int c, int r, int s, double e, int n, int file)
{
    switch (o)
    {
    case 1:
        matrix_columns_mean(f, c, n, file);
        return 0;
    case 2:
        matrix_columns_variance(f, c, n, file);
        return 0;
    case 3:
        Matrix *M = create_matrix_from_file("op1.txt", f, c);
        Vector *V = matrix_col_std(M);
        print_matrix(M);
        print_vector(V);
        return 0;
    case 4:
        return 0;
    case 5:
        add_two_matrices(f, c, r, s, n, file);
        return 0;
    case 6:
        dot_two_matrices(f, c, r, s, n, file);
        return 0;
    case 7:
        multiply_matrix_by_scalar(f, c, e, n, file);
        return 0;
    case 8:
        main_normalize_formula_1(f, c, n, file);
        return 0;
    case 9:
        main_normalize_formula_2(f, c, n, file);
        return 0;
    default:
        printf("Operacion no valida %d ", o);
        exit(EXIT_FAILURE);
    }
}
