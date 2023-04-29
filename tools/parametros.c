#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <getopt.h>
#include <locale.h>
#include <string.h>

void get_params(int argc, char *argv[], int *c, double *e, int *f, int *n, int *o, char *p, int *r, int *s ){
    int opt;
    setlocale(LC_ALL, "");
    while((opt=getopt(argc,argv,"o:f:c:e:r:s:p:n:"))!=-1){
        switch(opt){
            case 'c':
                *c=atoi(optarg);
                break;
            case 'e':
                *e=atoi(optarg);
                break;
            case 'f':
                *f=atoi(optarg);
                break;
            case 'n':
                *n=atoi(optarg);
                break;
            case 'o':
                *o=atoi(optarg);
                break;
            case 'p':
                strncpy(p, optarg, 200);
                p[sizeof(p)] = '\0';
                break;
            case 'r':
                *r=atoi(optarg);
                break;
            case 's':
                *s=atoi(optarg);
                break;
            default:
               fprintf(stderr, "Usage: %s -o <operation> [-f <value>] [-c <value>] [-e <value>] [-r <value>] [-s <value>] [-p <value>]\n", argv[0]);
                exit(EXIT_FAILURE);
        }
    }

}