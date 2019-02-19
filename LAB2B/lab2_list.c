#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <getopt.h>
#include <signal.h>
#include <time.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>
#include "SortedList.h"


int opt_sync;
int niter = 1;
int nlists = 0; //added to have multiple lists
int* lock = 0; 
pthread_mutex_t* mutex; //change to allow for many mutex's for time calc
SortedList_t *top;
SortedListElement_t *elems; 

unsigned int hash(const char *str){
  int hash = 53;
  while(*str){
    hash = hash * 28 + *str;
    str++;
  }
  return hash;
}

void* pass_to_thread(void *arg){
  int* p_num = (int *) arg; //These 2 lines are the way of interpreting void pntr as int
  int t_num = *p_num;
  //int i; //gonna try a long for indexing lol
  long start = t_num * niter;
  long i;
  long end = start + niter;
  long m_time_cum = 0;
  //unsigned long hashing_index;

  struct timespec start_time;
  struct timespec end_time;
  for (i = start; i < end; i++){
    //mutex opt
    unsigned int hashing_index = hash((elems+i)->key) % nlists; //hash index according to each i
    if(opt_sync == 'm'){
      clock_gettime(CLOCK_MONOTONIC, &start_time); //starting timing
      pthread_mutex_lock(mutex + hashing_index); //dont need ptr & b/c mutex is array
      // hashing index for loc. of appropriate thd
      //SortedList_insert(top, (elems + i));
      //pthread_mutex_unlock(&mutex);
      clock_gettime(CLOCK_MONOTONIC, &end_time);
      m_time_cum += 1000000000L * (end_time.tv_sec - start_time.tv_sec) + end_time.tv_nsec - start_time.tv_nsec;
    }
    //sync opt
    else if(opt_sync == 's'){
      while(__sync_lock_test_and_set(lock + hashing_index, 1));
    }
    SortedList_insert(top + hashing_index, (elems + i));
    //__sync_lock_release(lock + hashing_index);
    if(opt_sync == 'm'){
      pthread_mutex_unlock(mutex + hashing_index);
    }
    else if(opt_sync == 's'){
      __sync_lock_release(lock + hashing_index);
    }
    //default
    /*   else{
	 SortedList_insert(top, (elems+i)); 
	 }*/
  }


  //locks for lists
  for(i = 0; i < nlists; i++){
    if(opt_sync == 'm'){
      clock_gettime(CLOCK_MONOTONIC,&start_time);
      pthread_mutex_lock(mutex + i);
      clock_gettime(CLOCK_MONOTONIC,&end_time);
      m_time_cum += 1000000000L * (end_time.tv_sec - start_time.tv_sec) + end_time.tv_nsec - start_time.tv_nsec;
    }
    else if(opt_sync == 's'){
      while(__sync_lock_test_and_set(lock + i, 1));
    }
  }

  long list_length = 0;  
  
  for(i = 0; i < nlists; i++){
    list_length += SortedList_length(top + i);
  }
  
  //lock list release boi
  for(i = 0; i < nlists; i++){
    if(opt_sync == 'm'){
      pthread_mutex_unlock(mutex + i);
    }
    else if(opt_sync == 's'){
      __sync_lock_release(lock + i);
    }
  }

  char* checker = malloc(sizeof(char) * 6); // CHANGE NUM
  //The lookup+delete
  SortedListElement_t *visited;
  int deleted;

  for (i = t_num * niter; i < end; i++)
    {
      unsigned int hashing_index = hash((elems+i)->key) % nlists;
      strcpy(checker, (elems+i)->key);
      // Mutex lock
      if (opt_sync == 'm'){
	clock_gettime(CLOCK_MONOTONIC, &start_time);
	pthread_mutex_lock(mutex + hashing_index);
	//visited = SortedList_lookup(top, (elems + i)->key);
	//pthread_mutex_unlock(&mutex);
	//pthread_mutex_lock(muitex + hashing_index);
	//deleted = SortedList_delete(visited);
	//pthread_mutex_unlock(&mutex);
	clock_gettime(CLOCK_MONOTONIC, &end_time);
	m_time_cum += 1000000000L * (end_time.tv_sec - start_time.tv_sec) + end_time.tv_nsec - start_time.tv_nsec;
      }
      else if (opt_sync == 's'){
	while (__sync_lock_test_and_set(lock + hashing_index, 1));
      }
      visited = SortedList_lookup(top + hashing_index, checker);
      
      if(opt_sync == 'm'){
	pthread_mutex_unlock(mutex + hashing_index);
      }
      else if(opt_sync == 's'){
	__sync_lock_release(lock + hashing_index);
      }
      /*__sync_lock_release(&lock);
        while (__sync_lock_test_and_set(&lock, 1));
	deleted = SortedList_delete(visited);
        __sync_lock_release(&lock);
	}
      else {
      visited = SortedList_lookup(top, (elems + i)->key);
      deleted = SortedList_delete(visited);
      }*/
      if(visited == NULL){
	fprintf(stderr, "Error with looking at list\n");
	exit(2);
      }

      if(opt_sync == 'm'){
	clock_gettime(CLOCK_MONOTONIC, &start_time);
	pthread_mutex_lock(mutex + hashing_index);
	clock_gettime(CLOCK_MONOTONIC, &end_time);
	m_time_cum += 1000000000L * (end_time.tv_sec - start_time.tv_sec) + end_time.tv_nsec - start_time.tv_nsec;
      }
      else if(opt_sync == 's'){
	while(__sync_lock_test_and_set(lock + hashing_index, 1));
      }
      deleted = SortedList_delete(visited);

      if(opt_sync == 'm'){
	pthread_mutex_unlock(mutex + hashing_index);
      }
      else if(opt_sync == 's'){
	__sync_lock_release(lock + hashing_index);
      }
      if(deleted){
	fprintf(stderr, "Error with deleting item from list\n");
	exit(2);
      }
    }
  return (void *) m_time_cum;
}

