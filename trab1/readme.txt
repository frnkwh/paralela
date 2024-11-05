Para compilar o programa deve ser rodado o 'make' no diretório corrente.
Se quiser compilar apenas o executável da parte A (chamado de bsearch_single)
'make a' e da parte B (chamado de bsearch_multiple) 'make b'.

Para rodar o código, siga as instruções abaixo:

 Parte A:
  ./bsearch_single input (tamanho do vetor de input) nThreads (numero de threads)

 Parte B:
  ./bsearch_multiple input (tamanho do vetor de input) nThreads (numero de threads)

Em ambas as partes, substitua input pelo nome do arquivo ou caminho do vetor de entrada, (tamanho do vetor de input) pelo tamanho do vetor a ser processado, e nThreads pelo número de threads que deseja utilizar.

Os scripts criados foram:
 - gera_tabela_a.py: roda o programa para cada caso de teste exigido (de 1
 a 8 threads para os tamanhos 1, 2, 4, 8 e 16 milhões) e gera uma tabela em
 formato csv contendo os resultados de cada um execução.
 - gera_tabela_b.py:
 - gera_graficos.py: gera os gráficos do tempo de execução e do número de
 operações realizadas baseado em uma tabela csv, essa, que é gerada pelo
 script gera_tabela_a.py ou gera_tabela_b.py.


Sobre o desempenho ruim da parte A do trabalho:

Paralelizar uma busca binária não faz muito sentido porque a busca binária é
uma operação sequencial por natureza. Em cada etapa, a próxima decisão depende
do resultado da etapa anterior, o que significa que não é possível dividir a
tarefa de forma eficiente entre múltiplas threads ou processos.

A tentativa de paralelizar uma busca binária introduz overhead e complexidade
desnecessária, sem ganhos significativos de performance. Como a busca binária
já é eficiente com complexidade O(log n), a paralelização pode até piorar
o desempenho em vez de melhorá-lo.

E foi exatamente o que aconteceu nesse caso. Nossa tentativa foi baseada na
ideia de segmentar o vetor em n partes, considerando que há n threads, na qual
cada thread fará uma busca no seu segmento. Além de não fazer muito sentido do
ponto de vista teórico, vimos na prática que a perda de desempenho foi bastante
considerável.
