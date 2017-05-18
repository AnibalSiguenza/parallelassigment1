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
    char initial_list[6]={'P','A','N','D','S','B'};
    int do_search=0;
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
            // "Pierre","Andrew","Nicholas","Natasha","Napoleon","Dolokhov","Anatole","Anna","Sonya","Boris"
            else {
                for(int l=0;l<6;l++){
                    do_search+=(initial_list[l]==current_word[0]);
                }
                if(do_search){
                    current_word[c] = '\0';
                    for(int k = 0; k < NNAMES; k++)	{
                    if(!strcmp(current_word, names[k]))
                        histogram[k]++;
                    }
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
    int nBlockPThreads=(nBlocks+num_threads-1)/num_threads;
    int resNBlocks=nBlocks%nBlockPThreads; 
    printf("num_threads=%i nBlocks=%i nBlockPthreads=%i resNBlocks=%i\n",num_threads,nBlocks,nBlockPThreads,resNBlocks);
    if(resNBlocks==0){
        for(int i=0;i<num_threads;i++){
            arg[i].initIndex=i*nBlockPThreads;
            arg[i].endIndex=(i+1)*nBlockPThreads;
            arg[i].blocks=blocks;
            pthread_create(thread+i,NULL,&countingInBlocks,arg+i);
        }
    }else{
        for(int i=1;i<num_threads;i++){
            arg[i-1].initIndex=(i-1)*nBlockPThreads;
            arg[i-1].endIndex=i*nBlockPThreads;
            arg[i-1].blocks=blocks;
            pthread_create(thread+i-1,NULL,&countingInBlocks,arg+i-1);
        }
        arg[num_threads-1].initIndex=(num_threads-1)*nBlockPThreads;
        arg[num_threads-1].endIndex=(num_threads-1)*nBlockPThreads+resNBlocks;
        arg[num_threads-1].blocks=blocks;
        pthread_create(thread+num_threads-1,NULL,&countingInBlocks,arg+num_threads-1);
    }
    for(int i=0;i<num_threads;i++){
        printf("thread=%i init=%i end=%i\n",i+1,arg[i].initIndex,arg[i].endIndex);
        pthread_join(thread[i],NULL);  
        for(int j=0;j<NNAMES;j++){
            histogram[j]+=arg[i].histogram[j];
        }
    }
    free(thread);
    free(arg);
}

