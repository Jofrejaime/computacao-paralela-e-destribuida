#include<stdio.h>
#include<stdlib.h>
#include<omp.h>

#define TOTALSIZE 1000
#define NUMITER 200

#define f(x,y) ((x+y)/2.0)

int main(int argc, char *argv[]) {

  int i, iter;

  double *V1 = (double *) malloc(TOTALSIZE * sizeof(double));
  double *V2 = (double *) malloc(TOTALSIZE * sizeof(double));

  double *read, *write;

  /* inicialização */
  for(i = 0; i < TOTALSIZE; i++) {
    V1[i] = i;
  }

  /* apontadores iniciais */
  read = V1;
  write = V2;

  for(iter = 0; iter < NUMITER; iter++) {

    #pragma omp parallel for
    for(i = 0; i < TOTALSIZE-1; i++) {
      write[i] = f(read[i], read[i+1]);
    }

    /* 🔁 troca os vetores */
    double *temp = read;
    read = write;
    write = temp;
  }

  /* ⚠️ no final, o resultado está em 'read' */
  printf("Output:\n"); 
  for(i = 0; i < TOTALSIZE; i++) {
    printf("%4d %f\n", i, read[i]);
  }

  free(V1);
  free(V2);
}