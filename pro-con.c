#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>
#define PRO_N 3   //producer
#define CON_N 2   //consumer
#define M 10      //buffer
#define ITEMS 50
int in = 0;
int out = 0;
int buff[M] = {0};
sem_t empty_sem; 
sem_t full_sem;  
pthread_mutex_t mutex;
int producer_id = 0;  
int consumer_id = 0; 
int now_item = 0;

void print()
{
	int i;
	for(i = 0; i < M; i++)
    	printf("%d ", buff[i]);
	printf("\n");
}
 
void *producer()
{
	int id = ++producer_id;
	while(1)
	{
		sleep(1);
		sem_wait(&empty_sem);
		pthread_mutex_lock(&mutex);   
		in = in % M;
		printf("producer%d produces product in buffer%d  repo: \t", id, in);
		buff[in] = 1;  
		print();  
		++in;
		pthread_mutex_unlock(&mutex);
		sem_post(&full_sem);  
	}
}

void *consumer()
{
	int id = ++consumer_id;
	while(1)
	{
		sleep(1);       
		sem_wait(&full_sem);
		pthread_mutex_lock(&mutex);
		out = out % M;
		printf("consumer%d consumes product in buffer%d  repo: \t", id, out);
		buff[out] = 0;
		print();
		++now_item;
		++out;
		if(now_item >= ITEMS) 
		{
			exit(1);
		}
		pthread_mutex_unlock(&mutex);
		sem_post(&empty_sem);
	}
}

int main()
{
	pthread_t id1[PRO_N];
	pthread_t id2[CON_N];
	int i;
	int pro_thr[PRO_N];
	int con_thr[CON_N];
	int ini1 = sem_init(&empty_sem, 0, M); 
	int ini2 = sem_init(&full_sem, 0, 0);  
	if(ini1 && ini2 != 0)
	{
		printf("sem init failed \n");
		exit(1);
	} 
	int ini3 = pthread_mutex_init(&mutex, NULL);
	if(ini3 != 0)
	{
		printf("mutex init failed \n");
		exit(1);
	} 
	for(i = 0; i < PRO_N; i++)
	{
		pro_thr[i] = pthread_create(&id1[i], NULL, producer, (void *)(&i));
		if(pro_thr[i] != 0)
		{
			printf("producer%d creation failed \n", i);
			exit(1);
		}
	}
 	for(i = 0; i < CON_N; i++)
	{
		con_thr[i] = pthread_create(&id2[i], NULL, consumer, NULL);
		if(con_thr[i] != 0)
		{
			printf("consumer%d creation failed \n", i);
			exit(1);
		}
	}
	for(i = 0; i < PRO_N; i++)
	{
		pthread_join(id1[i],NULL);
	}
	for(i = 0; i < PRO_N; i++)
	{
		pthread_join(id2[i],NULL);
	}
	exit(0); 
}


