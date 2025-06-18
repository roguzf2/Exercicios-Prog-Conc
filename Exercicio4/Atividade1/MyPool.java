package Atividade1;

/* Disciplina: Programacao Concorrente */
/* Prof.: Silvana Rossetto */
/* Laboratório: 11 */
/* Codigo: Criando um pool de threads em Java */

import java.util.LinkedList;
import java.lang.Math;

//-------------------------------------------------------------------------------
// Classe que irá implementar um pool de threads
/*
 * Ela irá gerenciar um conjunto dado de threads permitindo adicionar tarefas na fila. Essas tarefas 
 * são objetos que implementam o Runnable, e eles serão distribuídos entre as threads.
 * As threads terão uma ordenação de encerramento, permitindo que todas as tarefas em execução ou aguardando na fila sejam concluídas antes de finalizar.
 * Irei comentar linha por linha para explicar o funcionamento.
 */
class FilaTarefas {
    // Declarando as variáveis
    private final int nThreads; // Numero de threads da pool
    private final MyPoolThreads[] threads; // As threads de fato, armazenadas em um array
    private final LinkedList<Runnable> queue; // A fila que irá guardar os objetos runnable
    private boolean shutdown; // Flag que faz o controle do encerramento das threads

    // Método construtor da classe que inicializa a pool de threads e a LinkedList
    public FilaTarefas(int nThreads) {
        // Atribuindo valores para as variáveis declaradas na classe
        this.shutdown = false; // Shutdown false pois aqui estamos inicializando
        this.nThreads = nThreads;
        queue = new LinkedList<Runnable>();
        threads = new MyPoolThreads[nThreads];

        // Fazemos um for para todas as threads serem inicializadas e colocadas na pool
        for (int i=0; i<nThreads; i++) {
            threads[i] = new MyPoolThreads();
            threads[i].start();
        } 
    }

    // Função que adiciona uma tarefa à fila para execução
    public void execute(Runnable r) {
      // Temos que adicionar tarefa por tarefa com exclusão mútua para não haver sobrescrita
        synchronized(queue) {
            if (this.shutdown) return;
            queue.addLast(r);
            queue.notify();
        }
    }
    
    // Função que encerra a pool de threads ordenadamente
    public void shutdown() {
      // Aqui também precisamos de exclusão mútua pelo mesmo motivo da Função execute
        synchronized(queue) {
            this.shutdown=true;
            queue.notifyAll();
        }
        // Realizando o join em todas as threads, e capturando o erro
        for (int i=0; i<nThreads; i++) {
          try { threads[i].join(); } catch (InterruptedException e) { return; }
        }
    }

    private class MyPoolThreads extends Thread {
       public void run() {
         Runnable r;
         while (true) {
           synchronized(queue) {
             while (queue.isEmpty() && (!shutdown)) {
               try { queue.wait(); }
               catch (InterruptedException ignored){}
             }
             if (queue.isEmpty()) return;   
             r = (Runnable) queue.removeFirst();
           }
           try { r.run(); }
           catch (RuntimeException e) {}
         } 
       } 
    } 
}
//-------------------------------------------------------------------------------

//--PASSO 1: cria uma classe que implementa a interface Runnable 
class Hello implements Runnable {
   String msg;
   public Hello(String m) { msg = m; }

   //--metodo executado pela thread
   public void run() {
      System.out.println(msg);
   }
}


class Primo implements Runnable {
  //...completar implementacao, recebe um numero inteiro positivo e imprime se esse numero eh primo ou nao
    String msg;

    public Primo(int n) {
        if (n <= 1) {
            msg = n + " não é primo.";
        } else if (n == 2) {
            msg = n + " é primo.";
        } else if (n % 2 == 0) {
            msg = n + " não é primo.";
        } else {
            for (long i = 3; i <= Math.sqrt(n); i += 2) {
              if (n % i == 0) {
                msg = n + " não é primo.";
                return;
              }
            }
          msg = n + " é primo.";
        }
    }

    // Método executado pela thread
    public void run() {
        System.out.println(msg);
    }
}

//Classe da aplicação (método main)
class MyPool {
    private static final int NTHREADS = 10;

    public static void main (String[] args) {
      //--PASSO 2: cria o pool de threads
      FilaTarefas pool = new FilaTarefas(NTHREADS); 
      
      //--PASSO 3: dispara a execução dos objetos runnable usando o pool de threads
      for (int i = 0; i < 100; i++) {
        // final String m = "Hello da tarefa " + i;
        // Runnable hello = new Hello(m);
        // pool.execute(hello);
        Runnable primo = new Primo(i);
        pool.execute(primo);
      }

      //--PASSO 4: esperar pelo termino das threads
      pool.shutdown();
      System.out.println("Terminou");
   }
}
