#include <stdio.h>
#include <stdlib.h>
#include <locale.h>
#include <time.h>

#define tam 58

const int tam_pop = 50;
const int tam_gera = 100;
const float valor_mutacao = 0.1;

int num_indi = 0;
int metade = tam_pop/2;
int cont_pop=0;
int cont_metade = 0;
int matriz[tam][tam];
int populacao[tam][tam];
int populacao2[tam][tam];
int aux[tam][tam];
int usado[tam+1];
int ind[tam];


struct vetor{
	int num;
	struct vetor *prox;
};

struct vetor *vetor = NULL;

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

void melhor() {
	int i, j, val, melhor = custo(populacao[0]);

	for(i = 1; i < tam_pop; i++) {
		val = custo(populacao[i]);
		if(val < melhor)
			melhor = val;
	}

	printf("Melhor Atual = %d\n", melhor);
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
	int i=0;

	if(custo(x) > 0) {
		add_individuo(x);
		//printf("Individuo [ %d ] -- ",num_indi);
		num_indi++;
		//for(i = 0; i < tam; i++)
		//	printf(" %d ", x[i]);
		//printf("\n");
	}
}

//==============================================================================================================

void add_cruzamento(int *x) {
	int i;

	if(cont_metade == metade)
		return;

	//		ERRO AQUI	<-------
	for(i = 0; i < tam; i++){
		populacao2[cont_metade][i] = x[i];
		//printf(" %d ",populacao2[cont_metade][i]);
	}
	//Aparentemente adicionou em populacao2
	cont_metade++;
}

void conferir_cruzamento(int *x){
	int i=0;

	if(custo(x) > 0) {
		add_cruzamento(x);

	}
}

int conferir_mutacao(int *x){
	int i=0;
	
	//printf("Populacao 2 [%d]\n",x[i]);
	
	if(custo(x) > 0){

		return 1;
	}else{
		return 0;
	}
}


void imprimir_geral(){
	int i,j;
	for(i = 0; i < tam_pop; i++){
		//if(custo(matriz[i])>0){
			printf("Individuo [%d]\t",i+1);
			for(j = 0; j < tam; j++){
				printf(" %d ", populacao[i][j]);
			}
		//}
		printf("\n");
	}
}

void mutar_populacao(){
	int i,j,k;
	int contador = 0;

	
	for(i = 0; i < metade; i++){
		for(j = 0; j < tam; j++){
			populacao[i][j] = populacao2[i][j];
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

void mutacao(int individuo[tam][tam]){
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

		if(conferir_mutacao(elemento)) {
			//printf("Mutacao individuo[%d]\n", ind);
			for(i = 0; i < tam; i++)
				individuo[ind][i] = elemento[i];
			num_mutacoes--;
		}
	}

}


void crossover_ox(){
	int num = tam/2;
	int i=0,j, z;
	int pai1[num],filho1[num], vetor_final[tam];
	
	int posicao;
	struct vetor *k;
	cont_metade = 0;
	
	for(z = 0; z < tam_pop; z += 2) {
		
		for (i = 0; i < num; ++i){
			pai1[i] = populacao[z][i];
		}
		
		for (i = 0; i < tam; ++i) {
			vetor = inserir(vetor,populacao[z+1][i]);
		}


		for (i = 0; i < num; ++i){
			vetor = remover(vetor, pai1[i]);
		}
		
		for (i = 0; i < num; ++i){
			vetor = inserir(vetor, pai1[i]);
		}
		
		for (k = vetor,i=0; k != NULL; k=k->prox,i++){
			vetor_final[i] = k->num;
			vetor = remover(vetor, k->num);
		}
		vetor = NULL;

		//for (i = 0; i < tam; ++i){
		//	printf("Vetor final [%d]",vetor_final[i]);
		//}
		//printf("\n");
		

		if(conferir_mutacao(vetor_final) > 0){
			conferir_cruzamento(vetor_final);
			//mutar_populacao();
		}
	}
	mutar_populacao();
}

void algoritmo_genetico() {
	int i = 0,k,j;
	

	//printf("\n\n\t\t\t --------------------------- POPULAÇÃO INICIAL --------------------------- \n\n");
	permutacao(0);

	while(i < tam_gera) {
		printf("\n\t\t\t --------------------------- GERAÇÃO [%d]--------------------------- \n\n", i+1);
		//imprimir_geral();
		crossover_ox();

		//printf("\n\t\t\t --------------------------- CRUZAMENTO [%d]--------------------------- \n\n", i+1);
		//imprimir_geral();

		//printf("\n\t\t\t --------------------------- MUTACAO [%d]--------------------------- \n\n", i+1);
		mutacao(populacao);
		melhor();
		//imprimir_geral();
		
		i++;
	}
}


int main(){
	setlocale(LC_ALL,"Portuguese");
	
	srand(time(0));
	

	for (int i = 0; i < tam; ++i){
		for (int j = 0; j < tam; ++j)
			scanf("%d",&matriz[i][j]);
	}

	algoritmo_genetico();


	return 0;
}