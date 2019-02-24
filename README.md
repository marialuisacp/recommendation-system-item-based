# recommendation-system-item-based
Collaborative movie recommendation (item based). Developed in C language.

## Introdução

O objetivo deste projeto foi a implementação de um algoritmo de recomendação para filmes usando modelo baseado em memória. São várias opções de implementação que irão impactar a qualidade das recomendações, incluindo opções para a normalização de dados, a computação da similaridade, selecção da vizinhança, a agregação, e redução da dimensionalidade.

## Desenvolvimento

### Linguagem
O projeto foi desenvolvido usando linguagem C.

### Item­based
Para a resolução deste projeto foi abordada a recomendação a partir de filtragem colaborativa baseada em itens. O algoritmo faz a cálculo da similaridade entre os itens, afim de encontrar itens semelhantes para prever as notas dos usuários.

### Pearson's correlation
Para computar a similaridade foi implementada a fórmula da correlação de Pearson, onde são passados dois vetores e retornados valores entre ­1 e 1, em que ­1 representa a que dois itens não são nada similares e 1 representa a similaridade máxima entre dois itens.

![Pearson's correlation](formula.png)

### Vizinhança de itens semelhantes
Após computar as semelhança foram considerados para as recomendações os 2 itens mais próximos, o tamanho pequeno da vizinhança se deu para que este cálculo não prejudicasse a performace do algoritmo.

### Os tipos de dados
Os dados lidos dos arquivos de entrada, foram organizados da seguinte forma:

```
typedef struct type_item{
  charid[12];
  intusers_evaluation[MAXUSERS];
  intratings;
} type_item;
```

Tipo de dados que guarda as informações importantes de um item. Observe que cada item
possui um vetor com as avaliações de todos usuários, para os usuários que não avaliaram o item é
inserido o valor 0, na posição do vetor referente àquele usuário.

![Esquema explicativo sobre os tipos de dados dos itens](img_01.png)

Cada usuário possui um número id (lido do arquivo de entrada), este número representa a
posição do vetor onde a avaliação do usuário irá entrar no vetor. Os usuários que não avaliaram
receberão 0 na posição correspondente do vetor.
```
typedef struct similar_item{
  charid[12];
  floatcorrelation;
  intrating;
} similar_item;
```

Este tipo de dados guarda informações relevantes aos itens semelhantes.

```
typedefstructuser_items{
  charid[12];
  intsize;
  type_item*item[3000];
} user_items;
```

Foi desenvolvido para otimizar a performace, um tipo de dados que guardasse todos os itens
que um usuário avaliou, pois assim não seria necessário percorrer toda a base para saber tal
informação. Foram usados ponteiros para os itens que o usuário avaliou.

- Estruturas de dados
Como o algoritmo foi feito usando filtragem colaborativa baseada em itens, os itens foram
tratados como principal "entidade", de forma que uma busca rápida e eficiente de um item se tornou
essencial do ponto de vista da performace. Ao ser lido dos arquivos de entrada, um item precisa ser
rapidamente inserido na estrutura de dados, caso este já esteja lá, é preciso adicionar uma avaliação
para ele.

Como isto foi feito? Bom, como visto acima, cada item possui um vetor que representa as
avaliações de todos os usuários. Ao receber uma avaliação de um usuário, é inserido no vetor a nota
na posição correspondente aquele usuário.

Os itens por sua vez, foram armazenados em uma lista de vetores (algo semelhante à tabelas
hash), onde foi feita a um certo tipo de indexação dos mesmos. Cada item possui também um número
id lido no arquivo de entrada. Os dois primeiros dígitos deste número, representam a posição na lista
que este item se encontra. Cada posição nesta lista, guarda um vetor com todos os itens que também
começam com o mesmos dígitos.

![Esquema explicativo sobre a estrutura de dados para armazenar items, a lista encadeada](img_02.png)

```
struct array_items {
  type_itemitem[ITEMS];
  intid;
  intsize;
  structarray_items*prox;
} array_items;
typedef struct array_itemslist;
```

## Evolução, Análise de complexidade e performance

A complexidade do algoritmo foi quase exponencial m², onde m representa o número de itens,
pois para cada item foi necessário percorrer todos os items para contruir a vizinhança de itens
semelhantes. Esta foi a parte do projeto que mais prejudicou a performace do cógido, assim, foi usado
um valor mínimo de similaridade, de forma que, dado um item, assim que o algoritmo encontrar dois
items com similaridade acima do valor mínimo, ele pára o cálculo dad similaridade. Este valor foi de
0,0 (lembrando que a escala da correlação de Pearson vai de ­1 a 1).
Esta decisão otimizou a complexidade e consequentemente a performace, pois o código
levava em torno de 15 a 10 minutos para executar, após esta mudança, foi para menos de 5 minutos de
execução.
