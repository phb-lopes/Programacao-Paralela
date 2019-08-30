#include <math.h>
#include <sys/time.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

void * f1(void *args) {
	int x = *(int *)args;
	double *seno = (double *)malloc(sizeof(double));
	*seno = sin(x);
	*seno = pow(*seno, 3.0);
	printf("Resultado f1 = %lf\n", *seno);
	pthread_exit((void *)seno);
}

void * f2(void *args) {
	int x = *(int *)args;
	double *cosseno = (double *)malloc(sizeof(double));
	*cosseno = cos(x);
	*cosseno = sqrt(*cosseno);
	printf("Resultado f2 = %lf\n", *cosseno);
	pthread_exit((void *)cosseno);
}

void calc_tempo(struct timeval t_inicio, struct timeval t_fim) {
	unsigned long long inicio;
	unsigned long long fim;

	inicio = (t_inicio.tv_sec * 1000) + (t_inicio.tv_usec / 1000); /* converte o tempo para milisegundos */
	fim = (t_fim.tv_sec * 1000) + (t_fim.tv_usec / 1000); /* converte o tempo para milisegundos */
	printf("Tempo = %llu.%03llus.\n", (fim - inicio)/1000, (fim - inicio)%1000);
}

int main() {
	pthread_t thread1, thread2;
	pthread_attr_t attr;
	int x;
	void *retval = NULL;
	struct timeval t_inicio, t_fim;

	gettimeofday(&t_inicio, NULL);

	pthread_attr_init(&attr);
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

	printf("Digite o valor de x: ");
	scanf("%d", &x);
	pthread_create(&thread1, &attr, f1, (void *)&x);
	pthread_create(&thread2, &attr, f2, (void *)&x);
	pthread_join(thread1, &retval);
	printf("Resultado FORA f1 = %lf\n", *(double *)retval);
	pthread_join(thread2, &retval);
	printf("Resultado FORA f2 = %lf\n", *(double *)retval);

	gettimeofday(&t_fim, NULL);

	calc_tempo(t_inicio, t_fim);
	
	return 0;
}
