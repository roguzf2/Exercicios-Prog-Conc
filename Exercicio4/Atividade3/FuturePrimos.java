package Atividade3;

/* Disciplina: Programacao Concorrente */
/* Prof.: Silvana Rossetto */
/* Laboratório: 11 */
/* Codigo: Exemplo de uso de futures */
/* -------------------------------------------------------------------*/

import java.util.concurrent.Callable;
import java.util.concurrent.ExecutionException;
import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;
import java.util.concurrent.Future;
import java.util.ArrayList;
import java.util.List;

//classe runnable
class VerificadorPrimosCallable implements Callable<String> {
    private final long n_primos;

    //construtor
    public VerificadorPrimosCallable(long n_primos) {
        this.n_primos = n_primos;
    }

    //método para execução
    @Override
    public String call() {
        if (n_primos <= 1) {
            return n_primos + " não é primo.";
        } 
        if (n_primos == 2) {
            return n_primos + " é primo.";
        } 
        if (n_primos % 2 == 0) {
            return n_primos + " não é primo.";
        }

        for (long i = 3; i <= Math.sqrt(n_primos); i += 2) {
            if (n_primos % i == 0) {
                return n_primos + " não é primo.";
            }
        }
        return n_primos + " é primo.";
    }
}

//classe original para somar números
class MyCallable implements Callable<Long> {
    public MyCallable() {}

    @Override
    public Long call() throws Exception {
        long s = 0;
        for (long i = 1; i <= 100; i++) {
            s++;
        }
        return s;
    }
}

//classe do método main
public class FuturePrimos {
    private static final int n_primos = 3;
    private static final int NTHREADS = 10;   
    private static final long PRIME_START = 1; 
    private static final long PRIME_END = 100;  

    public static void main(String[] args) {
        //cria um pool de threads (NTHREADS)
        ExecutorService executor = Executors.newFixedThreadPool(NTHREADS);

        //cria uma lista para armazenar referencias de chamadas assincronas
        List<Future<Long>> sumList = new ArrayList<>();
        List<Future<String>> primeList = new ArrayList<>();

        // Parte da soma (não removi)
        for (int i = 0; i < n_primos; i++) {
            Callable<Long> worker = new MyCallable();
            Future<Long> submit = executor.submit(worker);
            sumList.add(submit);
        }

        // Parte da verificação de primos
        for (long n_primos = PRIME_START; n_primos <= PRIME_END; n_primos++) {
            Callable<String> checker = new VerificadorPrimosCallable(n_primos);
            Future<String> submit = executor.submit(checker);
            primeList.add(submit);
        }

        System.out.println("Tarefas de soma: " + sumList.size());
        System.out.println("Tarefas de verificação de primos: " + primeList.size());

        //recupera os resultados e faz o somatório final
        long sum = 0;
        for (Future<Long> future : sumList) {
            try {
                sum += future.get(); // Bloqueia se a computação não tiver terminado
            } catch (InterruptedException | ExecutionException e) {
                e.printStackTrace();
            }
        }

        //recupera os resultados e faz o somatório final
        for (Future<String> future : primeList) {
            try {
                System.out.println(future.get()); //bloqueia se a computação nao tiver terminado
            } catch (InterruptedException | ExecutionException e) {
                e.printStackTrace();
            }
        }

        System.out.println("Soma total: " + sum);
        executor.shutdown();
    }
}
