#ifndef MINOR_VALUE_H
#define MINOR_VALUE_H

int minor_value(int n_threads, int n_rows){
    if(n_threads>n_rows){
        return n_rows;
    }else{
        return n_threads;
    }
}

#endif