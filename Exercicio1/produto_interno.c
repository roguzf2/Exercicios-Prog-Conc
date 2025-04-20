#include <stdio.h>
#include <stdlib.h> 
#include <pthread.h> 
#include <math.h>

// Definindos variáveis globais
long int n;          // Tamanho do vetor
float *vet;          // Primeiro vetor
float *vet_2;        // Segundo vetor
int nthreads;        // Número de threads
double produto_esperado; // Produto interno esperado (lido do arquivo)

// Utilizei estrutura para facilitar um pouco, mas sinto que não era 100% necessária aqui
// Definindo uma estrutura para passar argumentos para as threads
typedef struct {
    long int inicio;
    long int fim;
} ThreadArgs;

// Função executada pelas threads
void *ProdutoInternoVetor(void *arg) {
    ThreadArgs *args = (ThreadArgs *)arg;
    double *produto_local = (double *)malloc(sizeof(double));
    *produto_local = 0;

    for(long int i = args->inicio; i < args->fim; i++) {
        *produto_local += (double)vet[i] * (double)vet_2[i];
    }

    pthread_exit((void*)produto_local);
}

int main(int argc, char *argv[]) {
    FILE *arq;
    pthread_t *tid_sistema;
    ThreadArgs *args;
    double produto_interno_global = 0;
    double *produto_local;

    // Validando a entrada
    if(argc < 3) { 
        printf("Use: %s <arquivo de entrada> <numero de threads>\n", argv[0]); 
        exit(-1); 
    }

    // Abrindo o arquivo
    arq = fopen(argv[1], "rb");
    if(arq == NULL) { 
        printf("--ERRO: fopen()\n"); 
        exit(-1); 
    }

    // Lendo o tamanho do vetor
    if(fread(&n, sizeof(long int), 1, arq) != 1) {
        printf("Erro ao ler dimensao do vetor\n");
        exit(-1);
    }

    // Alocando e lendo os vetores
    vet = (float*)malloc(sizeof(float) * n);
    vet_2 = (float*)malloc(sizeof(float) * n);
    if(vet == NULL || vet_2 == NULL) {
        printf("--ERRO: malloc()\n"); 
        exit(-1);
    }

    if(fread(vet, sizeof(float), n, arq) != n ||
       fread(vet_2, sizeof(float), n, arq) != n ||
       fread(&produto_esperado, sizeof(double), 1, arq) != 1) {
        printf("Erro ao ler vetores ou produto esperado\n");
        exit(-1);
    }

    // Definindo o número de threads
    nthreads = atoi(argv[2]);
    if(nthreads > n) nthreads = n;

    // Alocando estruturas para threads
    tid_sistema = (pthread_t *)malloc(sizeof(pthread_t) * nthreads);
    args = (ThreadArgs *)malloc(sizeof(ThreadArgs) * nthreads);
    if(tid_sistema == NULL || args == NULL) {
        printf("--ERRO: malloc()\n"); 
        exit(-1);
    }

    // Dividindo o trabalho entre threads
    long int bloco = n / nthreads;
    for(int i = 0; i < nthreads; i++) {
        args[i].inicio = i * bloco;
        // Se o i for a qtde de threads - 1, significa q o fim do bloco eh o fim do vetor!!
        args[i].fim = (i == nthreads-1) ? n : (i+1) * bloco;
        
        if(pthread_create(&tid_sistema[i], NULL, ProdutoInternoVetor, (void*)&args[i])) {
            printf("--ERRO: pthread_create()\n"); 
            exit(-1);
        }
    }

    // Aguardando as threads e acumulando os resultados
    for(int i = 0; i < nthreads; i++) {
        if(pthread_join(tid_sistema[i], (void **)&produto_local)) {
            printf("--ERRO: pthread_join()\n"); 
            exit(-1);
        }
        produto_interno_global += *produto_local;
        free(produto_local);
    }

    // Calculando a variação relativa
    double diferenca = fabs(produto_esperado - produto_interno_global);
    double variacao_relativa = (produto_esperado != 0) ? diferenca / fabs(produto_esperado) : 0.0;
    
    // Printando os resultados
    printf("\nResultados:\n");
    printf("Produto interno calculado (concorrente): %.15f\n", produto_interno_global);
    printf("Produto interno esperado (sequencial):   %.15f\n", produto_esperado);
    printf("Diferença absoluta:                      %.15f\n", fabs(produto_esperado - produto_interno_global));
    printf("Variação relativa (e):                   %.3e (%.3e%%)\n", 
      variacao_relativa, variacao_relativa * 100);

    // Liberando a memória
    free(vet);
    free(vet_2);
    free(tid_sistema);
    free(args);
    fclose(arq);

    return 0;
}

// percebi que quanto mais threads eu coloco, menor é o erro (por mais que seja muito baixo já)
// sinto que temos pouco erro pois temos uma situação onde cada thread calcula uma parte independente do produto
// se eu colocar um vetor com muitos elementos pode ser que encontre um erro relevante