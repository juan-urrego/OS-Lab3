#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#ifndef EXECUTION_TIME_H
#define EXECUTION_TIME_H

void get_execution_time(struct timeval start, struct timeval end) {
    double elapsed_time = ((double)(end.tv_usec - start.tv_usec) /1000000 + (double)(end.tv_sec - start.tv_sec));
     printf("Tiempo de ejecución: %f microsegundos\n", elapsed_time);
}

#endif 