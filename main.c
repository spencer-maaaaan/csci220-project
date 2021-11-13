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

// test functions
void mem_dump(){
        // dumping first 100 addresses
        for(int i=0;i<100;i++){
                printf("%02x ", mem[i]);
                if((i+1)%10 == 0){
                        printf("\n");
                }
        }
}

void main(){
        // von neumann execution cycle as described in the book
        // load the machine language program
        int input;
        int i = 0;
        while(scanf("%x",&input)){
                mem[i] = input;
                i++;
        }

        mem_dump(); // to see if the program was successfully loaded (remove in final submission)

        // initialize program counter and stack pointer
        pc = 0;
        sp = 0;
        
        // executing instructions until stop is encountered
        int instruction = 0x00;
        do {
                // fetch next instruction
                instruction = mem[pc];
                
                // decode the instruction specifier
                int register_bit;
                int address_mode;

                // instructions <= 0x05 don't have address modes or register bits
                // instructions >0x05 and <= 0x11 only have a register bit at spec<7>, so we should extract that and set it
                if(instruction > 0x05 && instruction <= 0x11){
                        register_bit = instruction & 0x01;
                        printf("bbbb bbbr: register bit set to %d\n", register_bit);
                }
                // instructions > 0x11 and <= 0x27 only specify address mode as either direct or indexed with spec<7>
                else if(instruction > 0x11 && instruction <= 0x27){
                        address_mode = (instruction & 0x01) ? 5 : 0;
                        printf("bbbb bbba: address mode set to %d\n", address_mode);
                }
                // instructions >0x27 and <= 0x5f have a 3 bit address mode at spec<5...7>
                else if(instruction > 0x27 && instruction <= 0x5f){
                        address_mode = instruction & 0x07;
                        printf("bbbb baaa: address mode set to %d\n", address_mode);
                }
                // instructions > 0x5f and <= 0xff have a 3 bit address mode at spec<5...7> and register bit at spec<4>
                else if(instruction > 0x5f && instruction <= 0xff){
                        address_mode = instruction & 0x07;
                        register_bit = (instruction & 0x08) >> 3;
                        printf("bbbb raaa: address mode set to %d and register bit set to %d\n", address_mode, register_bit);
                }

                // increment the program counter
                // instructions in first condition are unary, otherwise inc by 3
                if(instruction >= 0x00 && instruction <= 0x11 || instruction >= 0x28 && instruction <= 0x2f){
                        pc++;
                }
                else {
                        pc += 3;
                }

                // execute the instruction fetched
                // this is where the big ugly 40 case switch statement will have to go
                // rather do this than a 255 case statement though

        } while(instruction != 0x00);
}