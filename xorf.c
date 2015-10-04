#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <signal.h>
#include <string.h>

#define MAX_MEMORY 0x10000000 //allocate no more than 256 MB total for the main buffers

//0x10000000

int64_t i;

void progress(){
  signal(SIGUSR1, progress);
  fprintf(stderr, "%d\n", i);
}

int64_t mini(int64_t a, int64_t b){
  return a < b ? a : b;
}

int64_t ceiling(int64_t a, int64_t b){
  return (b*(a/b) == a ? a/b : a/b+1);
}

int main(int argc, char** argv){
  if(argc == 2 && argv[1][0] == '?' && argv[1][1] == 0){
    printf("Usage: 'xorf target file1 file2 ...'\nThis will xor the contents of all the given files with the contents of target and send the result to stdout.  \nAny files smaller than target will be effectively repeated until it is the same size as target.\n");
    exit(-1);
  }
  if(argc < 3){
    printf("Usage: 'xorf file1 file2 ...\n"); 
    exit(-1);
  }

  FILE *stdout = fopen("/dev/stdout", "a");
  FILE *stderr = fopen("/dev/stderr", "a");

  signal(SIGUSR1, progress);

  int numfiles = argc-1;

  FILE **f = (FILE **)malloc(sizeof(FILE)*numfiles);
  char **buf = (char **)malloc(sizeof(char *)*numfiles);
  int64_t *bufsz = (int64_t *)malloc(sizeof(int64_t)*numfiles); //How big a chunk to read into memory at once
  char *wholefile = (char *)malloc(numfiles);
  int64_t *sz = (int64_t *)malloc(sizeof(int64_t)*numfiles);
  int64_t bufsize = MAX_MEMORY/(numfiles+1);
  int64_t j;

  for(i=0; i<numfiles; i++){
    f[i] = fopen(argv[i+1], "r");
    if(f[i] == NULL){ perror("findin: Error opening first file"); exit(-1); }
    fseek(f[i], 0L, SEEK_END);
    sz[i] = ftell(f[i]);
    fseek(f[i], 0L, SEEK_SET);
    bufsz[i] = mini(sz[i], bufsize);
    wholefile[i] = (sz[i] > bufsize ? 0 : 1);
    buf[i] = (char *)malloc(bufsz[i]);
    fprintf(stderr, "File %d: %s of size %d and buffer of size %d (%s)\n", i+1, argv[i+1], sz[i], bufsz[i], wholefile[i] ? "whole" : "part");
  }
  
  int64_t len = sz[0];
  int64_t obl = mini(len, bufsize); //output buffer length
  char *output = (char *)malloc(obl);
  
  for(i=0; i < len; i++){
    output[i%obl] = 0;
    for(j=0; j < numfiles; j++){
      if(i%bufsz[j] == 0 && (i == 0 || !(wholefile[j]))){ 
	// we're at the end of the buffer--read more file (unless
	// we've already got the whole file in memory)
	fprintf(stderr, "End of buffer: i=%d, j=%d\n", i, j);
	int64_t ans = fread(buf[j], 1, bufsz[j], f[j]);
	if(ans < bufsz[j]){
	  fprintf(stderr, "Failed to read more file: only read %d bytes\n", ans);
	  fseek(f[j], 0L, SEEK_SET);
	  int64_t ans2 = fread(buf[j]+ans, 1, bufsz[j]-ans, f[j]);
	  fprintf(stderr, "%d + %d = %d = %d\n", ans, ans2, ans+ans2, bufsz[j]);
	}
      }
      
      output[i%obl] ^= buf[j][i%bufsz[j]];
    }
    //fprintf(stderr,"%02x ", output[i%obl]);
    if((i+1) % obl == 0){ fwrite(output, obl, 1, stdout); }
  }
  if(len % obl != 0){ fwrite(output, len % obl, 1, stdout); }
}
