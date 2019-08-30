#include <math.h>
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

int main() {
	pthread_t thread1, thread2;
	pthread_attr_t attr;
	int x;
	void * retval = NULL;

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

	return 0;
}
