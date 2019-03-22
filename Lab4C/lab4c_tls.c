//NAME: Devyan Biswas
//EMAIL: devyanbiswas@outlook.com
//ID: #UID
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <getopt.h>
#include <poll.h>
#include <signal.h>
#include <string.h>
#include <sys/types.h>
#include <sys/errno.h>
#include <time.h>
#include <sys/time.h>
#include <math.h>
#include <ctype.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <mraa.h>
#include <openssl/bio.h>
#include <openssl/ssl.h>
#include <openssl/err.h>

#define TPIN 1
#define BPIN 60

mraa_aio_context temperature_val;
mraa_gpio_context button_boi;
char scale = 'F';
int per = 1; //default period value
FILE* output_fd = 0; //default file descriptor for output is stdout
struct timeval timer;
int logging_flag = 1;
time_t n_time = 0;
struct tm* curr_time;

//Constants added in part C
int port_num = -1;
char* host_name = "";
char* id_num = "";
int socket_num;
struct hostent* server_num;
struct sockaddr_in server_addr; //from https://pubs.opengroup.org/onlinepubs/7990989775/xns/netinetin.h.html:
//The sockaddr_in structure is used to store addresses for the Internet protocol family. Values of this type must b\e cast to struct sockaddr for use with the socket interfaces defined in this document.

SSL_CTX* ssl_cntxt = NULL;
SSL* ssl_instance = NULL;


//print for stdout and ssl write
void print(char* to_print, int print_server){
  if(print_server == 1){
    //dprintf(socket_num, "%s\n" ,to_print);
    char output_buff[200];
    sprintf(output_buff,  "%s\n", to_print);
    SSL_write(ssl_instance, output_buff, strlen(output_buff));
  }
  fprintf(stderr, "%s\n", to_print);
  fprintf(output_fd, "%s\n", to_print);
  fflush(output_fd);
}

float get_temperature(){ //get this algo: <http://wiki.seeedstudio.com/Grove-Temperature_Sensor_V1.2/>
  int raw_temp = mraa_aio_read(temperature_val);
  int thermis = 4275;
  float r0 = 100000;
  float r = 1023.0/raw_temp-1.0;
  r *= r0;
  float temp = 1.0/(log(r/r0)/thermis+1/298.15)-273.15;
  if(scale == 'F'){
    return (temp *(9/5)) + 32;
  }
  else{
    return temp;
  }
}

void time_stamp() { //not gonna lie, fiound this formatting bs online...
  gettimeofday(&timer, 0);
  if (logging_flag && timer.tv_sec >= n_time) {
    float temp_val = get_temperature();
    int t = temp_val * 10;
    curr_time = localtime(&timer.tv_sec);
    char out[200];
    sprintf(out, "%02d:%02d:%02d %d.%1d", curr_time->tm_hour,
	    curr_time->tm_min, curr_time->tm_sec, t/10, t%10);
    print(out,1);
    n_time = timer.tv_sec + per;
  }
}

void shutdown_func(){ //had to create a function, since I totally messed up and forgot to actually poll. the dang. \button. with isr.
  curr_time = localtime(&timer.tv_sec);
  char out[200];
  sprintf(out,"%02d:%02d:%02d SHUTDOWN", curr_time->tm_hour, curr_time->tm_min, curr_time->tm_sec);
  print(out, 1);
  SSL_shutdown(ssl_instance);
  SSL_free(ssl_instance);
  exit(0);
}

void process_line(char* input_line){
  // int len = strlen(input_line);
  //input_line[len-1] = '\n';
  while(*input_line == '\t' || *input_line == ' '){ //increment the input_line pointer till we get to a place with processable characters (by our perspective)
    input_line++;
  }
  char *in_per = strstr(input_line, "PERIOD="); //this is to get the period from the input_line string

  if(in_per == input_line){ //if the only thing in here is, in fact, the period
    char *t = input_line;
    t += 7; //size of "PERIOD=" lololol
    if(*t != 0) {
      int u = atoi(t);
      while(*t != 0) {
	if (!isdigit(*t)) {
	  return;
	}
	t++;
      }
      per = u; //if we are passed a valid period, tested by above, then set it equal to the per variable wich globlly tracks the period
    }
  }
  print(input_line, 0); //prints whatever line
  if(strcmp(input_line, "SCALE=F") == 0) {
    scale = 'F';
  }
  else if(strcmp(input_line, "SCALE=C") == 0) {
    scale = 'C';
  }
  else if(strcmp(input_line, "STOP") == 0) {
    logging_flag = 0;
  }
  else if(strcmp(input_line, "START") == 0) {
    logging_flag = 1;
  }
  else if(strcmp(input_line, "OFF") == 0) {
    shutdown_func();
  }
}

