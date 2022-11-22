#include<stdio.h>
#include "omp.h"
#define NUM_THREADS 18
int main()
{
        double t2,t1,tiempo;
        omp_set_num_threads (NUM_THREADS);
        t1 = omp_get_wtime();
#pragma omp parallel
{
        int ID = omp_get_thread_num();
        printf("Procesadores(%d)",ID);
        printf("Multiples(%d)",ID);
        printf("En accion(%d) ---%d\n",ID);
}
        t2 = omp_get_wtime();
        tiempo = t2-t1;
        printf("tomo (%lf) segundos\n",(tiempo));
}
