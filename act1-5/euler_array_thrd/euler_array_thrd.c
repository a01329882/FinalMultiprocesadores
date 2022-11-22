#include <stdio.h>
#include <omp.h>
#include <math.h>
#define NUM_THREADS 6

double iteracion(long N);

int main(){

    omp_set_num_threads(NUM_THREADS);

    double t1,t2; 
    double e1,e2,e3,e4,e5,e6;

    t1 = omp_get_wtime();

    #pragma omp parallel
    {    
     #pragma omp sections
    {
       #pragma omp section
         e1 = (double)iteracion(50000);
       #pragma omp section
         e2 = (double)iteracion(50000);
       #pragma omp section
         e3 = (double)iteracion(50000);
       #pragma omp section
         e4 = (double)iteracion(50000);
       #pragma omp section
         e5 = (double)iteracion(50000);
       #pragma omp section
         e6 = (double)iteracion(50000);
    }
}   
    t2 = omp_get_wtime();

    printf("e1: %f\n",e1);
    printf("e2: %f\n",e2);
    printf("e3: %f\n",e3);
    printf("e4: %f\n",e4);
    printf("e5: %f\n",e5);
    printf("e6: %f\n",e6);

    printf("tomo (%f) segundos\n",(t2-t1));
    
    return (0);
    
}
double iteracion(long N){

    printf("Numero de pasos:%d Atendido por thread:%d\n", N, omp_get_thread_num());

    double h,ab;
    double t[N],w[N];
    double w0=0.5,a=0,b=2;
    int i;

    h=(b-a)/N;
    w[0] = w0;
    t[0] = a;
    for(i=0;i<N;i++){
        t[i]=a+(h*i);
        w[i]=w[i-1]+h*(1+t[i-1]*t[i-1]-w[i-1]);
    }

    return w[N-1];

}
