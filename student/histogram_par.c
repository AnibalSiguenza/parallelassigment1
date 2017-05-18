#include <pthread.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>

#include "histogram.h"

struct pthread_args{
    int initIndex;
    int endIndex;
    block_t *blocks;
    char dummy[64];
    histogram_t histogram;
};

void * countingInBlocks(void * ptr){
    struct pthread_args * args=(struct pthread_args *)ptr;
    int initIndex=args->initIndex;
    int endIndex=args->endIndex;
    block_t *blocks=args->blocks;
    int * histogram=args->histogram;
	char current_word[20] = "";
	int c = 0;
    for(int i=0;i<NNAMES;i++){
        histogram[i]=0;
    }
    for(int i=initIndex;i<endIndex;i++){
        for (int j = 0; j < BLOCKSIZE; j++) {
            // if the character is an alphabet store it to form a word.
            if(isalpha(blocks[i][j])){
                current_word[c++] = blocks[i][j];
            }
            // if the character is not an alphabet it is the end of a word.
            // Compare the word with the list of names.
            else {
                current_word[c] = '\0';
                for(int k = 0; k < NNAMES; k++)	{
                    if(!strcmp(current_word, names[k]))
                        histogram[k]++;
                }
                c = 0;
            }
        }
    }
    return NULL;
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
        pthread_create(thread+i,NULL,&countingInBlocks,arg+i);
    }
    arg[num_threads-1].initIndex=num_threads*nBlockPThreads;
    arg[num_threads-1].endIndex=num_threads*nBlockPThreads+resNBlocks;
    pthread_create(thread+num_threads-1,NULL,&countingInBlocks,arg+num_threads-1);
    for(int i=0;i<num_threads;i++){
        pthread_join(thread[i],NULL);  
        for(int j=0;j<NNAMES;j++){
            histogram[j]+=arg[i].histogram[j];
        }
    }
    free(thread);
    free(arg);
}

