//Lista 6 - ED2
//Nícalo Ribeiro - 16/0016169

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

#define NAOVISITADO 0
#define VISITADO 1
#define ALTURA_MAX 1000
#define IFNT (1<<20)

static int menorDistancia = -1;
static int *menorCaminho;
double tempoExec = 0, tempoMontagem = 0;
int lprofile[ALTURA_MAX];
int rprofile[ALTURA_MAX];
int gap = 3;
int print_next;

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

typedef struct arvore Arvore;
struct arvore{
	Arvore *esq;
	Arvore *dir;
	int valor;
};

typedef struct arvore_ascii Arvore_ascii;	
struct arvore_ascii{
	Arvore_ascii *esq;
	Arvore_ascii *dir;
	int edge_length;
	int height;
	int lablen;
	int parent_dir;
	char label[11];
};

void FGVazio(TipoVertice *, int);
void insereAresta(TipoVertice *, int, int, int);
TipoVertice *montaGrafo(int *, char *);
void freeGrafo(TipoVertice *, int);
void buscaDFS(TipoVertice *, int, int, int, int *, int);
int verificaCidade(TipoVertice *, int);
void copiaVetor(int *, int *, int);
void imprimeCaminho(TipoVertice *, int *, int);
void buscaMelhorTrajeto(TipoVertice *, int, int);
Arvore *cria_arvore();
Arvore *inserir(int valor, Arvore *no);
void showTree(Arvore *no);
Arvore_ascii * cria_arvore_ascii(Arvore * no);
Arvore_ascii *arvore_ascii_recursiva(Arvore * no);
void compute_lprofile(Arvore_ascii *novo, int x, int y);
void compute_edge_lengths(Arvore_ascii *novo);
void print_level(Arvore_ascii *novo, int x, int level);
int MIN (int X, int Y);
int MAX (int X, int Y);
void libera_arvore_ascii(Arvore_ascii *novo);

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
    Arvore * raiz = cria_arvore();

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
            int valor = menorCaminho[i];        
            raiz = inserir(valor,raiz);
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

    printf("Árvore gerada a partir dos vértices que representam o menor caminho : \n");
    showTree(raiz);
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

Arvore* cria_arvore(){
	Arvore* raiz = (Arvore*)malloc(sizeof(Arvore));
	if(raiz != NULL){
		raiz = NULL;
	}
	return raiz;
}

Arvore *inserir(int valor, Arvore *no){  
	Arvore *novo;
	
	if(no == NULL){
		novo = (Arvore*)malloc(sizeof(Arvore));
		if(novo == NULL){
			return no;
		}
		novo->valor = valor;
		novo->esq = novo->dir = NULL;
		return novo;
	}
	
	if (valor < no->valor){
		no->esq = inserir(valor, no->esq);
	}
	else if (valor > no->valor){
		no->dir = inserir(valor, no->dir);
	}
	else{
		return no;
	}
		return no;
}		

void showTree(Arvore *no){
	Arvore_ascii *proot;
	int xmin, i,opcao;
	if (no == NULL){
		printf("Erro na montagem da árvore.\n");
	}
	proot = cria_arvore_ascii(no);
	compute_edge_lengths(proot);
	for (i=0; i<proot->height && i < ALTURA_MAX; i++){
		lprofile[i] = IFNT;
	}
	compute_lprofile(proot, 0, 0);
	xmin = 0;
	for (i = 0; i < proot->height && i < ALTURA_MAX; i++){
		xmin = MIN(xmin, lprofile[i]);
	}
	for (i = 0; i < proot->height; i++){
		print_next = 0;
		print_level(proot, -xmin, i);
		printf("\n");
	}
	libera_arvore_ascii(proot);

	
}

Arvore_ascii * cria_arvore_ascii(Arvore * no){
	Arvore_ascii *novo;
	if (no == NULL){
		return NULL;
	}
	novo = arvore_ascii_recursiva(no);
	novo->parent_dir = 0;
	return novo;
}

Arvore_ascii *arvore_ascii_recursiva(Arvore * no){
	Arvore_ascii * novo;
	
	if (no == NULL){
		return NULL;
	}
	novo = (Arvore_ascii*)malloc(sizeof(Arvore_ascii));
	novo->esq = arvore_ascii_recursiva(no->esq);
	novo->dir = arvore_ascii_recursiva(no->dir);
	if (novo->esq != NULL){
		novo->esq->parent_dir = -1;
	}
	
	if (novo->dir != NULL){
		novo->dir->parent_dir = 1;
	}
	
	sprintf(novo->label, "%d", no->valor);
	novo->lablen = strlen(novo->label);
	
	return novo;
}

