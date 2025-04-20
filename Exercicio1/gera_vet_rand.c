/*
Programa auxiliar para gerar um vetor de floats 
*/

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define MAX 1000 //valor maximo de um elemento do vetor
//descomentar o define abaixo caso deseje imprimir uma versao do vetor gerado no formato texto
#define TEXTO 

int main(int argc, char*argv[]) {
   float *vetor; //vetor que será gerada
   long int n; //qtde de elementos do vetor (IGUAL PARA OS 2 VETORES)
   float elem; //valor gerado para incluir no vetor
   double produto_interno = 0.0; //soma total dos elementos gerados (!! SUBTITUIR POR PROD INTERNO !!)
   int fator; //fator multiplicador para gerar números negativos
   FILE * descritorArquivo; //descritor do arquivo de saida
   size_t ret; //retorno da funcao de escrita no arquivo de saida
   float *vetor_2; //vetor que será gerada
   float elem_2; //valor gerado para incluir no vetor

   //recebe os argumentos de entrada
   if(argc < 3) {
      fprintf(stderr, "Digite: %s <dimensao> <nome arquivo saida>\n", argv[0]);
      return 1;
   }
   n = atoi(argv[1]);

   //aloca memoria para o vetor
   vetor = (float*) malloc(sizeof(float) * n);
   if(!vetor) {
      fprintf(stderr, "Erro de alocao da memoria do vetor\n");
      return 2;
   }

   //aloca memoria para o vetor_2
   vetor_2 = (float*) malloc(sizeof(float) * n);
   if(!vetor_2) {
      fprintf(stderr, "Erro de alocao da memoria do vetor 2\n");
      return 2;
   }

   //preenche o vetor com valores float aleatorios
   srand(time(NULL));

   // Vetor 1
   fator = 1; // Reiniciando o fator para o 1o vetor
   for(long int i=0; i<n; i++) {
        elem = (rand() % MAX)/3.0 * fator;
        vetor[i] = elem;
        fator*=-1;
   }

   // Vetor 2
   fator = -1; // Começa com fator diferente para gerar números diferentes
   for(long int i=0; i<n; i++) {
      elem_2 = (rand() % MAX)/3.0 * fator;
      vetor_2[i] = elem_2;
      fator*=-1;
 }

   // Calcula o produto interno
   for(long int i=0; i<n; i++) {
      produto_interno += (double)vetor[i] * (double)vetor_2[i];
   }

   //imprimir na saida padrao os vetores gerados e o produto interno
   #ifdef TEXTO
   fprintf(stdout, "%ld\n", n);
   for(long int i=0; i<n; i++) {
      fprintf(stdout, "%f ", vetor[i]);
   }
   fprintf(stdout, "\n");
   for(long int i=0; i<n; i++) {
      fprintf(stdout, "%f ", vetor_2[i]);
   }
   fprintf(stdout, "\n");
   fprintf(stdout, "Produto interno: %lf\n", produto_interno);
   #endif

   //escreve o vetor no arquivo
   //abre o arquivo para escrita binaria
   descritorArquivo = fopen(argv[2], "wb");
   if(!descritorArquivo) {
      fprintf(stderr, "Erro de abertura do arquivo\n");
      return 3;
   }
   //escreve a dimensao
   ret = fwrite(&n, sizeof(long int), 1, descritorArquivo);
   //escreve os elementos do vetor
   ret = fwrite(vetor, sizeof(float), n, descritorArquivo);
   ret = fwrite(vetor_2, sizeof(float), n, descritorArquivo);
   if(ret < n) {
      fprintf(stderr, "Erro de escrita no  arquivo\n");
      return 4;
   }
   //escreve o somatorio (!! SUBTITUIR POR PROD INTERNO !!)
   ret = fwrite(&produto_interno, sizeof(double), 1, descritorArquivo);

   //finaliza o uso das variaveis
   fclose(descritorArquivo);
   free(vetor);
   free(vetor_2);
   return 0;
} 
