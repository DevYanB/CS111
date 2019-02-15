#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <getopt.h>
#include <pthread.h>

//some globals for use
static long long counter = 0;
int niter = 1;
static char opt_sync = 'n'; //n for normal
static pthread_mutex_t lock;
static int spin_lock = 0;

//add function
int opt_yield;
void add(long long *pointer, long long value) {
  long long sum = *pointer + value;
  if (opt_yield)
    sched_yield();
  *pointer = sum;
}

//long long first_val;
//long long second_val;
//function to help the threadcaller
void add_helper(int pass){
  int k;
  for(k = 0; k < niter; k++){
    switch(opt_sync){
    case 'n':
      add(&counter, pass);
      break;
    case 'm': //mutex
      pthread_mutex_lock(&lock);
      add(&counter, pass);
      pthread_mutex_unlock(&lock);
      break;
    case 's':
      while(__sync_lock_test_and_set(&spin_lock, 1)){
	continue;
      }
      add(&counter, pass);
      __sync_lock_release(&spin_lock);
      break;
    case 'c':
      {
      long long first_val;
      long long second_val;
      while(1){
	first_val = counter;
	second_val = first_val + pass;
	if(__sync_val_compare_and_swap(&counter, first_val, second_val) == first_val){
	  break;
	}
      }
      break;
      }
    default:
      break;
    }
    //add(&counter, pass);
  }
}

//function for threads calling
void* thread_caller(){
  add_helper(1);
  add_helper(-1);
  return NULL;
}

//main code
int main(int argc , char* const argv[]){

  static struct option opts[] = {
    {"iterations", required_argument, NULL, 'i'},
    {"threads" , required_argument, NULL, 't'},
    {"yield" , no_argument, NULL, 'y'},
    {"sync", required_argument, NULL, 's'},
    {0,0,0,0}
  };

  // char opt_commands[16] = "add";

  //timing variables and structs
  struct timespec start_time;
  struct timespec end_time;

  //options for threads, iterations, and the c is for options check
  int c;
  int nthreads = 1;
  
  while((c = getopt_long(argc , argv, "", opts, 0)) != -1){
    switch(c){
    case 'i':
      niter = atoi(optarg);
      break;
    case 't':
      nthreads = atoi(optarg);
      break;
    case 'y':
      opt_yield = 1;
      break;
    case 's':
      opt_sync = optarg[0];
      if(opt_sync != 'm' && opt_sync != 's' && opt_sync != 'c'){
	fprintf(stderr, "Error with sync argument");
	exit(1);
      }
      if(opt_sync == 'm'){
	if(pthread_mutex_init(&lock, NULL)){
	  fprintf(stderr, "Error with mutex option for sync");
	  exit(1);
	}
      }
      break;
    default:
      fprintf(stderr, "Invalid argument entered\n");
      exit(1);
      break;
    }
  }

  //start timing for threads and processes
  clock_gettime(CLOCK_MONOTONIC, &start_time);

  //allocating array of thread pointers
  pthread_t* thds = (pthread_t* ) malloc(sizeof(pthread_t* ) * nthreads);
  if(thds == NULL){
    fprintf(stderr, "Problem allocating pointers to threads\n");
    exit(2);
  }

  //create threads
  int i;
  for(i = 0; i < nthreads; i++){
    if(pthread_create(&thds[i], NULL, thread_caller, NULL)){
      fprintf(stderr , "Error with creating threads\n");
      free(thds);
      exit(2);
    }
  }   
  
  //join threads
  int j;
  for(j = 0; j < nthreads; j++){
    if(pthread_join(thds[j] , NULL)){
      fprintf(stderr, "Error with joining threads\n");
      exit(2);
    }
  }
  
  //end time for threads
  clock_gettime(CLOCK_MONOTONIC, &end_time);

  //printing out commands
  long long num_ops = nthreads * niter * 2;
  long long time_elapsed_ns = (end_time.tv_sec = start_time.tv_sec);
  time_elapsed_ns += end_time.tv_nsec;
  time_elapsed_ns -= start_time.tv_nsec;
  long long  avg_time = time_elapsed_ns / num_ops;
  
  if(opt_yield == 1 && opt_sync == 'm'){
    fprintf(stdout, "%s,%d,%d,%lld,%lld,%lld,%lld\n", "add-yield-m", nthreads, niter, num_ops, time_elapsed_ns, avg_time, counter);
  }
  else if(opt_yield == 1 && opt_sync == 's'){
    fprintf(stdout, "%s,%d,%d,%lld,%lld,%lld,%lld\n", "add-yield-s", nthreads, niter, num_ops, time_elapsed_ns, avg_time, counter);
  }
  else if(opt_yield == 1 && opt_sync == 'c'){
    fprintf(stdout, "%s,%d,%d,%lld,%lld,%lld,%lld\n", "add-yield-c", nthreads, niter, num_ops, time_elapsed_ns, avg_time, counter);
  }
  else if(opt_yield == 1 && opt_sync == 'n'){
    fprintf(stdout, "%s,%d,%d,%lld,%lld,%lld,%lld\n", "add-yield-none", nthreads, niter, num_ops, time_elapsed_ns, avg_time, counter);
  }
  else if(opt_yield == 0 && opt_sync == 'm'){
    fprintf(stdout, "%s,%d,%d,%lld,%lld,%lld,%lld\n", "add-m", nthreads, niter, num_ops, time_elapsed_ns, avg_time, counter);
  }
  else if(opt_yield == 0 && opt_sync == 'c'){
    fprintf(stdout, "%s,%d,%d,%lld,%lld,%lld,%lld\n", "add-c", nthreads, niter, num_ops, time_elapsed_ns, avg_time, counter);
  }
  else if(opt_yield == 0 && opt_sync == 's'){
    fprintf(stdout, "%s,%d,%d,%lld,%lld,%lld,%lld\n", "add-s", nthreads, niter, num_ops, time_elapsed_ns, avg_time, counter);
  }
  else{
    fprintf(stdout, "%s,%d,%d,%lld,%lld,%lld,%lld\n", "add-none", nthreads, niter, num_ops, time_elapsed_ns, avg_time, counter);
  }
  
  free(thds);
  exit(0);
}     
