//NAME: Devyan Biswas
//EMAIL: devyanbiswas@outlook.com
//ID: 
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <getopt.h>

void segFaultHandler(){
    fprintf(stderr, "Segmentation Fault caught, stopping...\n");
    exit(4);
}

void forceSegFault(){
  char *t = NULL;
  *t = 'a';
}

int main(int argc, char *argv[]){
  //Using resources found online, will be referencing through comments
  //https://www.gnu.org/software/libc/manual/html_node/Getopt-Long-Option-Example.html
  struct option opts[] = {
    {"input", required_argument, NULL, 'I'},
    {"output", required_argument, NULL, 'O'},
    {"segfault", no_argument, NULL, 'S'},
    {"catch", no_argument, NULL, 'C'},
    {"dump-core", no_argument, NULL, 'D'}, 
    {0,0,0,0}
  };
  
  // int segFlag = 0;
  int c;
  //  int dF = 0;
  int fd;
  int od;
  //  int option_index = 0;
  //https://www.gnu.org/software/libc/manual/html_node/Getopt-Long-Option-Example.html
  //http://www.tutorialspoint.com/ansi_c/c_strerror.htm
  while((c=getopt_long(argc, argv, "", opts, NULL)) != -1){
    switch(c){
    //https://www.ibm.com/developerworks/community/blogs/58e72888-6340-46ac-b488-d31aa4058e9c/entry/understanding_linux_open_system_call?lang=en
    //https://linux.die.net/man/3/optarg
    case 'I':
      fd = open(optarg, O_RDONLY);
      if(fd == -1){
	fprintf(stderr, "Cannot open --input file: %s\n " , optarg); 
	fprintf(stderr, "%s\n", strerror(errno));
	exit(2);
	break;
      }
      else if(fd >= 0){
	dup2(fd, 0); //This is setting the fd as the new std input
      }
      break;
    case 'O':
      od = creat(optarg,0666);
      if(od == -1){
	fprintf(stderr, "Error accessing/creating: %s\n " , optarg);
	fprintf(stderr, "%s\n", strerror(errno));
	exit(3);
	break;
      }
      else if(od >= 0){
	dup2(od, 1); //This is setting the od as the nes std output
      }
      break;
    case  'S':
      forceSegFault();
      break;
    //https://www.geeksforgeeks.org/signals-c-language/
    case 'C':
      //      if(dF != 1){
	signal(SIGSEGV, segFaultHandler);
	  //  }
      break;
    case 'D':      
      //      dF = 1;
      signal(SIGSEGV,SIG_DFL);
      break;
    default:
      fprintf(stderr, "Can only have --input, --output, --segfault, --catch, and --dump-core as options\n");
      exit(1);
    }
  }
  /*
  if(segFlag){
    forceSegFault();
  }
  */
  char hold;
  while(read(0, &hold, sizeof(char)) > 0){
    if(write(1, &hold, sizeof(char)) < 0){
      fprintf(stderr, "Error writing file: %s\n" , strerror(errno));
      exit(3);
    }
  }
 close(0);
 close(1);
 exit(0);
}
