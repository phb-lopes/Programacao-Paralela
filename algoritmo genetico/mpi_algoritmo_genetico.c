#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include <locale.h>
#include <time.h>

#define tam 58
#define NUM_THREADS	2

const int tam_pop = 50;
const int tam_gera = 2;
const float valor_mutacao = 0.1;
const int metade = tam_pop / 2;

int num_indi = 0;
int cont_pop=0;
int matriz[tam][tam];
int populacao[tam][tam];
int usado[tam+1];
int ind[tam];

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

struct vetor{
	int num;
	struct vetor *prox;
};

void imprimir_lista(struct vetor *lista){
	if(lista == NULL) {
		printf("Lista Vazia\n");
		return;
	}

	printf("%d", lista->num);
	for(lista = lista->prox; lista != NULL; lista = lista->prox)
		printf("-->%d", lista->num);
	printf("\n");

}

struct vetor *inserir(struct vetor *lista, int num){
	struct vetor *v1;
	struct vetor *aux;
    
    if(lista == NULL){
        v1 = (struct vetor*)malloc(sizeof(struct vetor));
        if(v1 == NULL)
            return lista;
        
        v1->num = num;
        v1->prox = NULL;
        lista = v1;
    }else{    
        aux = lista;
        v1 = (struct vetor*)malloc(sizeof(struct vetor));
        if(v1 == NULL)
        	return lista;
        
        while(aux->prox != NULL)    
			aux = aux->prox;

		v1->num = num;
		v1->prox = NULL;
        aux->prox = v1;
            
    }
    return lista;
}

struct vetor *remover(struct vetor *lista, int num) {
	struct vetor *ant;

	if(lista == NULL)
		return lista;

	if(lista->num == num) {
		struct vetor *tmp = lista;
		lista = lista->prox;
		free(tmp);
		return lista;
	}

	ant = lista;
	while(ant->prox != NULL) {
		if(ant->prox->num == num) {
			struct vetor *tmp = ant->prox;
			ant->prox = ant->prox->prox;
			free(tmp);
			return lista;
		}
		ant = ant->prox;
	}
	return lista;
}

int custo(int *x) {
	int i, c = 0;

	for(i = 0; i < tam - 1; i++) {
		if(matriz[x[i] - 1][x[i + 1] - 1] == 0)
			return -1;
		else
			c += matriz[x[i] - 1][x[i + 1] - 1];
	}
	if(matriz[x[tam - 1] - 1][x[0] - 1] == 0)
		return -1;
	c += matriz[x[tam - 1] - 1][x[0] - 1];	

	//printf("Custo total do caminho [ %d ]\n", c);
	return c;
	
}

void melhor(int **pop, int id) {
	int i, j, val, melhor = custo(pop[0]);

	for(i = 1; i < tam_pop; i++) {
		val = custo(pop[i]);
		if(val < melhor)
			melhor = val;
	}

	printf("\n\t\t\t\t\t\t\t\t\t\t Melhor Atual [%d] = %d\n\n", id, melhor);
}

void add_individuo(int *x) {
	int i;

	if(cont_pop == tam)
		return;

	for(i = 0; i < tam; i++)
		populacao[cont_pop][i] = x[i];
	cont_pop++;
}


void conferir(int *x){
	if(custo(x) > 0) {
		add_individuo(x);
		num_indi++;
	}
}

void add_cruzamento(int *x, int **pop2, int *cont_metade) {
	int i;

	if(*cont_metade == metade)
		return;

	for(i = 0; i < tam; i++)
		pop2[*cont_metade][i] = x[i];
		
	(*cont_metade)++;
}

void conferir_cruzamento(int *x, int **pop2, int *cont_metade){
	int i=0;

	if(custo(x) > 0)
		add_cruzamento(x, pop2, cont_metade);
}

int conferir_mutacao(int *x){
	
	if(custo(x) > 0)
		return 1;
	else
		return 0;
}

void mutar_populacao(int **pop, int **pop2){
	int i,j,k;
	int contador = 0;

	
	for(i = 0; i < metade; i++){
		for(j = 0; j < tam; j++){
			pop[i][j] = pop2[i][j];
		}
	}
}

