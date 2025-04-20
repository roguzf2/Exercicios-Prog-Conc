/* Disciplina: Programacao Concorrente */
/* Prof.: Silvana Rossetto */
/* Codigo: Comunicação entre threads usando variável compartilhada e exclusao mutua com bloqueio */

#include <stdio.h>
#include <stdlib.h> 
#include <pthread.h>
#include <math.h>
#include "timer.h"

long int qtdd_primos = 0; //variavel compartilhada entre as threads
int proximo_numero = 0;// Próximo número a ser verificado
long long int n_valores;
pthread_mutex_t mutex; //variavel de lock para exclusao mutua

int ehPrimo(long long int n) {
    // retorna 1 é primo, 0 não é primo
    int i;
    if (n<=1) return 0;
    if (n==2) return 1;
    if (n%2==0) return 0;
    for (i=3; i<sqrt(n)+1; i+=2)
    if(n%i==0) return 0;
    return 1;
 }

//funcao executada pelas threads
void *ExecutaTarefa (void *arg) {
    // long int id = (long int) arg;
    // printf("Thread : %ld esta executando...\n", id);

    for (int i=0; i<n_valores; i++) {
    
        long int num;

        //--entrada na SC
        pthread_mutex_lock(&mutex);
    
        // Caso onde estamos passando do n_valores, o que significa que ja teremos lido todos o N
        if (proximo_numero >= n_valores) {
            pthread_mutex_unlock(&mutex);
            break;  // Saindo do while
        }

    num = proximo_numero++;
    pthread_mutex_unlock(&mutex);

    //--SC (seção critica) 
    if (ehPrimo(num)) {
        pthread_mutex_lock(&mutex);
        // printf("Thread %ld encontrou primo: %ld\n", id, num);
        qtdd_primos++;
        //--saida da SC
        pthread_mutex_unlock(&mutex);
    }
     // Esse processo de lock e unlock custa um pouco, demorando mais para realizar o processo

  }
//   printf("Thread : %ld terminou!\n", id);
  pthread_exit(NULL);
}

//fluxo principal
int main(int argc, char *argv[]) {
   pthread_t *tid; //identificadores das threads no sistema
   int nthreads; //qtde de threads (passada linha de comando)
   double inicio, fim, delta, delta_inicializacao, total=0;

   GET_TIME(inicio);
   //--le e avalia os parametros de entrada
   if(argc<3) {
      printf("Digite: %s <numero de threads> <n valores a serem lidos>\n", argv[0]);
      return 1;
   }
   nthreads = atoi(argv[1]);
   n_valores = atoi(argv[2]);

   //--aloca as estruturas
   tid = (pthread_t*) malloc(sizeof(pthread_t)*nthreads);
   if(tid==NULL) {puts("ERRO--malloc"); return 2;}

   //--inicilaiza o mutex (lock de exclusao mutua)
   pthread_mutex_init(&mutex, NULL);

   //--cria as threads
   for(long int t=0; t<nthreads; t++) {
     if (pthread_create(&tid[t], NULL, ExecutaTarefa, (void *)t)) {
       printf("--ERRO: pthread_create()\n"); exit(-1);
     }
   }

   GET_TIME(fim);
   delta_inicializacao = fim - inicio;

   GET_TIME(inicio);
   //--espera todas as threads terminarem
   for (int t=0; t<nthreads; t++) {
     if (pthread_join(tid[t], NULL)) {
         printf("--ERRO: pthread_join() \n"); exit(-1); 
     } 
   }
   GET_TIME(fim);
   delta = fim - inicio;
   printf("Valor_de_N, Nthreads, Inicialização, Busca_de_Primos, Finalização, Total\n");
   printf("%lld, %d, ", n_valores, nthreads);
   printf("%lf, %lf, ", delta_inicializacao, delta);
   total = delta;

   GET_TIME(inicio);
   //--finaliza o mutex -> como se fosse o free()
   pthread_mutex_destroy(&mutex);
   free(tid);
   GET_TIME(fim);
   delta = fim - inicio;
   printf("%lf, ", delta);
   total += delta + delta_inicializacao;
   printf("%lf\n", total);

//    printf("\nTotal de primos entre 0 e %lld: %ld\n", n_valores, qtdd_primos);

   return 0;
}
