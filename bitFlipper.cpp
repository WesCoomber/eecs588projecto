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

int maxIterations = 1;

//returns random address in a dif row
char *pick_addr() {
  size_t offset = (rand() << 12) % mem_size;
  return g_mem + offset;
}


//returns random address in a dif row
char testAll(int ActiveInterval, int refreshInterval) {
  int ai = ActiveInterval;
  int ri = refreshInterval;
  int count = (2*ri)/ai;

  uint32_t *addrs;

   //we have a certain window to hammer
   // and we need to maximize the amount of hammering we do within that window
//shoot for 100 to 128 ms

  //for max iterations number of loops pick a random adress within our allocated block, read the row and then flush that row from memory ie rowhammering
   for (int i = 0; i < maxIterations; i++){

      addrs = (uint32_t *) pick_addr();

      uint32_t sum = 0;
      sum += *addrs + 1;
      asm volatile("clflush (%0)" : : "r" (addrs) : "memory");

   }


   //how to find rowMax?
   /* 
   rowMax = -1;
   for (int i = 0; i < rowMax; i++){
      for (int i = 0; i < count; i++){
         int dst, src;
         asm volatile( "goforever:\n" 
         "mov (X), %%eax \n"
         "mov (Y), %%ebx \n"
         "clflush (X)\n"
         "clflush (Y)\n"
         "mfence\n"
         "jmp goforever "
         : "=r"(dst) 
         : "%eax");         
         }
   }
   */

   //readAll and find errors
   //compare the bitflipped target to a control block of memory (all 1's)
   int differ = memcmp(g_mem, control_mem, sizeof(g_mem));
   printf("If not 0 then bitsFlipped!: %s\n", differ);

}

int main()
{
	pid_t processID, processParentID;

	//fork();

   g_mem = (char *) mmap(NULL, mem_size, PROT_READ | PROT_WRITE, MAP_ANON | MAP_PRIVATE, -1, 0);

   control_mem = (char *) mmap(NULL, mem_size, PROT_READ | PROT_WRITE, MAP_ANON | MAP_PRIVATE, -1, 0);

   //write DP to entire module (mapped memory) all 1's
   //set the memory pointed to by the g_mem value to a value of 255 decimal [0xff hex], and sets 1<<30 [memsize] bytes to the 0xff value
   memset(g_mem, dataPattern, mem_size);
   memset(control_mem, dataPattern, mem_size);

   //trying to evaluate duration of loop iteration for timing purposes
    clock_t t;
    t = clock();
    testAll(48, 64);
    t = clock() - t;
    double time_taken = ((double)t)/CLOCKS_PER_SEC; // in seconds
 
    printf("testAll() took %f seconds to execute \n", time_taken);


  // printf("time Taken: %g\n", time_taken);


	printf("Aloha World\n");

	return(0);
}
