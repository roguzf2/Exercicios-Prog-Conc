#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <semaphore.h>
#include <unistd.h>
#include <math.h>

// Variáveis globais
int *buffer;
int N, M, C;
int in = 0, out = 0;
int totalConsumido = 0;

sem_t slotCheio, slotVazio;
sem_t semBuffer, semContagem;

int *contagemPrimos; // por consumidor

//imprime o buffer
void printBuffer(int buffer[], int tam) {
    for (int i = 0; i < tam; i++) 
        printf("%d ", buffer[i]); 
    puts("");
}

// Função para verificar se é primo
int ehPrimo(long long int n) {
    if (n <= 1) return 0;
    if (n == 2) return 1;
    if (n % 2 == 0) return 0;
    for (int i = 3; i <= sqrt(n); i += 2)
        if (n % i == 0) return 0;
    return 1;
}

// Função de inserção no buffer
void insere(int item, int id) {
    sem_wait(&slotVazio); //aguarda slot vazio para inserir
    sem_wait(&semBuffer); //exclusao mutua entre produtores
    buffer[in] = item;
    in = (in + 1) % M;
    printf("Prod[%d]: inseriu %d\n", id, item);
    printBuffer(buffer, M); //para log apenas
    sem_post(&semBuffer);
    sem_post(&slotCheio);
}

// Função de retirada do buffer
int retira(int id) {
    int item;
    sem_wait(&slotCheio); //aguarda slot vazio para inserir
    sem_wait(&semBuffer); //exclusao mutua entre produtores
    item = buffer[out];
    buffer[out] = 0;
    out = (out + 1) % M;
    printf("Cons[%d]: retirou %d\n", id, item);
    printBuffer(buffer, M); //para log apenas
    sem_post(&semBuffer);
    sem_post(&slotVazio);
    return item;
}

// Thread produtora
void *produtor(void *arg) {
  int id = *(int *)(arg);
  free(arg);
    for (int i = 1; i <= N; i++) {
        insere(i, id);
    }
    pthread_exit(NULL);
}

// Thread consumidora
void *consumidor(void *arg) {
    int id = *((int *)arg);
    free(arg);

    while (1) {
        sem_wait(&semContagem);
        if (totalConsumido >= N) {
            sem_post(&semContagem);
            break;
        }
        totalConsumido++;
        sem_post(&semContagem);

        int num = retira(id);
        if (ehPrimo(num)) {
            contagemPrimos[id]++;
        }
    }
    pthread_exit(NULL);
}

// Função principal
int main(int argc, char *argv[]) {
    if (argc != 4) {
        printf("Digite: %s <sequencia de numeros inteiros> <tamanho do buffer> <qtdde de threads>\n", argv[0]);
        exit(1);
    }

    N = atoi(argv[1]);
    M = atoi(argv[2]);
    C = atoi(argv[3]);

    if (N <= 0 || M <= 0 || C <= 0) {
        printf("Erro: N, M e C devem ser inteiros positivos.\n");
        exit(1);
    }

    buffer = malloc(sizeof(int) * M);
    contagemPrimos = calloc(C, sizeof(int));

    // Inicializações dos semáforos
    sem_init(&slotCheio, 0, 0);
    sem_init(&slotVazio, 0, M);
    sem_init(&semBuffer, 0, 1);   // semáforo binário para o buffer
    sem_init(&semContagem, 0, 1); // semáforo binário para totalConsumido

    pthread_t tidProdutor;
    pthread_t consumidores[C];

    int *idProd = malloc(sizeof(int));
    *idProd = 0;
    pthread_create(&tidProdutor, NULL, produtor, idProd);


    for (int i = 0; i < C; i++) {
        int *id = malloc(sizeof(int));
        *id = i;
        pthread_create(&consumidores[i], NULL, consumidor, id);
    }

    pthread_join(tidProdutor, NULL);
    for (int i = 0; i < C; i++) {
        pthread_join(consumidores[i], NULL);
    }

    // Verifica vencedor 
    // (tem que ser dps do join para poder resgatar os valores retornados e verificar quem fez mais verificações)
    int totalPrimos = 0, vencedor = 0;
    for (int i = 0; i < C; i++) {
        totalPrimos += contagemPrimos[i];
        if (contagemPrimos[i] > contagemPrimos[vencedor])
            vencedor = i;
    }

    printf("\nQuantidade de primos encontrados: %d\n", totalPrimos);
    printf("Thread consumidora vencedora: %d (com %d primos encontrados)\n", vencedor, contagemPrimos[vencedor]);

    // Liberando os recursos
    free(buffer);
    free(contagemPrimos);
    sem_destroy(&slotCheio);
    sem_destroy(&slotVazio);
    sem_destroy(&semBuffer);
    sem_destroy(&semContagem);

    return 0;
}
