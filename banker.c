#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>

/* number of customers */
int noc;
/* number of resources */
int nor;
/* the avaliable amount of each resources */
int *avaliable;
/* the maximum demand of each customer */
int *maximum;
/* the amount currently allocated to each customer */
int *allocation;
/* the ramaining need of each customer */
int *need;
/* pthread mutex */
pthread_mutex_t mutex;
pthread_mutex_t mutexsum;

void print_vector(int *x, int n){
    printf("\t\t  ");
    for (int i = 0; i < n; i++)
        printf("%4d ",x[i]);
    printf("\n");
}

//print matrix A which row number is m and col number is n
void print_matrix(int *A, int m,int n){
    for (int i = 0; i < m; i++)
    {
        printf("\t\t  ");
        for (int j = 0; j < n; j++)
            printf("%4d ",A[i * m + j]);
        printf("\n");
    }
}

void printinfo(){
    printf("\n");
    printf("AVALI:\n");
    print_vector(avaliable, nor);
    printf("MAX:\n");
    print_matrix(maximum,noc,nor);
    printf("ALLOC:\n");
    print_matrix(allocation,noc,nor);
    printf("NEED:\n");
    print_matrix(need,noc,nor);
    printf("\n");
}

/* success return 0, false return -1 */
int request_resource(int customer_num, int *request)
{
    // pthread_mutex_lock(&mutex);
    for (int i = 0; i < nor; i++)
        if (request[i] > avaliable[i])
            return -1;
            
    for (int i = 0; i < nor; i++)
    {
        allocation[customer_num * nor + i] += request[i];
        avaliable[i] -= request[i];
        need[customer_num * nor + i] -= request[i];
    }
    // pthread_mutex_unlock(&mutex);
    return 0;
}

/* success return 0, false return -1 */
int release_resource(int customer_num, int *release)
{
    // pthread_mutex_lock(&mutex);
    for (int i = 0; i < nor; i++)
        if (release[i] > allocation[customer_num * nor + i])
            return -1;
    
    for (int i = 0; i < nor; i++)
    {
        avaliable[i] += release[i]; 
        need[customer_num * nor + i] += release[i]; 
        allocation[customer_num * nor + i] -= release[i];
    }
    // pthread_mutex_unlock(&mutex);
    return 0;
}

/* below return 1, else return 0 */
int needbwork(int *work, int i){
    for (int j = 0; j < nor; j++)
    {
        if (need[i * nor + j] > work[j])
        {
            return 0;
        }
    }
    return 1;
}

/* safe return 1, unsafe return 0 */
int safe()
{
    // pthread_mutex_lock(&mutex);
    int *work = (int *)malloc(sizeof(int) * nor);
    for (int i = 0; i < nor; i++)
    {
        work[i] = avaliable[i];
    }
    // memcpy(work, avaliable, sizeof(int) * nor);
    int *finish = (int *)malloc(sizeof(int) * noc);
    memset(finish, 0, sizeof(int) * noc);
    for (int k = 0; k < noc; k++)
    {
        for (int i = 0; i < noc; i++)
        {
            if (finish[i] == 0 && needbwork(work,i))
            {
                for (int j = 0; j < nor; j++)
                {
                    work[j] += allocation[i * nor + j];
                }
                finish[i] = 1;
            }
        }
    }
    // pthread_mutex_unlock(&mutex);
    for (int i = 0; i < noc; i++)
    {
        if (finish[i] == 0)
        {
            free(work);
            free(finish);
            return 0;
        }
    }
    free(work);
    free(finish);
    return 1;
}

void *multithread(int *i)
{
    // pthread_mutex_lock(&mutexsum);
    int cid = *i;
    int *request = (int *)malloc(sizeof(int) * nor);
    int flag = 1;
    while(flag)
    {    
        // sleep(1);
        pthread_mutex_lock(&mutex);
        for (int i = 0; i < nor; i++){
            if (need[cid * noc + i] == 0)
            {
                request[i] = 0;
            }
            else
            {
                request[i] = rand()%(need[cid * noc + i]+1);
            }
        }
        printf("thread %d request :",cid);
        for (int i = 0; i < nor; i++)
        {
            printf("%4d ",request[i]);
        }
        // printf("\n");
        // sleep(1);

        request_resource(cid, request);

        if (safe())
        {
            printf("\t safe\n");
            // printinfo();
            // printf("\t%d safe\n",cid);
            int count = 0;
            for (int i = 0; i < nor; i++)
            {
                if (need[cid * noc + i] == 0)
                {
                    count++;
                }
            }
            if (count == nor) /* free all */
            {
                release_resource(cid, &allocation[cid * nor]);
                printf("---------------------------------------------thread %d finished!\n",cid);
                flag = 0;
            }
        }
        else
        {
            printf("\t unsafe\n");
            // printinfo();
            release_resource(cid, request);
        }
        pthread_mutex_unlock(&mutex);
    }
    free(request);
    // pthread_mutex_unlock(&mutexsum);
    pthread_exit(0);
}

int main(int argc, char **argv)
{
    /* initailizing */
    noc = atoi(argv[1]);
    nor = atoi(argv[2]);
    avaliable = (int *)malloc(sizeof(int) * nor);
    for (int i = 0; i < nor; i++)
        avaliable[i] = atoi(argv[i+3]);

    maximum = (int *)malloc(sizeof(int) * noc * nor);
    for (int i = 0; i < noc; i++)
        for (int j = 0; j < nor; j++)
            maximum[i * nor + j] = rand()%avaliable[i];

    allocation = (int *)malloc(sizeof(int) * noc * nor);
    memset(allocation, 0, sizeof(int) * noc * nor);

    need = (int *)malloc(sizeof(int) * noc * nor);
    for (int i = 0; i < noc; i++)
        for (int j = 0; j < nor; j++)
            need[i * nor + j] = maximum[i * nor + j];

    printinfo();
    /* initailized */

    /* pthread */
    /* the thread identifier */
    pthread_t *tid = (pthread_t *)malloc(sizeof(pthread_t) * noc);
    /* create the thread */
    int cid[noc];
    for (int i = 0; i < noc; i++)
    {
        cid[i] = i;
    }
    pthread_mutex_init(&mutexsum,NULL);
    pthread_mutex_init(&mutex,NULL);
    for (int i = 0; i < noc; i++)
        pthread_create(&tid[i], NULL, (void *)multithread, &cid[i]);
    
    for (int i = 0; i < noc; i++)
        pthread_join(tid[i], NULL);
    
    free(tid);
    free(avaliable);
    free(maximum);
    free(allocation);
    free(need);
    return 0;
}