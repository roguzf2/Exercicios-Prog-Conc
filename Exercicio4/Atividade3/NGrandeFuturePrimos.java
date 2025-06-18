package Atividade3;

import java.util.concurrent.Callable;
import java.util.concurrent.ExecutionException;
import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;
import java.util.concurrent.Future;
import java.util.ArrayList;
import java.util.List;

//classe runnable
class NGrandeVerificadorPrimosCallable implements Callable<Integer> {
    private final long number;

    //construtor
    public NGrandeVerificadorPrimosCallable(long number) {
        this.number = number;
    }

    //método para execução
    @Override
    public Integer call() {
        if (number <= 1) {
            return 0;
        }
        if (number == 2) {
            return 1; 
        }
        if (number % 2 == 0) {
            return 0; 
        }

        for (long i = 3; i * i <= number; i += 2) {
            if (number % i == 0) {
                return 0; 
            }
        }
        return 1; 
    }
}

//classe do método main
public class NGrandeFuturePrimos {
    private static final int NTHREADS = 10;   // Tamanho do pool de threads
    private static final long PRIME_END = 1000000;  // Último número a verificar como primo

    public static void main(String[] args) {
        //cria um pool de threads (NTHREADS)
        ExecutorService executor = Executors.newFixedThreadPool(NTHREADS);

        //cria uma lista para armazenar referencias de chamadas assincronas
        List<Future<Integer>> primeList = new ArrayList<>();

        // Parte da verificação de primos
        for (long number = 1; number <= PRIME_END; number++) {
            Callable<Integer> checker = new NGrandeVerificadorPrimosCallable(number);
            Future<Integer> submit = executor.submit(checker);
            primeList.add(submit);
        }

        System.out.println("Tarefas de verificação de primos: " + primeList.size());

        //recupera os resultados e faz o somatório final
        long totalPrimes = 0;
        for (Future<Integer> future : primeList) {
            try {
                totalPrimes += future.get();
            } catch (InterruptedException | ExecutionException e) {
                e.printStackTrace();
            }
        }
        
        System.out.println("Quantidade de números primos no intervalo de 1 a " + PRIME_END + ": " + totalPrimes);
        executor.shutdown();
    }
}
