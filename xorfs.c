#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <signal.h>
#include <string.h>


#define BUFSZ 1048756
#define MAX_MEMORY 0x10000000 //allocate no more than 256 MB total for the main buffers

//0x10000000

int64_t i;

void progress(){
  signal(SIGUSR1, progress);
  fprintf(stderr, "%d\n", i);
}

int main(int argc, char** argv){
  if(argc == 2 && argv[1][0] == '?' && argv[1][1] == 0){
    printf("Usage: 'xorf inputfile keyfile ...'\nThis will xor the contents of all the given inputile with the contents of keyfile and send the result to stdout.  \nIf keyfile is smaller than inputfile it will be effectively repeated until it is the same size as inputfile.  keyfile must be at least %d bytes, and only the first %d bytes will be used.\n",BUFSZ,BUFSZ);
    exit(-1);
  }
  if(argc < 3){
    printf("Usage: 'xorf file keyfile\n"); 
    exit(-1);
  }

  FILE *inputfile = fopen(argv[2], "r");
  FILE *keyfile = fopen(argv[1], "r");
  FILE *stdout = fopen("/dev/stdout", "a");
  FILE *stderr = fopen("/dev/stderr", "a");

  signal(SIGUSR1, progress);

  char *inputbuf = (char *)malloc(sizeof(char *)*BUFSZ);
  char *keybuf = (char *)malloc(sizeof(char *)*BUFSZ);
  
  if(inputfile == NULL || keyfile == NULL){ perror("findin: Error opening first file"); exit(-1); }
  fseek(inputfile, 0L, SEEK_END);
  int64_t inputsize = ftell(inputfile);
  fseek(inputfile, 0L, SEEK_SET);
  // Read the key file
  fread(keybuf,BUFSZ,1,keyfile);
  int64_t i,j,read_len;
  
  for(i=0; i < inputsize; i += read_len){
    read_len = fread(inputbuf,BUFSZ,1,inputfile);
    for(j=0; j < read_len; j++){ inputbuf[j] ^= keybuf[j]; }
    fwrite(inputbuf, read_len, 1, stdout);
  }
}
