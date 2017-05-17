#include <pthread.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>

#include "histogram.h"
#include<pthread.h>

struct pthread_args{
    int initIndex;
    int endIndex;
    block_t *blocks;
    char dummy[64];
    histrogram_t histrogram;
};

void * countingInBlocks(void * ptr){
    struct pthread_args * args=(struct pthread_args *)ptr;
    int initIndex=args->initIndex;
    int endIndex=args->endIndex;
    block_t *blocks=args->blocks;
    histrogram_t histrogram=args->histogram;
}

void get_histogram(int nBlocks, block_t *blocks, histogram_t histogram, int num_threads){
    pthread_t * thread=(pthread_t*)malloc(num_threads*sizeof(*thread));
    struct pthread_args * arg ;
    arg = (struct pthread_args *)malloc(num_threads*sizeof(*arg));
    int nBlockPThreads=nBlocks/num_threads+1;
    int resNBlocks=nBlocks%num_threads;
    for(int i=0;i<num_threads-1;i++){
        arg[i].initIndex=i*nBlockPThreads;
        arg[i].endIndex=(i+1)*nBlockPThreads-1;
        pthread_create(thread+i,NULL,&hello,arg+i);
    }
    arg[num_threads-1].initIndex=num_threads*nBlockPThreads;
    arg[num_threads-1].endIndex=num_threads*nBlockPThreads+resNBlocks;
    pthread_create(thread+i,NULL,&countingInBlock,arg+num_threads-1);
    for(int i=0;i<n_threads;i++){
        pthread_join(thread[i],NULL);  
        for(int j=0;j<NNAMES;j++){
            histogram[j]+=arg[i].histogram[j];
        }
    }
    free(thread);
    free(arg);
}

