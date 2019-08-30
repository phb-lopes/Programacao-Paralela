#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

struct dados {
	int x;
	int y;
};

int res; /* resultado por variavel global */

void *funcao(void *args) {
	struct dados *d = (struct dados *)args;
	res = d->x + d->y;
	pthread_exit(NULL);
}

int main() {
	pthread_t t;
	pthread_attr_t attr;
	struct dados d;

	printf("Digite o valor de x: ");
	scanf("%d", &d.x);
	printf("Digite o valor de y: ");
	scanf("%d", &d.y);

	pthread_attr_init(&attr);
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

	pthread_create(&t, &attr, funcao, (void *)&d);

	pthread_join(t, NULL);

	printf("Resultado = %d\n", res);

	return 0;
}