void compute_lprofile(Arvore_ascii *novo, int x, int y){
	int i, isesq;
	if (novo == NULL){
		return;
	}
	isesq = (novo->parent_dir == -1);
	lprofile[y] = MIN(lprofile[y], x-((novo->lablen-isesq)/2));
	if (novo->esq != NULL){
		for (i=1; i <= novo->edge_length && y+i < ALTURA_MAX; i++){
			lprofile[y+i] = MIN(lprofile[y+i], x-i);
		}
	}
	compute_lprofile(novo->esq, x-novo->edge_length-1, y+novo->edge_length+1);
	compute_lprofile(novo->dir, x+novo->edge_length+1, y+novo->edge_length+1);
}

void compute_rprofile(Arvore_ascii *novo, int x, int y){
	int i, notesq;
	if (novo == NULL){
		return;
	}
	notesq = (novo->parent_dir != -1);
	rprofile[y] = MAX(rprofile[y], x+((novo->lablen-notesq)/2));
	if (novo->dir != NULL){
		for (i=1; i <= novo->edge_length && y+i < ALTURA_MAX; i++){
			rprofile[y+i] = MAX(rprofile[y+i], x+i);
		}
	}
	compute_rprofile(novo->esq, x-novo->edge_length-1, y+novo->edge_length+1);
	compute_rprofile(novo->dir, x+novo->edge_length+1, y+novo->edge_length+1);
}

int MIN (int X, int Y){
	return ((X) < (Y)) ? (X) : (Y);
}
	
int MAX (int X, int Y){
	return ((X) > (Y)) ? (X) : (Y);
}
	
void compute_edge_lengths(Arvore_ascii *novo){
	int h, hmin, i, delta;
	if (novo == NULL){
		return;
	}
	compute_edge_lengths(novo->esq);
	compute_edge_lengths(novo->dir);

	if (novo->dir == NULL && novo->esq == NULL){
		novo->edge_length = 0;
	}
	else{
		if (novo->esq != NULL){
			for (i=0; i<novo->esq->height && i < ALTURA_MAX; i++){
				rprofile[i] = -IFNT;
			}
			compute_rprofile(novo->esq, 0, 0);
			hmin = novo->esq->height;
		}
		else{
			hmin = 0;
		}
		if (novo->dir != NULL){
			for (i=0; i<novo->dir->height && i < ALTURA_MAX; i++){
				lprofile[i] = IFNT;
			}
		compute_lprofile(novo->dir, 0, 0);
		hmin = MIN(novo->dir->height, hmin);
		}
		else{
			hmin = 0;
		}
		delta = 4;
		for (i=0; i<hmin; i++){
			delta = MAX(delta, gap + 1 + rprofile[i] - lprofile[i]);
		}
		if (((novo->esq != NULL && novo->esq->height == 1) ||(novo->dir != NULL && novo->dir->height == 1))&&delta>4){
			delta--;
		}
		novo->edge_length = ((delta+1)/2) - 1;
	}
	h = 1;
	if (novo->esq != NULL){
		h = MAX(novo->esq->height + novo->edge_length + 1, h);
	}
	if (novo->dir != NULL){
		h = MAX(novo->dir->height + novo->edge_length + 1, h);
	}
	novo->height = h;
}
	
void print_level(Arvore_ascii *novo, int x, int level){
	int i, isesq;
	if (novo == NULL){
		return;
	}
	isesq = (novo->parent_dir == -1);
	if (level == 0){
		for (i=0; i<(x-print_next-((novo->lablen-isesq)/2)); i++){
			printf(" ");
		}
	print_next += i;
	printf("%s", novo->label);
	print_next += novo->lablen;
	}
	else if (novo->edge_length >= level){
		if (novo->esq != NULL){
			for (i=0; i<(x-print_next-(level)); i++){
				printf(" ");
			}
		print_next += i;
		printf("/");
		print_next++;
		}
		if (novo->dir != NULL){
			for (i=0; i<(x-print_next+(level)); i++){
				printf(" ");
			}
		print_next += i;
		printf("\\");
		print_next++;
		}
	 }
	 else{
	 	print_level(novo->esq,x-novo->edge_length-1,level-novo->edge_length-1);
	 	print_level(novo->dir,x+novo->edge_length+1,level-novo->edge_length-1);
	 }
}

void libera_arvore_ascii(Arvore_ascii *novo){
	if (novo == NULL){
		return;
	}
	libera_arvore_ascii(novo->esq);
	libera_arvore_ascii(novo->dir);
	free(novo);
}