int main (int argc, char** argv)
{
  int c = 0;    // Stores the options
  int nthds = 1;
  struct timespec start, end;
  niter = 1;
  nlists = 1;
  opt_sync = 0;
  opt_yield = 0;

    static struct option long_options[] = {
      {"iterations",  required_argument, NULL, 'i'},
      {"lists",       required_argument, NULL, 'l'},
      {"sync",        required_argument, NULL, 's'},
      {"threads",     required_argument, NULL, 't'},
      {"yield",       required_argument, NULL, 'y'},
        {0, 0, 0, 0}
    };

    while ((c = getopt_long(argc, argv, "", long_options, NULL)) != -1){
      switch (c){
	  case 'i':
	      niter = atoi(optarg);
	      break;
	  case 'l':
	      nlists = atoi(optarg);
	      break;
	  case 's':
	      opt_sync = optarg[0];
	      break;
	  case 't':
	      nthds = atoi(optarg);
	      break;
	  case 'y':
            {
	      unsigned int flag;
	      for (flag = 0; flag < strlen(optarg); flag++)
		{
		  if (optarg[flag] == 'i')
		    opt_yield = opt_yield | INSERT_YIELD;
		  else if (optarg[flag] == 'd')
		    opt_yield = opt_yield | DELETE_YIELD;
		  else if (optarg[flag] == 'l')
		    opt_yield = opt_yield | LOOKUP_YIELD;
		}
	      break;
            }
	  default:
            {
	      fprintf(stderr, "Error: incorrect arguments.\n");
	      exit(1);
	      break;
            }
	  }
      }

    top = malloc(sizeof(SortedList_t) * nlists);

    // If error in allocation
    if (!top){
	fprintf(stderr, "Error: unable to create lists.\n");
	exit(2);
      }

    long numberElems = nthds * niter;

    elems = malloc(sizeof(SortedListElement_t) * numberElems);
    
    // If error in allocation
    if (!elems){
	fprintf(stderr, "Error: unable to pre-allocate elements.\n");
	exit(2);
      }

    long itr;
    char* nKey;
      for (itr = 0; itr < numberElems; itr++){
	// Create a random key
	nKey = malloc(sizeof(char) * 6);
        if (nKey == NULL)
	  {
            fprintf(stderr, "Error: unable to create keys.\n");
            exit(2);
	  }

	nKey[0] = rand() % 254 + 1;
	nKey[1] = rand() % 254 + 1;
	nKey[2] = rand() % 254 + 1;
        nKey[3] = rand() % 254 + 1;
        nKey[4] = rand() % 254 + 1;
	nKey[5] = 0;

	// Copy the random key to the element
	(elems + itr)->key = nKey;
      }

    int* thd_num = malloc(sizeof(int) * nthds);
    if (!thd_num){
	fprintf(stderr, "Error: unable to create thread numbers.\n");
	exit(2);
      }

    // Create thread numbers to be passed as arguments in the thread function
    int i;
    for (i = 0; i < nthds; i++){
      thd_num[i] = i;
    }

    pthread_t *pthd_ids = malloc(sizeof(pthread_t) * nthds);

    // If error in allocation
    if (!pthd_ids){
        fprintf(stderr, "Error: unable to create threads.\n");
        exit(2);
      }

    // Mutex locks
    if (opt_sync == 'm'){
        // Mutex array
        mutex = malloc(sizeof(pthread_mutex_t) * nlists);

        // If error in allocation
        if (!mutex){
            fprintf(stderr, "Error: unable to create mutexes.\n");
            exit(2);
	  }

        // Have to initialize a mutex for each list
        long m = 0;
        for (m = 0; m < nlists; m++){
            // If unable to initialize a mutex lock
	  if (pthread_mutex_init((mutex + m), NULL) != 0){
                fprintf(stderr, "Error: unable to initialize mutexes.\n");
                exit(1);
	      }
	  }
      }
    // Spin locks
    else if (opt_sync == 's'){
        lock = malloc(sizeof(int) * nlists);
        // If error in allocation
        if (!lock){
            fprintf(stderr, "Error: unable to create spin locks.\n");
            exit(2);
	  }

        // Must initalize all the spin lock values to 0
        long t;
        for (t = 0; t < nlists; t++)
	  lock[t] = 0;
      }

    // Start the timer
    clock_gettime(CLOCK_MONOTONIC, &start);

    // Create threads
    for (i = 0; i < nthds; i++){
        int thd_state = pthread_create(&pthd_ids[i], NULL, &pass_to_thread, (void *)(thd_num + i));
        if (thd_state){
            fprintf(stderr, "Error: unable to create thread.\n");
            exit(1);
	  }
      }

    // Timing mutex waits
    long total_mutex_wait_time = 0;
    void* mutex_wait_time;

    // Join the threads
    for (i = 0; i < nthds; i++){
        int thd_state = pthread_join(pthd_ids[i], &mutex_wait_time);
        // If error joining a thread
        if (thd_state)
	  {
            fprintf(stderr, "Error: unable to join thread.\n");
            exit(1);
	  }
        // Add to total mutex wait time
        total_mutex_wait_time += (long) mutex_wait_time;
      }

    // Stop the timer
    clock_gettime(CLOCK_MONOTONIC, &end);
    long list_length = 0;

    // Get the length of list
    for (i = 0; i < nlists; i++){
      list_length += SortedList_length(top + i);
    }

    if (list_length != 0){
        fprintf(stderr, "Error: list length should be 0.\n");
        exit(2);
      }

    fprintf(stdout, "list");

    switch(opt_yield){
      case 0:
	fprintf(stdout, "-none");
	break;
      case 1:
	fprintf(stdout, "-i");
	break;
      case 2:
	fprintf(stdout, "-d");
	break;
      case 3:
	fprintf(stdout, "-id");
	break;
      case 4:
	fprintf(stdout, "-l");
	break;
      case 5:
	fprintf(stdout, "-il");
	break;
      case 6:
	fprintf(stdout, "-dl");
	break;
      case 7:
	fprintf(stdout, "-idl");
	break;
      default:
	break;
      }

    switch(opt_sync){
      case 0:
	fprintf(stdout, "-none");
	break;
      case 's':
	fprintf(stdout, "-s");
	break;
      case 'm':
	fprintf(stdout, "-m");
	break;
      default:
	break;
      }

    // Data used for CSV files
    long operations = nthds * niter * 3;
    long run_time = 1000000000L * (end.tv_sec - start.tv_sec) + end.tv_nsec - start.tv_nsec;
    long avg_time_ops = run_time / operations;
    long avg_mutex_wait = total_mutex_wait_time / operations;
    // Printing for mutex wait times
    if (opt_sync == 'm' || opt_sync == 's'){
        fprintf(stdout, ",%d,%d,%d,%ld,%ld,%ld,%ld\n", nthds, niter, nlists, operations, run_time, avg_time_ops, avg_mutex_wait);
      }
    // Printing for regular data
    else        {
      fprintf(stdout, ",%d,%d,%d,%ld,%ld,%ld\n", nthds, niter, nlists, operations, run_time, avg_time_ops);
    }

    // Free everything
    free(pthd_ids);
    free(top);
    free(elems);
    free(thd_num);

    exit(0);
}