void server_send(char* in_line){
  int ret = SSL_read(ssl_instance, in_line, 256);
  if(ret > 0){
    in_line[ret] = 0;
  }
  char* t = in_line;
  while (t < &in_line[ret]) {
    char* s = t;
    while (s < &in_line[ret] && *s != '\n') {
      s++;
    }
    *s = 0;
    process_line(t);
    t = &s[1];
  }
}

int main(int argc, char** argv){
  //fprintf(stdout, "hello mraa\n Version: %s\n", mraa_get_version());

  //Time to get options and set up stuff!
  struct option opts[] = {
    {"scale", required_argument, NULL, 's'},
    {"period", required_argument, NULL , 'p'},
    {"log", required_argument, NULL, 'l'},
    {"id", required_argument, NULL, 'i'},
    {"host", required_argument, NULL, 'h'},
    {0,0,0,0}
  };
  int c;
  while((c = getopt_long(argc, argv, "", opts, NULL)) != -1){
    switch(c){
    case 'h':
      host_name = optarg;
      break;
    case 'i':
      id_num = optarg;
      break;
    case 'p':
      per = atoi(optarg);
      break;
    case 'l':
      output_fd = fopen(optarg, "w+");
      if(output_fd == NULL){
	fprintf(stderr, "Problem opening the logfile\n");
	exit(1);
      }
      break;
    case 's':
      if(optarg[0] == 'F' || optarg[0] == 'C'){
	scale = optarg[0];
	break;
      }
    default:
      fprintf(stderr , "BAD ARGUMENT %s\n" , optarg);
      exit(1);
      break;
    }
  }

  //Getting the port after hase havce processed the commands
  if(optind < argc){
    port_num = atoi(argv[optind]);
    if(port_num <= 0){
      fprintf(stderr, "Problem with opening port %x\n" , port_num);
      exit(1);
    }
  }

  //check that a hostname has been given
  if(strlen(host_name) == 0){
    fprintf(stderr, "Invalid host name: %s\n" , host_name);
    exit(1);
  }

  if(strlen(id_num) != 9){
    fprintf(stderr, "ID field has incorrect size: %x\n", strlen(id_num));
    exit(1);
  }

  //Note: according to new spec, a logfile is now mandatory. So will check for that here
  if(output_fd == 0){
    fprintf(stderr , "Error: logfile option is mandatory\n");
    exit(1);
  }

  if((socket_num = socket(AF_INET, SOCK_STREAM, 0)) < 0){
    fprintf(stderr, "Unable to open the socket\n");
    exit(1);
  }

  if((server_num = gethostbyname(host_name)) == NULL){
    fprintf(stderr, "Error with getting the host name: %s\n" , host_name);
    exit(1);
  }

  memset((void*) &server_addr, 0, sizeof(server_addr));
  server_addr.sin_family = AF_INET; //type of internet addr
  memcpy((char *)&server_addr.sin_addr.s_addr, (char *)server_num->h_addr, server_num->h_length);//getting the server IP
  //part of marshalling: transform endianness to readable by server format
  server_addr.sin_port = htons(port_num);
  //connect to the server
  if((connect(socket_num, (struct sockaddr*) &server_addr, sizeof(server_addr))) < 0){
    fprintf(stderr, "Unable to make connection with the remote server\n");
    exit(1);
  }

  SSL_library_init();
  SSL_load_error_strings();
  OpenSSL_add_all_algorithms();
  ssl_cntxt = SSL_CTX_new(TLSv1_client_method());

  if(ssl_cntxt == NULL){
    fprintf(stderr , "Problem with extablishind ssl context\n");
    exit(2);
  }

  ssl_instance = SSL_new(ssl_cntxt);
  if(ssl_instance == NULL){
    fprintf(stderr, "Problem setting ssl\n");
    exit(2);
  }
  
  if(!SSL_set_fd(ssl_instance, socket_num)){
    fprintf(stderr, "Problem setting file descriptor (socket)\n");
    exit(2);
  }

  if(SSL_connect(ssl_instance) != 1){
    fprintf(stderr, "Problem with TLS hanshake\n");
    exit(2);
  }

 char send[50];
 sprintf(send, "ID=%s", id_num);
 print(send, 1);
 
 temperature_val = mraa_aio_init(TPIN);
 if(temperature_val == NULL){
   fprintf(stderr, "Failed to initialize Temperature Pin %d\n" , TPIN);
   exit(1);
 }
 
 struct pollfd poller;
 poller.fd = socket_num;
 poller.events = POLLIN;
 
 char input_buffer[256];
 
 while(1){
   time_stamp();
   poll(&poller, 1, 0);
   if(poller.revents & POLLIN){
     server_send(input_buffer);
   }
 }
 
 mraa_aio_close(temperature_val);
 exit(0);
}
 

