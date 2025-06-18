# Exercicios-Prog-Conc

Cada exercício possui sua própria pasta, e irei adicionando eles aqui com o passar do tempo

Como rodar códigos do exercício 2:

no diretório do Exercicio 2 basta:

* gcc primos-lock.c -o primos -Wall -lm
* ./script.sh (isso irá gerar a tabela para n = 1000)
* modificar o script.sh para 1000000
* rodar ./script.sh
* depois basta rodar o arquivo analise.ipynb para observar os gráficos e outros resultados, como eficiência, aceleração e tempo médio

Como rodar códigos do exercício 3:

no diretório do Exercicio 3 basta:

* gcc pc_primos.c -o pc_primos_saida -Wall -lm
* ./pc_primos_saida `<sequencia de numeros inteiros>` `<tamanho do buffer>` `<qtdde de threads>`
* Mantive os logs, mas podem ser comentados caso seja necessário nas funções de inserção e retirada

Como rodar códigos do exercício 4:

no diretório do Exercicio 4 basta:

* No terminal, continue no diretório do exercício 4 e rode:
  * java Atividade1.MyPool (para a Atividade 1)
  * java Atividade3.FuturePrimos (para Atividade3 item 2)
  * java Atividade3.NGrandeFuturePrimos (para Atividade3 item 3)
