#include <stdio.h>

// declaring memory and registers as globals
int mem[65536];
int a, x, n, z, v, c, pc, sp;

// given an operand specifier and a memory addressing mode, returns the value of the operand
int retrieve_operand(int specifier, int mode){
        switch(mode){
                // immediate
                case 0:
                        return specifier;
                        break;
                // direct                        
                case 1:
                        return mem[specifier];
                        break;
                // indirect
                case 2:
                        return mem[mem[specifier]];
                        break;
                // stack-relative
                case 3:
                        return mem[sp+specifier];
                        break;
                // stack-relative deferred
                case 4:
                        return mem[mem[sp+specifier]];
                        break;
                // indexed
                case 5:
                        return mem[x+specifier];
                        break;
                // stack indexed
                case 6:
                        return mem[x+sp+specifier];
                        break;
                // stack index deferred
                case 7:
                        return mem[mem[sp+specifier]+x];
                        break;
        }
}

// test function
void run_tests(){
        // dumping top 100 memory locations
        for(int i=0;i<100;i++){
                printf("%x ", mem[i]);
        }

        // testing address mode
        int op = retrieve_operand(0x0000, 1);
        printf("\n%x", op);
}

int main(){
        // von neumann execution cycle as described in the book
        // load the machine language program
        int input;
        int i = 0;
        while(scanf("%x",&input)){
                mem[i] = input;
                i++;
        }

        run_tests();

        // initialize program counter and stack pointer
        pc = 0;
        sp = 0;
        
        // executing instructions until stop is found
        /* PSEUDOCODE PSEUDOCODE PSEUDOCODE
        do {
                // fetch next instruction
                instruction = mem[pc]

                // decode the instruction specifier
                decode(instruction);

                // increment the program counter
                pc++;

                // execute the instruction fetched
                execute(instruction);
        } while(instruction != STOP())
        ABOVE IS NOT ENTIRELY IMPLEMENTED YET*/
        
        return 0;
}