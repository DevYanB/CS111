//NAME: Devyan Biswas
//EMAIL: devyanbiswas@outlook.com
//ID: 

#include <sys/resource.h>
#include <stdio.h>
#include <unistd.h>
#include <getopt.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <errno.h>

int errCode = 0;

//to handle errors
void catchHandler(int e){
  fprintf(stderr, "%d caught\n" , e);
  exit(e);
}

//struct for holding information about processes; useful for wait and command!
typedef struct{
  pid_t process;
  char** pString;
  int size;
} proStrings;

int main(int argc, char * const argv[])
{
    static struct option opts[]= 
      {
        {"rdonly", required_argument, NULL, 'R'},
        {"wronly", required_argument, NULL, 'W'},
        {"verbose", no_argument, NULL, 'V'},
        {"command", required_argument, NULL, 'C'},
	{"append", no_argument, NULL, 'a'},
	{"cloexec", no_argument, NULL, 'c'},
	{"creat", no_argument, NULL, 'q'},
	{"directory", no_argument, NULL, 'd'},
	{"dsync" , no_argument, NULL, 'y'},
	{"excl" , no_argument, NULL, 'e'},
	{"nofollow", no_argument, NULL, 'n'},
	{"nonblock", no_argument, NULL, 'N'},
	{"rsync", no_argument, NULL, 'r'},
	{"sync" , no_argument, NULL, 's'},
	{"trunc", no_argument, NULL, 't'},
	{"rdwr", required_argument, NULL, 'x'},
	{"pipe", no_argument, NULL, 'p'},
	{"abort", no_argument, NULL, 'f'},
	{"default", required_argument, NULL, 'D'},
	{"catch", required_argument, NULL, 'A'},
	{"pause", no_argument, NULL, 'P'},
	{"ignore", required_argument, NULL, 'I'},
	{"wait" , no_argument, NULL, 'w'},
	{"close" , required_argument, NULL, 'E'},
	{"profile" , no_argument, NULL, 'o'},
        {0,0,0,0}
      };

    ///--->DEFINING STUFF<---

    //max exit code and max signal code variables
    int maxSigCode = 0;
    int maxExCode = 0;

    //Struct for use in profile and for getrusage()
    struct rusage usage;
    int usageStart;
    int usageEnd;
    //more info for profile
    struct timeval startingUserTime;
    struct timeval startingSystemTime;
    struct timeval endingUserTime;
    struct timeval endingSystemTime;
    long int diffUs;
    long int diffUms;
    long int diffSs;
    long int diffSms;
    
    //maitnain processes and file descriptors
    int fdPlace = 0;
    int* fdArr = (int*) malloc(25 * sizeof(int));    
    int ppos = 0;
    pid_t* pArr = (pid_t*) malloc(25 * sizeof(pid_t));
    if(!fdArr){
      fprintf(stderr, "Error in allocating memory for file descriptor array");
      exit(1);
    }
    if(!pArr){
      fprintf(stderr, "Error in allocating memory for process array");
      exit(1);
    }
    
    //dynamic memory for options
    proStrings* bork = malloc(sizeof(proStrings*) * 50);
    int bPlace = 0;

    int i; //keeps track of...something in command...number of options i think
    int lastPpos = 0; //keep track of the last ppos for the wait command to use    
    int c; //also keeps track of...something

    //command flags, verbose and profile for their respective commands
    int verboseFlag = 0;
    int profileFlag = 0;
    int flags = 0;
    
    //For the simple options: 
    //sig is signal and
    //toClose is for indexing files to close
    int sig;
    int toClose;

    //--->MAIN SECTION OF CODE, USING LOOP INSTEAD OF SWITCH BECAUSE...REASONS...<---
    while((c=getopt_long(argc, argv, "", opts, NULL)) != -1){
      //--append
      if(c == 'a'){
	if(verboseFlag == 1){
	  printf("%s\n", argv[optind - 1]);
	}
        if(profileFlag == 1){
          usageStart = getrusage(RUSAGE_SELF, &usage);
	  if(usageStart < 0){
	    fprintf(stderr , "Problem with getrusage");
	    errCode = 1;
	  }
          startingUserTime = usage.ru_utime;
          startingSystemTime= usage.ru_stime;
        }
	flags |= O_APPEND;
        if(profileFlag == 1){
          usageEnd = getrusage(RUSAGE_SELF, &usage);
	  if(usageEnd < 0){
            fprintf(stderr , "Problem with getrusage");
            errCode = 1;
          }
	  endingUserTime = usage.ru_utime;
          endingSystemTime = usage.ru_stime;
          diffUs = (long int) (endingUserTime.tv_sec - startingUserTime.tv_sec);
          diffUms = (long int) (endingUserTime.tv_usec - startingUserTime.tv_usec);
          diffSs = (long int) (endingSystemTime.tv_sec - startingSystemTime.tv_sec);
          diffSms = (long int) (endingSystemTime.tv_usec - startingSystemTime.tv_usec);
          printf("User Time: %ld.%.6lds | System Time: %ld.%.6lds\n" , diffUs, diffUms, diffSs, diffSms);
        }
      }
      //--cloexec
      else if(c == 'c'){
        if(verboseFlag == 1){
          printf("%s\n", argv[optind - 1]);
        }
        if(profileFlag == 1){
          usageStart = getrusage(RUSAGE_SELF, &usage);
          startingUserTime = usage.ru_utime;
          startingSystemTime= usage.ru_stime;
        }
        flags |= O_CLOEXEC;
        if(profileFlag == 1){
          usageEnd = getrusage(RUSAGE_SELF, &usage);
          endingUserTime = usage.ru_utime;
          endingSystemTime = usage.ru_stime;
          diffUs = (long int) (endingUserTime.tv_sec - startingUserTime.tv_sec);
          diffUms = (long int) (endingUserTime.tv_usec - startingUserTime.tv_usec);
          diffSs = (long int) (endingSystemTime.tv_sec - startingSystemTime.tv_sec);
          diffSms = (long int) (endingSystemTime.tv_usec - startingSystemTime.tv_usec);
          printf("User Time: %ld.%.6lds | System Time: %ld.%.6lds\n" , diffUs, diffUms, diffSs, diffSms);
        }
      }
      //--creat
      else if(c == 'q'){
        if(verboseFlag == 1){
          printf("%s\n", argv[optind - 1]);
        }
        if(profileFlag == 1){
          usageStart = getrusage(RUSAGE_SELF, &usage);
          startingUserTime = usage.ru_utime;
          startingSystemTime= usage.ru_stime;
        }
        flags |= O_CREAT;
        if(profileFlag == 1){
          usageEnd = getrusage(RUSAGE_SELF, &usage);
          endingUserTime = usage.ru_utime;
          endingSystemTime = usage.ru_stime;
          diffUs = (long int) (endingUserTime.tv_sec - startingUserTime.tv_sec);
          diffUms = (long int) (endingUserTime.tv_usec - startingUserTime.tv_usec);
          diffSs = (long int) (endingSystemTime.tv_sec - startingSystemTime.tv_sec);
          diffSms = (long int) (endingSystemTime.tv_usec - startingSystemTime.tv_usec);
          printf("User Time: %ld.%.6lds | System Time: %ld.%.6lds\n" , diffUs, diffUms, diffSs, diffSms);
        }
      }
      //--directory
      else if(c == 'd'){
        if(verboseFlag == 1){
          printf("%s\n", argv[optind - 1]);
        }
        if(profileFlag == 1){
          usageStart = getrusage(RUSAGE_SELF, &usage);
          startingUserTime = usage.ru_utime;
          startingSystemTime= usage.ru_stime;
        }
        flags |= O_DIRECTORY;
        if(profileFlag == 1){
          usageEnd = getrusage(RUSAGE_SELF, &usage);
          endingUserTime = usage.ru_utime;
          endingSystemTime = usage.ru_stime;
          diffUs = (long int) (endingUserTime.tv_sec - startingUserTime.tv_sec);
          diffUms = (long int) (endingUserTime.tv_usec - startingUserTime.tv_usec);
          diffSs = (long int) (endingSystemTime.tv_sec - startingSystemTime.tv_sec);
          diffSms = (long int) (endingSystemTime.tv_usec - startingSystemTime.tv_usec);
          printf("User Time: %ld.%.6lds | System Time: %ld.%.6lds\n" , diffUs, diffUms, diffSs, diffSms);
        }
      }
      //--dsync
      else if(c == 'y'){
        if(verboseFlag == 1){
          printf("%s\n", argv[optind - 1]);
        }
        if(profileFlag == 1){
          usageStart = getrusage(RUSAGE_SELF, &usage);
          startingUserTime = usage.ru_utime;
          startingSystemTime= usage.ru_stime;
        }
        flags |= O_DSYNC;
        if(profileFlag == 1){
          usageEnd = getrusage(RUSAGE_SELF, &usage);
          endingUserTime = usage.ru_utime;
          endingSystemTime = usage.ru_stime;
          diffUs = (long int) (endingUserTime.tv_sec - startingUserTime.tv_sec);
          diffUms = (long int) (endingUserTime.tv_usec - startingUserTime.tv_usec);
          diffSs = (long int) (endingSystemTime.tv_sec - startingSystemTime.tv_sec);
          diffSms = (long int) (endingSystemTime.tv_usec - startingSystemTime.tv_usec);
          printf("User Time: %ld.%.6lds | System Time: %ld.%.6lds\n" , diffUs, diffUms, diffSs, diffSms);
        }
      }
      //--excl
      else if(c == 'e'){
        if(verboseFlag == 1){
          printf("%s\n", argv[optind - 1]);
        }
	if(profileFlag == 1){
          usageStart = getrusage(RUSAGE_SELF, &usage);
          startingUserTime = usage.ru_utime;
          startingSystemTime= usage.ru_stime;
        }
        flags |= O_EXCL;
        if(profileFlag == 1){
          usageEnd = getrusage(RUSAGE_SELF, &usage);
          endingUserTime = usage.ru_utime;
          endingSystemTime = usage.ru_stime;
          diffUs = (long int) (endingUserTime.tv_sec - startingUserTime.tv_sec);
          diffUms = (long int) (endingUserTime.tv_usec - startingUserTime.tv_usec);
          diffSs = (long int) (endingSystemTime.tv_sec - startingSystemTime.tv_sec);
          diffSms = (long int) (endingSystemTime.tv_usec - startingSystemTime.tv_usec);
          printf("User Time: %ld.%.6lds | System Time: %ld.%.6lds\n" , diffUs, diffUms, diffSs, diffSms);
        }
      }
      //--nofollow
      else if(c == 'n'){
        if(verboseFlag == 1){
          printf("%s\n", argv[optind - 1]);
        }
        if(profileFlag == 1){
          usageStart = getrusage(RUSAGE_SELF, &usage);
          startingUserTime = usage.ru_utime;
          startingSystemTime= usage.ru_stime;
        }
        flags |= O_NOFOLLOW;
        if(profileFlag == 1){
          usageEnd = getrusage(RUSAGE_SELF, &usage);
          endingUserTime = usage.ru_utime;
          endingSystemTime = usage.ru_stime;
          diffUs = (long int) (endingUserTime.tv_sec - startingUserTime.tv_sec);
          diffUms = (long int) (endingUserTime.tv_usec - startingUserTime.tv_usec);
          diffSs = (long int) (endingSystemTime.tv_sec - startingSystemTime.tv_sec);
          diffSms = (long int) (endingSystemTime.tv_usec - startingSystemTime.tv_usec);
          printf("User Time: %ld.%.6lds | System Time: %ld.%.6lds\n" , diffUs, diffUms, diffSs, diffSms);
        }
      }
      //--nonblock
      else if(c == 'N'){
        if(verboseFlag == 1){
          printf("%s\n", argv[optind - 1]);
        }
        if(profileFlag == 1){
          usageStart = getrusage(RUSAGE_SELF, &usage);
          startingUserTime = usage.ru_utime;
          startingSystemTime= usage.ru_stime;
        }
        flags |= O_NONBLOCK;
        if(profileFlag == 1){
          usageEnd = getrusage(RUSAGE_SELF, &usage);
          endingUserTime = usage.ru_utime;
          endingSystemTime = usage.ru_stime;
          diffUs = (long int) (endingUserTime.tv_sec - startingUserTime.tv_sec);
          diffUms = (long int) (endingUserTime.tv_usec - startingUserTime.tv_usec);
          diffSs = (long int) (endingSystemTime.tv_sec - startingSystemTime.tv_sec);
          diffSms = (long int) (endingSystemTime.tv_usec - startingSystemTime.tv_usec);
          printf("User Time: %ld.%.6lds | System Time: %ld.%.6lds\n" , diffUs, diffUms, diffSs, diffSms);
        }
      }
      //--rsync
      else if(c == 'r'){
        if(verboseFlag == 1){
          printf("%s\n", argv[optind - 1]);
        }
	if(profileFlag == 1){
          usageStart = getrusage(RUSAGE_SELF, &usage);
          startingUserTime = usage.ru_utime;
          startingSystemTime= usage.ru_stime;
        }
        flags |= O_RSYNC;
        if(profileFlag == 1){
          usageEnd = getrusage(RUSAGE_SELF, &usage);
          endingUserTime = usage.ru_utime;
          endingSystemTime = usage.ru_stime;
          diffUs = (long int) (endingUserTime.tv_sec - startingUserTime.tv_sec);
          diffUms = (long int) (endingUserTime.tv_usec - startingUserTime.tv_usec);
          diffSs = (long int) (endingSystemTime.tv_sec - startingSystemTime.tv_sec);
          diffSms = (long int) (endingSystemTime.tv_usec - startingSystemTime.tv_usec);
          printf("User Time: %ld.%.6lds | System Time: %ld.%.6lds\n" , diffUs, diffUms, diffSs, diffSms);
        }
      }
      //--sync
      else if(c == 's'){
        if(verboseFlag == 1){
          printf("%s\n", argv[optind - 1]);
        }
        if(profileFlag == 1){
          usageStart = getrusage(RUSAGE_SELF, &usage);
          startingUserTime = usage.ru_utime;
          startingSystemTime= usage.ru_stime;
        }
        flags |= O_SYNC;
        if(profileFlag == 1){
          usageEnd = getrusage(RUSAGE_SELF, &usage);
          endingUserTime = usage.ru_utime;
          endingSystemTime = usage.ru_stime;
          diffUs = (long int) (endingUserTime.tv_sec - startingUserTime.tv_sec);
          diffUms = (long int) (endingUserTime.tv_usec - startingUserTime.tv_usec);
          diffSs = (long int) (endingSystemTime.tv_sec - startingSystemTime.tv_sec);
          diffSms = (long int) (endingSystemTime.tv_usec - startingSystemTime.tv_usec);
          printf("User Time: %ld.%.6lds | System Time: %ld.%.6lds\n" , diffUs, diffUms, diffSs, diffSms);
        }
      }
      //--trunc
      else if(c == 't'){
        if(verboseFlag == 1){
          printf("%s\n", argv[optind - 1]);
        }
	if(profileFlag == 1){
          usageStart = getrusage(RUSAGE_SELF, &usage);
          startingUserTime = usage.ru_utime;
          startingSystemTime= usage.ru_stime;
        }
        flags |= O_TRUNC;
        if(profileFlag == 1){
          usageEnd = getrusage(RUSAGE_SELF, &usage);
          endingUserTime = usage.ru_utime;
          endingSystemTime = usage.ru_stime;
          diffUs = (long int) (endingUserTime.tv_sec - startingUserTime.tv_sec);
          diffUms = (long int) (endingUserTime.tv_usec - startingUserTime.tv_usec);
          diffSs = (long int) (endingSystemTime.tv_sec - startingSystemTime.tv_sec);
          diffSms = (long int) (endingSystemTime.tv_usec - startingSystemTime.tv_usec);
          printf("User Time: %ld.%.6lds | System Time: %ld.%.6lds\n" , diffUs, diffUms, diffSs, diffSms);
        }
      }
      //--abort
      else if(c == 'f'){
	if(verboseFlag == 1){
          printf("%s\n", argv[optind - 1]);
        }
	if(profileFlag == 1){
          usageStart = getrusage(RUSAGE_SELF, &usage);
          startingUserTime = usage.ru_utime;
          startingSystemTime= usage.ru_stime;
        }
	fflush(stdout);
	if(raise(SIGSEGV) != 0){
	  fprintf(stderr , "Error aborting code");
	}
	if(profileFlag == 1){
          usageEnd = getrusage(RUSAGE_SELF, &usage);
          endingUserTime = usage.ru_utime;
          endingSystemTime = usage.ru_stime;
          diffUs = (long int) (endingUserTime.tv_sec - startingUserTime.tv_sec);
          diffUms = (long int) (endingUserTime.tv_usec - startingUserTime.tv_usec);
          diffSs = (long int) (endingSystemTime.tv_sec - startingSystemTime.tv_sec);
          diffSms = (long int) (endingSystemTime.tv_usec - startingSystemTime.tv_usec);
          printf("User Time: %ld.%.6lds | System Time: %ld.%.6lds\n" , diffUs, diffUms, diffSs, diffSms);
        }
      }
      //--default
      else if(c == 'D'){
	if(verboseFlag == 1){
          printf("%s %s\n", argv[optind-2] , argv[optind - 1]);
        }
        if(profileFlag == 1){
          usageStart = getrusage(RUSAGE_SELF, &usage);
          startingUserTime = usage.ru_utime;
          startingSystemTime= usage.ru_stime;
        }
	sig = atoi(argv[optind-1]);
	if(signal(sig, SIG_DFL) == SIG_ERR){
	  fprintf(stderr, "Problem with default behavior");
	  errCode = 1;
	}
        if(profileFlag == 1){
          usageEnd = getrusage(RUSAGE_SELF, &usage);
          endingUserTime = usage.ru_utime;
          endingSystemTime = usage.ru_stime;
          diffUs = (long int) (endingUserTime.tv_sec - startingUserTime.tv_sec);
          diffUms = (long int) (endingUserTime.tv_usec - startingUserTime.tv_usec);
          diffSs = (long int) (endingSystemTime.tv_sec - startingSystemTime.tv_sec);
          diffSms = (long int) (endingSystemTime.tv_usec - startingSystemTime.tv_usec);
          printf("User Time: %ld.%.6lds | System Time: %ld.%.6lds\n" , diffUs, diffUms, diffSs, diffSms);
        }
      }
      //--catch
      else if(c == 'A'){
	if(verboseFlag == 1){
          printf("%s %s\n", argv[optind-2] , argv[optind - 1]);
        }
	if(profileFlag == 1){
          usageStart = getrusage(RUSAGE_SELF, &usage);
          startingUserTime = usage.ru_utime;
          startingSystemTime= usage.ru_stime;
	}
	sig = atoi(argv[optind-1]);
	if(signal(sig, catchHandler) == SIG_ERR){
	  fprintf(stderr, "Problem with catching a signal");
	  errCode = 1;
	}
	if(profileFlag == 1){
          usageEnd = getrusage(RUSAGE_SELF, &usage);
          endingUserTime = usage.ru_utime;
          endingSystemTime = usage.ru_stime;
          diffUs = (long int) (endingUserTime.tv_sec - startingUserTime.tv_sec);
          diffUms = (long int) (endingUserTime.tv_usec - startingUserTime.tv_usec);
          diffSs = (long int) (endingSystemTime.tv_sec - startingSystemTime.tv_sec);
          diffSms = (long int) (endingSystemTime.tv_usec - startingSystemTime.tv_usec);
          printf("User Time: %ld.%.6lds | System Time: %ld.%.6lds\n" , diffUs, diffUms, diffSs, diffSms);
        }
      }
      //--pause
      else if(c == 'P'){
	if(verboseFlag == 1){
          printf("%s\n", argv[optind - 1]);
	}
	if(profileFlag == 1){
          usageStart = getrusage(RUSAGE_SELF, &usage);
          startingUserTime = usage.ru_utime;
          startingSystemTime= usage.ru_stime;
        }
	pause();
	if(profileFlag == 1){
          usageEnd = getrusage(RUSAGE_SELF, &usage);
          endingUserTime = usage.ru_utime;
          endingSystemTime = usage.ru_stime;
          diffUs = (long int) (endingUserTime.tv_sec - startingUserTime.tv_sec);
          diffUms = (long int) (endingUserTime.tv_usec - startingUserTime.tv_usec);
          diffSs = (long int) (endingSystemTime.tv_sec - startingSystemTime.tv_sec);
          diffSms = (long int) (endingSystemTime.tv_usec - startingSystemTime.tv_usec);
          printf("User Time: %ld.%.6lds | System Time: %ld.%.6lds\n" , diffUs, diffUms, diffSs, diffSms);
        }
      }
      //--ignore
      else if(c == 'I'){
	if(verboseFlag == 1){
          printf("%s %s\n", argv[optind-2] , argv[optind - 1]);
        }
	if(profileFlag == 1){
          usageStart = getrusage(RUSAGE_SELF, &usage);
          startingUserTime = usage.ru_utime;
          startingSystemTime= usage.ru_stime;
        }
	sig = atoi(argv[optind-1]);
	if(signal(sig, SIG_IGN) == SIG_ERR){
          fprintf(stderr, "Problem with catching a signal");
          errCode = 1;
	}
	if(profileFlag == 1){
          usageEnd = getrusage(RUSAGE_SELF, &usage);
          endingUserTime = usage.ru_utime;
          endingSystemTime = usage.ru_stime;
          diffUs = (long int) (endingUserTime.tv_sec - startingUserTime.tv_sec);
          diffUms = (long int) (endingUserTime.tv_usec - startingUserTime.tv_usec);
          diffSs = (long int) (endingSystemTime.tv_sec - startingSystemTime.tv_sec);
          diffSms = (long int) (endingSystemTime.tv_usec - startingSystemTime.tv_usec);
          printf("User Time: %ld.%.6lds | System Time: %ld.%.6lds\n" , diffUs, diffUms, diffSs, diffSms);
        }
      }
      //--profile
      else if (c == 'o'){
	if(verboseFlag ==1){
	  printf("%s" , argv[optind-1]);
	}
	profileFlag = 1;
      }
      //--wait
      else if (c == 'w'){
	if(verboseFlag == 1){
          printf("%s\n", argv[optind - 1]);
        }
	if(profileFlag == 1){
          usageStart = getrusage(RUSAGE_SELF, &usage);
          startingUserTime = usage.ru_utime;
          startingSystemTime= usage.ru_stime;
        }
	int counter = lastPpos; //to prevent repeat print outs
	int proV;
	for(; counter < ppos; counter++){
	  int status;
	  proV = wait(&status);
	  //either the process exits, or raises a signal
	  if(WIFEXITED(status)){
	    int ec = WEXITSTATUS(status);
	    printf("exit %d" , ec);
	    if(ec > maxExCode){
	      maxExCode = ec;
	    }
	  }
	  else if(WIFSIGNALED(status)){
	    int sc = WTERMSIG(status);
	    printf("signal %d" , WTERMSIG(status));
	    if(sc > maxSigCode){
	      maxSigCode = sc;
	    }
	  }
	  //goes through and prints out the actual options of the appropriate process
	  int temp = 0;
	  for(; temp < bPlace; temp++){
	    if(proV == bork[temp].process){
	      int tol = 0;
	      for(; tol < bork[temp].size; tol++){
		printf(" %s" , bork[temp].pString[tol]);
	      }
	      break;
	    }
	  }
	  printf("\n");
      	}
	//for wait, will also have to print out time for the child processes!
	//normal
	if(profileFlag == 1){
          usageEnd = getrusage(RUSAGE_SELF, &usage);
          endingUserTime = usage.ru_utime;
          endingSystemTime = usage.ru_stime;
          diffUs = (long int) (endingUserTime.tv_sec - startingUserTime.tv_sec);
          diffUms = (long int) (endingUserTime.tv_usec - startingUserTime.tv_usec);
          diffSs = (long int) (endingSystemTime.tv_sec - startingSystemTime.tv_sec);
          diffSms = (long int) (endingSystemTime.tv_usec - startingSystemTime.tv_usec);
          printf("User Time: %ld.%.6lds | System Time: %ld.%.6lds\n" , diffUs, diffUms, diffSs, diffSms);
        }
	//child
        if(profileFlag == 1){
          usageEnd = getrusage(RUSAGE_CHILDREN, &usage);
          endingUserTime = usage.ru_utime;
          endingSystemTime = usage.ru_stime;
          diffUs = (long int) (endingUserTime.tv_sec - startingUserTime.tv_sec);
	  diffUms = (long int) (endingUserTime.tv_usec - startingUserTime.tv_usec);
	  diffSs = (long int) (endingSystemTime.tv_sec - startingSystemTime.tv_sec);
	  diffSms = (long int) (endingSystemTime.tv_usec - startingSystemTime.tv_usec);
          printf("Babbi User Time: %ld.%.6lds | Babbi System Time: %ld.%.6lds\n" , diffUs, diffUms, diffSs, diffSms);
        }
	lastPpos = ppos;
      }
      //--close
      else if (c == 'E'){
	if(verboseFlag == 1){
          printf("%s %s\n", argv[optind-2] , argv[optind - 1]);
        }
	if(profileFlag == 1){
          usageStart = getrusage(RUSAGE_SELF, &usage);
          startingUserTime = usage.ru_utime;
          startingSystemTime= usage.ru_stime;
        }
	toClose = atoi(argv[optind-1]);
	close(fdArr[toClose]);
	fdArr[toClose] = -1;
	if(profileFlag == 1){
          usageEnd = getrusage(RUSAGE_SELF, &usage);
          endingUserTime = usage.ru_utime;
          endingSystemTime = usage.ru_stime;
          diffUs = (long int) (endingUserTime.tv_sec - startingUserTime.tv_sec);
          diffUms = (long int) (endingUserTime.tv_usec - startingUserTime.tv_usec);
          diffSs = (long int) (endingSystemTime.tv_sec - startingSystemTime.tv_sec);
          diffSms = (long int) (endingSystemTime.tv_usec - startingSystemTime.tv_usec);
          printf("User Time: %ld.%.6lds | System Time: %ld.%.6lds\n" , diffUs, diffUms, diffSs, diffSms);
        }
      }
      //--rdonly
      else if(c == 'R'){
	if(verboseFlag ==1){
	  printf("%s %s\n" , argv[optind-2], argv[optind-1]);
	}
	if(profileFlag == 1){
          usageStart = getrusage(RUSAGE_SELF, &usage);
          startingUserTime = usage.ru_utime;
          startingSystemTime= usage.ru_stime;
        }
	flags |= O_RDONLY;
	fdArr[fdPlace] = open(argv[optind-1], flags, 0644);
	if(fdArr[fdPlace] == -1){
	  fprintf(stderr, "Error in opening specified file for r");
	  
	  errCode = 1;
	}
	if(profileFlag == 1){
          usageEnd = getrusage(RUSAGE_SELF, &usage);
          endingUserTime = usage.ru_utime;
          endingSystemTime = usage.ru_stime;
          diffUs = (long int) (endingUserTime.tv_sec - startingUserTime.tv_sec);
          diffUms = (long int) (endingUserTime.tv_usec - startingUserTime.tv_usec);
          diffSs = (long int) (endingSystemTime.tv_sec - startingSystemTime.tv_sec);
          diffSms = (long int) (endingSystemTime.tv_usec - startingSystemTime.tv_usec);
          printf("User Time: %ld.%.6lds | System Time: %ld.%.6lds\n" , diffUs, diffUms, diffSs, diffSms);
        }
	fdPlace++;
	flags = 0;
      }
      //--wronly
      else if(c == 'W') {
	if(verboseFlag == 1){
	  printf( "%s %s\n" , argv[optind-2], argv[optind-1]);
	}
	if(profileFlag == 1){
          usageStart = getrusage(RUSAGE_SELF, &usage);
          startingUserTime = usage.ru_utime;
          startingSystemTime= usage.ru_stime;
        }

	flags |= O_WRONLY;
	fdArr[fdPlace] = open(argv[optind-1], flags, 0644);
	if(fdArr[fdPlace] == -1){
	  fprintf(stderr,"Error in opening specified file for w");
	  errCode = 1;
	}

	if(profileFlag == 1){
          usageEnd = getrusage(RUSAGE_SELF, &usage);
	  endingUserTime = usage.ru_utime;
          endingSystemTime = usage.ru_stime;
          diffUs = (long int) (endingUserTime.tv_sec - startingUserTime.tv_sec);
          diffUms = (long int) (endingUserTime.tv_usec - startingUserTime.tv_usec);
          diffSs = (long int) (endingSystemTime.tv_sec - startingSystemTime.tv_sec);
          diffSms = (long int) (endingSystemTime.tv_usec - startingSystemTime.tv_usec);
          printf("User Time: %ld.%.6lds | System Time: %ld.%.6lds\n" , diffUs, diffUms, diffSs, diffSms);
        }
	fdPlace++;
	flags = 0;
      }
      //--verbose
      else if(c == 'V'){
	if(profileFlag == 1){
          usageStart = getrusage(RUSAGE_SELF, &usage);
          startingUserTime = usage.ru_utime;
          startingSystemTime= usage.ru_stime;
        }
	verboseFlag = 1;
        if(profileFlag == 1){
          usageEnd = getrusage(RUSAGE_SELF, &usage);
          endingUserTime = usage.ru_utime;
          endingSystemTime = usage.ru_stime;
          diffUs = (long int) (endingUserTime.tv_sec - startingUserTime.tv_sec);
          diffUms = (long int) (endingUserTime.tv_usec - startingUserTime.tv_usec);
          diffSs = (long int) (endingSystemTime.tv_sec - startingSystemTime.tv_sec);
          diffSms = (long int) (endingSystemTime.tv_usec - startingSystemTime.tv_usec);
          printf("User Time: %ld.%.6lds | System Time: %ld.%.6lds\n" , diffUs, diffUms, diffSs, diffSms);
        }
      }
      //--rdwr
      else if(c == 'x'){
        if(verboseFlag ==1){
          printf("%s %s\n" , argv[optind-2], argv[optind-1]);
        }
	if(profileFlag == 1){
	  usageStart = getrusage(RUSAGE_SELF, &usage);
	  startingUserTime = usage.ru_utime;
	  startingSystemTime= usage.ru_stime;
	}

        flags |= O_RDWR;
        fdArr[fdPlace] = open(argv[optind-1], flags, 0644);
        if(fdArr[fdPlace] == -1){
          fprintf(stderr, "Error in opening specified file for rdwr");
          errCode = 1;
        }

	if(profileFlag == 1){
          usageEnd = getrusage(RUSAGE_SELF, &usage);
	  endingUserTime = usage.ru_utime;
          endingSystemTime = usage.ru_stime;
	  diffUs = (long int) (endingUserTime.tv_sec - startingUserTime.tv_sec);
	  diffUms = (long int) (endingUserTime.tv_usec - startingUserTime.tv_usec);
	  diffSs = (long int) (endingSystemTime.tv_sec - startingSystemTime.tv_sec);
	  diffSms = (long int) (endingSystemTime.tv_usec - startingSystemTime.tv_usec);
          printf("User Time: %ld.%.6lds | System Time: %ld.%.6lds\n" , diffUs, diffUms, diffSs, diffSms);
        }
        fdPlace++;
        flags = 0;
      }
      //--pipe
      else if(c == 'p'){
	if(verboseFlag ==1){
          printf("%s\n" , argv[optind-1]);
        }
        if(profileFlag == 1){
          usageStart = getrusage(RUSAGE_SELF, &usage);
          startingUserTime = usage.ru_utime;
          startingSystemTime= usage.ru_stime;
        }

	int endpts[2];
	if(pipe(endpts) == -1){
	  errCode = 1;
	}
	
	if(profileFlag == 1){
          usageEnd = getrusage(RUSAGE_SELF, &usage);
	  endingUserTime = usage.ru_utime;
          endingSystemTime = usage.ru_stime;
          diffUs = (long int) (endingUserTime.tv_sec - startingUserTime.tv_sec);
          diffUms = (long int) (endingUserTime.tv_usec - startingUserTime.tv_usec);
          diffSs = (long int) (endingSystemTime.tv_sec - startingSystemTime.tv_sec);
          diffSms = (long int) (endingSystemTime.tv_usec - startingSystemTime.tv_usec);
          printf("User Time: %ld.%.6lds | System Time: %ld.%.6lds\n" , diffUs, diffUms, diffSs, diffSms);
        }
	fdArr[fdPlace++] = endpts[0];
	fdArr[fdPlace++] = endpts[1];
      }
      //--command
      else if(c == 'C'){
	int start = optind;
	int curr = start;
	int last;
	char** options = malloc(sizeof(char*) * 25);
	if(profileFlag == 1){
          usageStart = getrusage(RUSAGE_SELF, &usage);
          startingUserTime = usage.ru_utime;
          startingSystemTime= usage.ru_stime;
        }

	for (;curr < argc; curr++){
	  if(argv[curr][0] == '-'){
	    if(argv[curr][1] == '-'){
	      last = curr - 1;
	      break;
	    }
	  }
	}
        
	if(curr == argc){
	  last = argc - 1;
	}
	if(last <= start+1){
	  fprintf(stderr, "Problem with options\n");
	  errCode = 1;
	  continue;
	}

	int inArg = atoi(argv[start - 1]);
	int outArg = atoi(argv[start]);
	int errArg = atoi(argv[start + 1]);
	char* cmd = argv[start + 2];

	if(!options){
	  fprintf(stderr, "Problem with allocating memory for  options\n");
	  errCode = 1;
	  continue;
	}

	//going through command options and setting them
	options[0]= cmd;
	i = 1;
	for (;i <= last - start - 2; i++){
	  options[i] = argv[start + 2 + i];
	}
	options[i] = NULL;

	if(inArg >= fdPlace || outArg >= fdPlace || errArg >= fdPlace){
	  fprintf(stderr, "Invalid file descriptors.\n");
	  errCode = 1;
	  break;
	}
          
	if(fdArr[inArg] < 0 || fdArr[outArg] < 0 || fdArr[errArg] < 0){
	  fprintf(stderr, "Invalid file descriptors.\n");
	  errCode = 1;
	  break;
	}
          
	if(verboseFlag){
	  printf("%s %d %d %d", argv[start - 2], inArg, outArg, errArg);
	  int num_opt = 0;
	  for (; num_opt < i; num_opt++)
	    printf(" %s", options[num_opt]);
	  printf("\n");
	}
	
	//putting process ID into array, and also putting it and string of commands into associated
	//proStrings entry in proStrings array
	pArr[ppos] = fork();
	bork[bPlace].process = pArr[ppos];
	bork[bPlace].pString = options;
	bork[bPlace].size = i;
	bPlace++;
	if (pArr[ppos] >= 0){

	  if (pArr[ppos] == 0){
	    dup2(fdArr[inArg], 0);
	    dup2(fdArr[outArg], 1);
	    dup2(fdArr[errArg], 2);

	    int closer = 0;
	    for (; closer < fdPlace; closer++){
	      close(fdArr[closer]);
	      fdArr[closer] = -1;
	    }

	    if (execvp(options[0], options) < 0){
	      fprintf(stderr, "Error executing command.\n");
	      errCode = 1;
	    }
	  }
	}
	else{
	  fprintf(stderr, "Error creating child process.\n");
	  ;
	  errCode = 1;
	}
	ppos++;
	if(profileFlag == 1){
          usageEnd = getrusage(RUSAGE_SELF, &usage);
	  endingUserTime = usage.ru_utime;
          endingSystemTime = usage.ru_stime;
          diffUs = (long int) (endingUserTime.tv_sec - startingUserTime.tv_sec);
          diffUms = (long int) (endingUserTime.tv_usec - startingUserTime.tv_usec);
          diffSs = (long int) (endingSystemTime.tv_sec - startingSystemTime.tv_sec);
          diffSms = (long int) (endingSystemTime.tv_usec - startingSystemTime.tv_usec);
          printf("User Time: %ld.%.6lds | System Time: %ld.%.6lds\n" , diffUs, diffUms, diffSs, diffSms);
        }
	fflush(stdout);
      }
      else{
	fprintf(stderr , "Incorrect argument passed: %s\n" , argv[optind]);
	errCode = 1;
      }
      fflush(stdout);
      //The following increments the option pointer, since we don't count command flags with
      //one - as options for simsh or anything else
      int inc;
      for(inc = optind; inc < argc; inc++){
	if(argv[optind][0] == '-'){
	  if(argv[optind][1] == '-')
	    break;
	}
	optind++;
      }
      }
    //free dynamically allocated memory
    free(fdArr);
    free(pArr);
    //    free(proStrings);
    //exit with the correc exit code or raise the correct signal
    if(maxSigCode > 0){
      signal(maxSigCode, SIG_DFL);
      raise(maxSigCode);
    }
    else if(maxExCode > 0){
      exit(maxExCode);
    }
    exit(errCode);
}