int permutacao(int indice){
	int i=0;	
	
	if(indice == tam){
		conferir(ind);
		if(cont_pop == tam_pop)			
			return 1;
		return 0;
	}

	if(indice == 0) {
		for(i = 0; i <= tam; i++)
			usado[i] = 0;
	}

	for(i = 1;i<=tam;i++){
		if(usado[i]==0){
			ind[indice] = i;
			usado[i] = 1;
			if(permutacao(indice +1))
				return 1;
			usado[i] = 0;
		}
	}
	return 0;
}

void mutacao(int **individuo){
	int i,j;
	int aux=1000,aux1;
	int num_mutacoes = valor_mutacao * tam_pop;
	int elemento[tam];

	//printf("NUM_MUTACOES = %.2f * %d = %d\n", valor_mutacao, tam_pop, num_mutacoes);

	while(num_mutacoes) {
		int ind = rand() % tam_pop;
		int g1 = rand() % tam;
		int g2 = rand() % tam;

		for(i = 0; i < tam; i++)
			elemento[i] = individuo[ind][i];

		aux = elemento[g1];
		elemento[g1] = elemento[g2];
		elemento[g2] = aux;

		if(conferir_mutacao(elemento)){
			for(i = 0; i < tam; i++)
				individuo[ind][i] = elemento[i];
			num_mutacoes--;
		}
	}

}


void crossover_ox(int **pop){
	int num = tam/2;
	int i=0,j, z;
	int pai1[num],filho1[num], vetor_final[tam];
	
	int posicao;
	struct vetor *k;
	int cont_metade = 0;
	int **pop2;
	struct vetor *vetor = NULL;

	pop2 = (int **) malloc(sizeof(int *) * tam);
	for(i = 0; i < tam; i++) {
		pop2[i] = (int *) malloc(sizeof(int) * tam);
	}

	for(z = 0; z < tam_pop; z += 2) {
		for (i = 0; i < num; ++i)
			pai1[i] = pop[z][i];
		
		for (i = 0; i < tam; ++i)
			vetor = inserir(vetor,pop[z+1][i]);

		for (i = 0; i < num; ++i)
			vetor = remover(vetor, pai1[i]);
		
		for (i = 0; i < num; ++i)
			vetor = inserir(vetor, pai1[i]);
		
		for (k = vetor,i=0; k != NULL; k=k->prox,i++){
			vetor_final[i] = k->num;
			vetor = remover(vetor, k->num);
		}
		vetor = NULL;
		if(conferir_mutacao(vetor_final) > 0)
			conferir_cruzamento(vetor_final, pop2, &cont_metade);
	}
	mutar_populacao(pop, pop2);
	for(i = 0; i < tam; i++)
		free(pop2[i]);
	free(pop2);
}

void algoritmo_genetico(int **pop, int id) {
	int i = 0;

	while(i < tam_gera) {
		//printf("\n\t\t\t\t\t\t\t --------------------------- GERAÇÃO T%d [%d] --------------------------- \n\n", id, i+1);
		crossover_ox(pop);
		//printf("\n\t\t\t\t\t\t\t --------------------------- CRUZAMENTO T%d [%d] --------------------------- \n\n", id, i+1);
		mutacao(pop);
		//printf("\n\t\t\t\t\t\t\t --------------------------- MUTACAO T%d [%d] --------------------------- \n\n", id, i+1);
		melhor(pop, id);
		//printf("\n\t\t\t\t\t\t\t --------------------------- FIM T%d [%d] --------------------------- \n\n", id, i+1);
		i++;
	}
}

void *tread_populacao(void *args){
	int **pop_int, i, j;

	pop_int = (int **) malloc(sizeof(int *) * tam);
	for(i = 0; i < tam; i++) {
		pop_int[i] = (int *) malloc(sizeof(int) * tam);
		for(j = 0; j < tam; j++)
			pop_int[i][j] = populacao[i][j];
	}

	algoritmo_genetico(pop_int, *(int *)args);

	for(i = 0; i < tam; i++)
		free(pop_int[i]);
	free(pop_int);

	pthread_exit(NULL);
}

int main(int argc, char **argv){
	int i, j;
	int id,num_proc;
	setlocale(LC_ALL,"Portuguese");
	

	MPI_Init(&argc,&argv);
	MPI_Comm_rank(MPI_COMM_WORLD,&id);
	MPI_Comm_size(MPI_COMM_WORLD,&num_proc);
	
	srand(time(0));
	
	for (i = 0; i < tam; ++i){
		for (j = 0; j < tam; ++j)
			scanf("%d",&matriz[i][j]);
	}

	permutacao(0);
	
	MPI_Finalize();

	return 0;
}


