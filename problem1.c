#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <time.h>

int INC=0;


void* routine(void* arg){
    time_t start, end;
    start = time(NULL);
    int id = *(int*)arg;
    int a,b,c;
    int count = 0; // number of triplets found
    int low = id*INC; // lower bound on range
    if (low == 0){
        low = 1;
    }
    c = low;
    int upper = (id+1)*INC; // upper bound on range

    for( c; c<upper; c++){
        for( a = 1; a < c; a++){
            for( b = 1; b < c; b++){
                if((c*c) == ((a*a)+(b*b))){
                    count++;
                }
            }
        }
    }
    end = time(NULL);
    printf("id = %d, count = %d, elapsed time = %.0f seconds \n", id, count , difftime(end,start));
    free(arg);
}


int main(int argc, char* argv[]){   // first arg is threads second arg is increment
    int num_threads = atoi(argv[1]);
    INC = atoi(argv[2]);
    pthread_t th[num_threads];

    for( int i = 0; i < num_threads;i++){
        int* a = malloc(sizeof(int));
        *a = i;
        if(pthread_create(&th[i], NULL, &routine, a) !=0 ){
            perror("failed to create thread");
        }
    }
    for( int i = 0; i < num_threads;i++){
        if(pthread_join(th[i], NULL) !=0 ){
            perror("failed to join thread");
        }
    }
    printf("All threads completed.\n");
    return 0;
}
