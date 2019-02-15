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


char opt_sync;
int niter = 1;
int lock = 0;
pthread_mutex_t mutex;
SortedList_t *top;
SortedListElement_t *elems; 

void* pass_to_thread(void *arg)
{
  int* p_num = (int *) arg;
  
  int i;
 
  for (i = (*p_num * niter); i < (*p_num * niter + niter); i++){
    //mutex opt
      if(opt_sync == 'm'){
	pthread_mutex_lock(&mutex);
	SortedList_insert(top, (elems + i));
	pthread_mutex_unlock(&mutex);
      }
      //sync opt
      else if(opt_sync == 's'){
	while(__sync_lock_test_and_set(&lock, 1));
	SortedList_insert(top, (elems + i));
	__sync_lock_release(&lock);
      }
      //default
      else{
	SortedList_insert(top, (elems+i)); 
      }
    }

  //mutex opt
  if(opt_sync == 'm'){
    pthread_mutex_lock(&mutex);
    if(SortedList_length(top) < 0){
      fprintf(stderr, "Error with finding length of list");
      exit(2); 
    }
    pthread_mutex_unlock(&mutex);
  }
  //sync opt 
  else if(opt_sync == 's'){
    while(__sync_lock_test_and_set(&lock, 1) == 1);
    if(SortedList_length(top)< 0){
      fprintf(stderr, "Error with finding length of list\n");
      exit(2);
    }
    __sync_lock_release(&lock); 
  }
  //default 
  else{
    if(SortedList_length(top)< 0){
      fprintf(stderr, "Error with finding length of list\n");
      exit(2);
    }
  }

  //The lookup+delete
  SortedListElement_t *visited;
  int deleted;

  for (i = (*p_num * niter); i < (*p_num * niter + niter); i++)
    {
      // Mutex lock
      if (opt_sync == 'm'){
	pthread_mutex_lock(&mutex);
	visited = SortedList_lookup(top, (elems + i)->key);
	pthread_mutex_unlock(&mutex);
	pthread_mutex_lock(&mutex);
	deleted = SortedList_delete(visited);
	pthread_mutex_unlock(&mutex);
      }
      else if (opt_sync == 's'){
	while (__sync_lock_test_and_set(&lock, 1));
	visited = SortedList_lookup(top, (elems + i)->key);
	__sync_lock_release(&lock);
        while (__sync_lock_test_and_set(&lock, 1));
	deleted = SortedList_delete(visited);
        __sync_lock_release(&lock);
      }
      else {
	visited = SortedList_lookup(top, (elems + i)->key);
	deleted = SortedList_delete(visited);
      }
      if(visited == NULL){
	fprintf(stderr, "Error with looking at list\n");
	exit(2);
      }

      if(deleted){
	fprintf(stderr, "Error with deleting item from list\n");
	exit(2);
      }
    }
  return NULL;
}

int main (int argc, char** argv)
{
  int num_threads = 1;
  opt_yield = 0;
  opt_sync = 0; 
  int c = 0;
  

  static struct option opts[] = {
    {"iterations", required_argument, 0, 'i'},
    {"sync", required_argument, 0, 's'},
    {"threads", required_argument, 0, 't'},
    {"yield", required_argument, 0, 'y'},
    {0, 0, 0, 0}
  };

  while ((c = getopt_long(argc, argv, "", opts, NULL)) != -1){
      switch (c){
	case 'i':
	    niter = atoi(optarg);
	    break;
	case 't':
	    num_threads = atoi(optarg);
	    break;
	case 's':
	    opt_sync = optarg[0];
	    break;
	case 'y':
	  {
	    int lengthOpt = strlen(optarg);
	    int r;
	    for (r = 0; r < lengthOpt; r++)
	      {
		if(optarg[r] == 'i')
		  opt_yield = opt_yield | INSERT_YIELD;
		else if(optarg[r] == 'd')
		  opt_yield = opt_yield | DELETE_YIELD;
		else if(optarg[r] == 'l')
		  opt_yield = opt_yield | LOOKUP_YIELD;
		else {
		  fprintf(stderr, "Yield has incorrect option\n");
		  break; }
	      }
	    break;
	  }
	default:
	    fprintf(stderr, "Incorrect option passed\n");
	    exit(1);
	    break; 
      }
  }

  top = malloc(sizeof(SortedList_t));

  if (!top){
      fprintf(stderr, "Error: unable to create list.\n");
      exit(2);
    }

  long numberElements = num_threads * niter;

  elems = malloc(sizeof(SortedListElement_t) * numberElements);
    
  if (!elems)
    {
      fprintf(stderr, "Error: unable to allocate elements.\n");
      exit(2);
    }

  long n;
  for (n = 0; n < numberElements; n++)
    {
 
      char *newKey = malloc(sizeof(char));
      if (newKey == NULL)
	{
	  fprintf(stderr, "Error: unable to create keys.\n");
	  exit(1);
	}

      newKey[0] = rand() % 256;
      newKey[1] = rand() % 256;
      newKey[2] = rand() % 256;
      newKey[3] = rand() % 256;
      newKey[4] = rand() % 256;
      newKey[5] = 0;

       (elems + n)->key = newKey;
    }

  int *thread_num = malloc(sizeof(int) * num_threads);
  if (!thread_num){
      fprintf(stderr, "Error: unable to create thread numbers.\n");
      exit(2);
    }

 
  int i;
  for (i = 0; i < num_threads; i++)
    thread_num[i] = i;

 
  struct timespec start_time;
  if (clock_gettime(CLOCK_MONOTONIC, &start_time) == -1){
      fprintf(stderr, "Error: unable to start time.\n");
      exit(1);
    }

  pthread_t *thds = malloc(sizeof(pthread_t) * num_threads);
  if (!thds){
      fprintf(stderr, "Error: unable to create threads.\n");
      exit(1);
    }

 
  if (opt_sync == 'm'){
      if (pthread_mutex_init(&mutex, NULL) != 0){
	  fprintf(stderr, "Error: unable to create mutex.\n");
	  exit(1);
	}
    }

 
  for (i = 0; i < num_threads; i++){
      int thread_status = pthread_create(&thds[i], NULL, &pass_to_thread, (void *)(thread_num + i));
      if (thread_status){
	  fprintf(stderr, "Error: unable to create thread.\n");
	  exit(1);
	}
    }
 
  for (i = 0; i < num_threads; i++){
      int thread_status = pthread_join(thds[i], NULL);
      if (thread_status){
	  fprintf(stderr, "Error: unable to join thread.\n");
	  exit(1);
	}
    }


  struct timespec end_time; 
  if (clock_gettime(CLOCK_MONOTONIC, &end_time) == -1){
      fprintf(stderr, "Error: unable to stop time.\n");
      exit(1);
    }


  long list_length = SortedList_length(top);

  if (list_length != 0){
      fprintf(stderr, "Error: list length should be 0.\n");
      exit(1);
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


  long long operations = num_threads * niter * 3;
  long long time_elapsed_ns = (end_time.tv_sec = start_time.tv_sec);
  long long avg_time = time_elapsed_ns / operations;
  long long nlist = 1;

  fprintf(stdout, ",%d,%d,%lld,%lld,%lld,%lld\n", num_threads, niter, nlist, operations, time_elapsed_ns, avg_time);

  free(thds);
  free(top);
  free(elems);
  free(thread_num);

  exit(0);
}
