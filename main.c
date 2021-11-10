#include <stdio.h>

// declaring memory and registers as globals
int mem[65536];
int a, x, n, z, v, c, pc, sp;

int main(){
        // von neumann execution cycle as described in the book
        // load the machine language program
        int input;
        int i = 0;
        while(scanf("%x",&input)){
                mem[i] = input;
                i++;
        }

        // printing out the first 20 numbers stored in memory as decimals
        for(i=0;i<20;i++){
                printf("%d ", mem[i]);
        }

        // initialize program counter and stack pointer
        pc = 0;
        sp = 0;
        
        // executing instructions until stop is found
        /* PSEUDOCODE PSEUDOCODE PSEUDOCODE
        do {
                // fetch next instruction
                instruction = mem[pc]

                // decode the instruction specifer
                decode(instruction);

                // increment the program counter
                pc++;

                // execute the instruction fetched
                execute(instruction);
        } while(instruction != STOP())
        ABOVE IS NOT ENTIRELY IMPLEMENTED YET*/
        
        return 0;
}