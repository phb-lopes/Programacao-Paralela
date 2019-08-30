#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

struct dados {
	int x;
	int y;
};

void *funcao(void *args) {
	struct dados *d = (struct dados *)args;
	int *res = (int *)malloc(sizeof(int)); /* resultado por alocacao dinamica */
	*res = d->x + d->y;
	pthread_exit((void *)res);
}

int main() {
	pthread_t t;
	pthread_attr_t attr;
	struct dados d;
	int *retval;

	printf("Digite o valor de x: ");
	scanf("%d", &d.x);
	printf("Digite o valor de y: ");
	scanf("%d", &d.y);

	pthread_attr_init(&attr);
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

	pthread_create(&t, &attr, funcao, (void *)&d);

	pthread_join(t, (void **)&retval);

	printf("Resultado = %d\n", *retval);

	return 0;
}
