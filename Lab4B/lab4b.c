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
#include <mraa.h>

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

//this is gonna allow us to control where we print to
//useful since we need to be able to do this STOP, START, OFF, and the two SCALEs when we wnat to be able to start adding commands in
void print(char *to_print, int print_stdout) {
  if (print_stdout == 1) {
    fprintf(stdout, "%s\n", to_print);
  }
  if (output_fd != 0) {
    fprintf(output_fd, "%s\n", to_print);
    fflush(output_fd);
  }
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

void shutdown(){
  curr_time = localtime(&timer.tv_sec);
  char out[200];
  sprintf(out, "%02d:%02d:%02d SHUTDOWN", curr_time->tm_hour, curr_time->tm_min, curr_time->tm_sec);
  print(out, 1);
  exit(0);
}

void process_line(char* input_line){
  int len = strlen(input_line);
  input_line[len - 1] = '\0';
  while(*input_line == '\t' || *input_line == ' '){
    input_line++;
  }
  char *in_per = strstr(input_line, "PERIOD=");

  if(in_per == input_line){
    char *t = input_line;
    t += 7;
    if(*t != 0) {
      int u = atoi(t);
      while(*t != 0) {
	if (!isdigit(*t)) {
	  return;
	}
	t++;
      }
      per = u;
    }
  }
  print(input_line, 0);
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
    shutdown();
  }
}

int main(int argc, char** argv){
  //fprintf(stdout, "hello mraa\n Version: %s\n", mraa_get_version());

  //Time to get options and set up stuff!
  struct option opts[] = {
    {"scale", required_argument, NULL, 's'},
    {"period", required_argument, NULL , 'p'},
    {"log", required_argument, NULL, 'l'},
    {0,0,0,0}
  };
  int c;
  while((c = getopt_long(argc, argv, "", opts, NULL)) != -1){
    switch(c){
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

  //Initialize the pins for button and temp sensor
  temperature_val = mraa_aio_init(TPIN);
  if(temperature_val == NULL){
    fprintf(stderr, "Failed to initialize Temperature Pin %d\n" , TPIN);
    exit(1);
  }
  button_boi = mraa_gpio_init(BPIN);
  if(button_boi == NULL){
    fprintf(stderr, "Failed to initialize Button Pin %d\n" , BPIN);
    exit(1);
  }
  mraa_gpio_dir(button_boi, MRAA_GPIO_IN); //need this to specify that the button is for input
  mraa_gpio_isr(button_boi, MRAA_GPIO_EDGE_RISING, &shutdown, NULL);
  //struct for poll
  struct pollfd poller;
  poller.fd = STDIN_FILENO;
  poller.events = POLLRDNORM; //same as POLLIN

  char* input_buffer;
  input_buffer = (char *)malloc(1024 * sizeof(char));
  if(input_buffer == NULL){
    fprintf(stderr, "Problem with allocating memory to input buffer\n");
    exit(1);
  }

  while(1){
    time_stamp();
    int result = poll(&poller, 1, 0);
    if(result){
      fgets(input_buffer, 1024, stdin);
      process_line(input_buffer);
    }
  }
  mraa_aio_close(temperature_val);
  mraa_gpio_close(button_boi);
  exit(0);
}
