# Compilação

Para compilar o programa, rode `make` no diretório corrente. Para compilar apenas o executável da parte A (chamado de `bsearch_single`), use `make a`. Para a parte B (chamado de `bsearch_multiple`), use `make b`.

## Scripts Criados

- **gera_tabela_a.py**: Executa o programa para cada caso de teste exigido (de 1 a 8 threads para os tamanhos 1, 2, 4, 8 e 16 milhões) e gera uma tabela em formato CSV contendo os resultados de cada execução. Esse script realiza os testes sobre o executável `bsearch_single`, que corresponde à parte A.

- **gera_tabela_b.py**: Faz o mesmo que o `gera_tabela_a.py`, mas utiliza o executável `bsearch_multiple`, que corresponde à parte B.

- **gera_graficos.py**: Gera gráficos do tempo de execução e do número de operações realizadas, baseando-se em uma tabela CSV. Esta tabela é gerada pelo script `gera_tabela_a.py` ou `gera_tabela_b.py`.

## Sobre o Desempenho Ruim da Parte A

Paralelizar uma busca binária não é ideal, pois a busca binária é naturalmente uma operação sequencial. Em cada etapa, a próxima decisão depende do resultado da etapa anterior, o que impede uma divisão eficiente da tarefa entre múltiplas threads ou processos.

A tentativa de paralelizar uma busca binária adiciona sobrecarga e complexidade desnecessária, sem ganhos significativos de desempenho. Como a busca binária já é eficiente, com complexidade O(log n), a paralelização pode até piorar o desempenho em vez de melhorá-lo.

Foi exatamente isso que aconteceu neste caso. A tentativa se baseou na ideia de segmentar o vetor em *n* partes (considerando *n* threads), onde cada thread faria uma busca em seu segmento. Além de não fazer muito sentido do ponto de vista teórico, observamos na prática que houve uma perda de desempenho significativa.

