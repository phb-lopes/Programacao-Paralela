int rc;
long t;

for(t=0; t<NUM_THREADS; t++) 
{
   printf("Creating thread %ld\n", t);
   rc = pthread_create(&threads[t], NULL, PrintHello, (void *) &t);
   ...
}