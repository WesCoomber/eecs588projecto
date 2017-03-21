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
#include <unistd.h>


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

char *g_mem;

//returns random address in a dif row
char *gen_addr() {
  size_t offset = (rand() << 12) % mem_size;
  return g_mem + offset;
}

int main()
{
	pid_t processID, processParentID;

	//fork();

	  g_mem = (char *) mmap(NULL, mem_size, PROT_READ | PROT_WRITE, MAP_ANON | MAP_PRIVATE, -1, 0);

     //set the memory pointed to by the g_mem value to a value of 255 decimal [0xff hex], and sets 1<<30 [memsize] bytes to the 0xff value
     memset(g_mem, 0xff, mem_size);

     //loop forever
     while(true){
      //pick two random addresses in diff rows and then try toclflush toggle them.
      for (int i = 0; i < 8; i++){
         addrs[0] = (uint32_t *) gen_addr();
         addrs[1] = (uint32_t *) gen_addr();
         for (int j = 0; j <addrs.size(); j++){
            asm volatile("clflush (%0)" : : "r" (addrs[j]) : "memory");
         }
      }
     }

	 // inline assembly, have NOT tested this.
   	asm volatile("mainForever:
   			mov (X), %eax
   			mov (Y), %ebx
   			clflush (X)
   			clflush (Y)
   			mfence
   			jmp mainForever");


	printf("Aloha World\n");

	return(0);
}
