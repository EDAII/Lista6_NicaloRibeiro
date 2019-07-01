//Lista 6 - ED2
//Nícalo Ribeiro - 16/0016169

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define NAOVISITADO 0
#define VISITADO 1

static int menorDistancia = -1;
static int *menorCaminho;
double tempoExec = 0, tempoMontagem = 0;

typedef struct TipoNoAdj *TipoApontador;

typedef struct TipoNoAdj
{

    int cidade;
    int distancia;
    TipoApontador prox;

} TipoNoAdj;

typedef struct
{

    TipoApontador primeiro;

} TipoListaAdj;

typedef struct
{

    TipoListaAdj listaAdj;
    int visitou;

} TipoVertice;

void FGVazio(TipoVertice *, int);
void insereAresta(TipoVertice *, int, int, int);
TipoVertice *montaGrafo(int *, char *);
void freeGrafo(TipoVertice *, int);
void buscaDFS(TipoVertice *, int, int, int, int *, int);
int verificaCidade(TipoVertice *, int);
void copiaVetor(int *, int *, int);
void imprimeCaminho(TipoVertice *, int *, int);
void buscaMelhorTrajeto(TipoVertice *, int, int);

int main()
{
    int opt, origem, numCidades;

    TipoVertice *vertice;
    system("clear");

    char *entradaArq = malloc(sizeof(char) * 1024);
    printf("\nDigite o Arquivo de entrada < ARQUIVOS PADRÕES DISPONÍVEIS : 'entrada.txt'-'entrada2.txt'-'entrada3.txt' > : \n");
    scanf("%s", entradaArq);
    printf("Digite a origem :\n");
    scanf("%d", &origem);
    vertice = montaGrafo(&numCidades, entradaArq);
    printf("O grafo demorou %.8lf segundos para ser montado.\n", tempoMontagem);
    if (origem < 0 || origem >= numCidades)
    {
        printf("Origem incorreta\n");
        return 0;
    }

    buscaMelhorTrajeto(vertice, origem, numCidades);
    printf("A busca pelo melhor caminho durou %.8lf segundos.\n", tempoExec);

    freeGrafo(vertice, numCidades);
    free(entradaArq);

    return 0;
}

void FGVazio(TipoVertice *vertice, int numCidades)
{
    int i;

    for (i = 0; i < numCidades; i++)
    {
        vertice[i].listaAdj.primeiro = NULL;
        vertice[i].visitou = NAOVISITADO;
    }
}

void insereAresta(TipoVertice *vertice, int cidade1, int cidade2, int distancia)
{
    TipoApontador novo;

    novo = (TipoApontador)malloc(sizeof(TipoNoAdj));
    novo->cidade = cidade2;
    novo->distancia = distancia;
    novo->prox = vertice[cidade1].listaAdj.primeiro;
    vertice[cidade1].listaAdj.primeiro = novo;
}

TipoVertice *montaGrafo(int *numCidades, char *entradaArq)
{
    int cidade1, cidade2, distancia;
    FILE *fp;
    TipoVertice *vertice;

    fp = fopen(entradaArq, "r+");
    if (fp == NULL)
    {
        printf("Erro na abertura do arquivo!\n");
        exit(-1);
    }
    else
    {
        printf("Arquivo aberto com sucesso!\n");
    }
    fscanf(fp, "%d\n", numCidades);
    vertice = (TipoVertice *)malloc((*numCidades) * sizeof(TipoVertice));

    srand(0);
    clock_t time;

    time = clock();
    FGVazio(vertice, *numCidades);

    while (fscanf(fp, "%d %d %d\n", &cidade1, &cidade2, &distancia) != EOF)
    {

        insereAresta(vertice, cidade1, cidade2, distancia);
        insereAresta(vertice, cidade2, cidade1, distancia);
    }

    time = clock() - time;
    tempoMontagem = ((double)time) / CLOCKS_PER_SEC;

    fclose(fp);
    return vertice;
}

void freeGrafo(TipoVertice *vertice, int numCidades)
{
    int i;
    TipoApontador no, aux;

    for (i = 0; i < numCidades; i++)
    {
        no = vertice[i].listaAdj.primeiro;
        while (no != NULL)
        {
            aux = no;
            no = no->prox;
            free(aux);
        }
        vertice[i].listaAdj.primeiro = NULL;
        vertice[i].visitou = NAOVISITADO;
    }
}

int verificaCidade(TipoVertice *vertice, int numCidades)
{
    int cinza = 0;

    while (cinza < numCidades && vertice[cinza].visitou == VISITADO)
    {
        cinza++;
    }
    if (cinza == numCidades)
        return 1;
    else
        return 0;
}

void copiaVetor(int *caminho, int *menorCaminho, int tamanho)
{
    int i;

    for (i = 0; i < tamanho; i++)
    {
        menorCaminho[i] = caminho[i];
    }
}

void buscaDFS(TipoVertice *vertice, int origem, int numCidades, int distancia, int *caminho, int i)
{
    TipoApontador atual;

    caminho[i] = origem;
    i++;

    if (menorDistancia != -1 && distancia > menorDistancia)
        return;

    vertice[origem].visitou = VISITADO;
    atual = vertice[origem].listaAdj.primeiro;
    while (atual != NULL)
    {
        if (vertice[atual->cidade].visitou == NAOVISITADO)
        {
            buscaDFS(vertice, atual->cidade, numCidades, distancia + atual->distancia, caminho, i);
        }
        atual = atual->prox;
    }
    if ((verificaCidade(vertice, numCidades) == 1 && distancia < menorDistancia) || (verificaCidade(vertice, numCidades) == 1 && menorDistancia == -1))
    {
        menorDistancia = distancia;
        copiaVetor(caminho, menorCaminho, numCidades);
    }
    vertice[origem].visitou = NAOVISITADO;
}

void imprimeCaminho(TipoVertice *vertice, int *menorCaminho, int numCidades)
{
    TipoApontador atual = NULL;
    int i;

    printf("\n");
    if (menorDistancia != -1)
    {
        for (i = 0; i < numCidades - 1; i++)
        {
            atual = vertice[menorCaminho[i]].listaAdj.primeiro;
            while (atual->cidade != menorCaminho[i + 1])
            {
                atual = atual->prox;
            }
            printf("Distância entra as cidades %d e %d: %d\n", menorCaminho[i], menorCaminho[i + 1], atual->distancia);
        }
        printf("\nCaminho mais curto:\n%d", menorCaminho[0]);
        for (i = 1; i < numCidades; i++)
        {
            printf("-%d", menorCaminho[i]);
        }
        printf("\n\nDistância Total: %d\n\n", menorDistancia);
    }
    else
    {
        printf("\nPartindo dessa cidade não é possível passar por todas as outras sem passar duas vezes em uma mesma cidade.\n\n");
    }
}

void buscaMelhorTrajeto(TipoVertice *vertice, int origem, int numCidades)
{
    int caminho[numCidades], i;

    menorCaminho = (int *)malloc(numCidades * sizeof(int));

    srand(0);
    clock_t time;

    time = clock();
    buscaDFS(vertice, origem, numCidades, 0, caminho, 0);
    time = clock() - time;

    tempoExec = ((double)time) / CLOCKS_PER_SEC;

    imprimeCaminho(vertice, menorCaminho, numCidades);
    free(menorCaminho);
}
