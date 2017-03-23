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
#include <vector>
#include <chrono>

/*
@Author: Wesley Coomber

MIT License

Copyright (c) 2017 

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

const size_t mem_size = 1 << 30;
const size_t dataPattern = 0xff;

char *g_mem;
char *control_mem;

int maxIterations = 35000;

int refreshPeriods = -1;

uint32_t *addrs;
uint32_t *addrs2;

//returns random address in a dif row
char *pick_addr() {
  size_t offset = (rand() << 12) % mem_size;
  return g_mem + offset;
}


//returns random address in a dif row
double testAll(int ActiveInterval, int refreshInterval, int loopIters) {
   int ai = ActiveInterval;
   int ri = refreshInterval;
   int count = (2*ri)/ai;

   uint32_t sum = 0;
   int32_t sum2 = 0;
    clock_t t;
    t = clock();

   //we have a certain window to hammer
   // and we need to maximize the amount of hammering we do within that window
   //shoot for 100 to 128 ms

   //for max iterations number of loops pick a pair random addresses within our allocated block, read the row and then flush that row from memory ie rowhammering
   for (int i = 0; i < maxIterations*loopIters; i++){
      sum += *addrs + 1;
      sum2 += *addrs2 + 1;
      asm volatile("clflush (%0)" : : "r" (addrs) : "memory");
      asm volatile("clflush (%0)" : : "r" (addrs2) : "memory");
   }
   
    t = clock() - t;
    double time_taken = ((double)t)/CLOCKS_PER_SEC; // in seconds

   //readAll and find errors
   //compare the bitflipped target to a control block of memory (all 1's)
   int differ = -1;
   differ = memcmp(g_mem, control_mem, mem_size);
   //printf("%i\n", differ);
   assert(&differ != NULL);
   if(differ != 0){
      for (int i=0; i<mem_size/8; i++) {
        if (((uint64_t*)g_mem)[i] != ((uint64_t*)control_mem)[i]) {
          printf("Flip at location 0x%lx: %lx\n", &(((uint64_t*)g_mem)[i]), ((uint64_t*)g_mem)[i]);
        }
      }
      printf("bitFlip detected! Ending execution: %i\n", differ);
      exit(1);
   }

   //printf("If not 0 then bitsFlipped!: %i\n", differ);
   return time_taken;

}

int main( int argc, char *argv[] )
{
	pid_t processID, processParentID;

	//fork();

   if( argc == 2 ) {
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

   refreshPeriods = atoi(argv[1]);


   g_mem = (char *) mmap(NULL, mem_size, PROT_READ | PROT_WRITE, MAP_ANON | MAP_PRIVATE, -1, 0);

   control_mem = (char *) mmap(NULL, mem_size, PROT_READ | PROT_WRITE, MAP_ANON | MAP_PRIVATE, -1, 0);

   //write DP to entire module (mapped memory) all 1's
   //set the memory pointed to by the g_mem value to a value of 255 decimal [0xff hex], and sets 1<<30 [memsize] bytes to the 0xff value
   memset(g_mem, dataPattern, mem_size);
   memset(control_mem, dataPattern, mem_size);


   //select two addresses to hammer different each refresh period.

   //trying to evaluate duration of loop iteration for timing purposes
   while(1) {
    addrs = (uint32_t *) pick_addr();
    addrs2 = (uint32_t *) pick_addr();
    double time_taken = testAll(48, 64, refreshPeriods);
 
    printf("testAll() took %f seconds to execute \n", time_taken);
   }


  // printf("time Taken: %g\n", time_taken);


	printf("Aloha World\n");

	return(0);
}
