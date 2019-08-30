#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/time.h>

const int TAM = 10;
const int NUM_THREADS = 3;

struct dados {
	int *vetor;
	int val;
};

void calc_tempo(struct timeval t_inicio, struct timeval t_fim) {
	unsigned long long inicio;
	unsigned long long fim;

	inicio = (t_inicio.tv_sec * 1000) + (t_inicio.tv_usec / 1000); /* converte o tempo para milisegundos */
	fim = (t_fim.tv_sec * 1000) + (t_fim.tv_usec / 1000); /* converte o tempo para milisegundos */
	printf("Tempo = %llu.%03llus.\n", (fim - inicio)/1000, (fim - inicio)%1000);
}

void *mult(void *args) {
	struct dados *d = (struct dados *)args;
	int *orig, i;

	if(d == NULL)
		pthread_exit(NULL);

	orig = d->vetor;
	if(orig == NULL)
		pthread_exit(NULL);
	
	d->vetor = (int *)malloc(TAM * sizeof(int));
	if(d->vetor == NULL)
		pthread_exit(NULL);

	for(i = 0; i < TAM; i++)
		d->vetor[i] = orig[i] * d->val;

	pthread_exit(NULL);
}

int main() {
	int i, j;
	int vet[TAM];
	struct dados dthread[NUM_THREADS];
	pthread_t threads[NUM_THREADS];
	pthread_attr_t attr;
	struct timeval t_inicio, t_fim;

	srand(time(0));
	gettimeofday(&t_inicio, NULL);

	pthread_attr_init(&attr);
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
	
	for(i = 0; i < TAM; i++)
		vet[i] = rand() % 100;

	for(i = 0; i < NUM_THREADS; i++) {
		dthread[i].vetor = vet;
		dthread[i].val = rand() % 10;
		printf("Thread[%d] val = %d\n", i, dthread[i].val);
		pthread_create(&threads[i], &attr, mult, (void *) &dthread[i]);
	}

	printf("Original =");
	for(i = 0; i < TAM; i++)
		printf(" %4d", vet[i]);
	printf("\n");

	for(i = 0; i < NUM_THREADS; i++) {
		pthread_join(threads[i], NULL);
		printf("Thread %d =", i);
		for(j = 0; j < TAM; j++)
			printf(" %4d", dthread[i].vetor[j]);
		printf("\n");
	}

	gettimeofday(&t_fim, NULL);

	calc_tempo(t_inicio, t_fim);

	return 0;
}
