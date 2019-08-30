#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/time.h>

const int TAM = 1000000;
const int NUM_THREADS = 5;

struct dados {
	int *vet;
	int id;
	int tam;
};

void calc_tempo(struct timeval t_inicio, struct timeval t_fim) {
	unsigned long long inicio;
	unsigned long long fim;

	inicio = (t_inicio.tv_sec * 1000) + (t_inicio.tv_usec / 1000); /* converte o tempo para milisegundos */
	fim = (t_fim.tv_sec * 1000) + (t_fim.tv_usec / 1000); /* converte o tempo para milisegundos */
	printf("Tempo = %llu.%03llus.\n", (fim - inicio)/1000, (fim - inicio)%1000);
}

void *primo(void *args) {
	int i, j, cont;
	struct dados *d = (struct dados *)args;

	if(d == NULL)
		pthread_exit(args);

	d->vet = (int *)malloc((TAM / NUM_THREADS) * sizeof(int));

	for(i = d->id - 1; i < TAM; i+= NUM_THREADS) {
		cont = 0;
		for(j = 1; j <= i; j++) {
			if((i % j) == 0) {
				cont++;
				if(cont > 2)
					break;
			}
		}
		if(cont == 2) {
			d->vet[d->tam++] = i;
		}
	}
	pthread_exit(args);
}

int main() {
	int i, j;
	int vet[TAM], tvet = 0;
	struct dados dthread[NUM_THREADS];
	pthread_t threads[NUM_THREADS];
	pthread_attr_t attr;
	struct timeval t_inicio, t_fim;

	gettimeofday(&t_inicio, NULL);

	pthread_attr_init(&attr);
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

	for(i = 0; i < NUM_THREADS; i++) {
		dthread[i].vet = NULL;
		dthread[i].id = i + 1;
		pthread_create(&threads[i], &attr, primo, (void *) &dthread[i]);
	}

	

	for(i = 0; i < NUM_THREADS; i++) {
		pthread_join(threads[i], NULL);
		if(dthread[i].vet == NULL)
			printf("Erro ao receber vetor\n");
		else {
			for(j = 0; j < dthread[i].tam; j++)
				vet[tvet++] = dthread[i].vet[j];
			free(dthread[i].vet);
		}
	}

	gettimeofday(&t_fim, NULL);

	calc_tempo(t_inicio, t_fim);

	/*for(i = 0; i < tvet; i++)
		printf("%d ", vet[i]);
	printf("\n");*/

	return 0;
}
