#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>

#define __STDC_FORMAT_MACROS

#include <assert.h>
#include <inttypes.h>
#include <stdint.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/time.h>
#include <sys/wait.h>
#include <time.h>
#include <stdbool.h>
#include <unistd.h>

#define PAGEMAP_LENGTH 8

int main( int argc, char *argv[] )
{

  /*if( argc == 2 ) {
    printf("The number of DRAM refresh periods that will be run is %s\n", argv[1]);
  }
  else if( argc > 2 ) {
    printf("Too many arguments supplied.\n");
    exit(1);
  }
  else {
    printf("Please input how many DRAM refresh periods you want to run. (ie a Number)\n");
    exit(1);
  }

  refreshPeriods = atoi(argv[1]);*/
  
  void *addr = (void*)0x7fda1eb32a68;
  
  printf("0x%lx\n", addr);
  
  FILE *pagemap = fopen("/proc/9924/pagemap", "rb");
  unsigned long offset = (unsigned long)addr / getpagesize() * PAGEMAP_LENGTH;
  if (fseek(pagemap, (unsigned long)offset, SEEK_SET) != 0) {
    fprintf(stderr,"Failed to seek pagemap to proper location\n");
    exit(1);
  }
  
  unsigned long page_frame_number = 0;
  fread(&page_frame_number, 1, PAGEMAP_LENGTH-1, pagemap);
  
  page_frame_number &= 0x7fffffffffff;//ff;
  
  fclose(pagemap);
  
  printf("%d\n", page_frame_number);
  
  void *phys_addr = (void*)((page_frame_number<<12/*PAGE_SHIFT*/) + ((unsigned long)addr%getpagesize()));
  
  printf("0x%lx\n", phys_addr);

	return(0);
}
