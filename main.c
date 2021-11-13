#include <stdio.h>

// declaring memory and registers as globals
int mem[65536];
int a, x, n, z, v, c, pc, sp;

void mem_dump(){
        // dumping first 100 addresses
        for(int i=0;i<100;i++){
                printf("%02x ", mem[i]);
                if((i+1)%10 == 0){
                        printf("\n");
                }
        }
        printf("\n");
}

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

// instructions
/*
TODO:
 - ldb needs to set n and z bits accordingly
 - ldb and stb need to only interact with bytes, they basically do words
*/
void ldb(int reg, int operand_specifier, int address_mode){
        // setting working register
        int *working_register = (reg)? &x:&a;

        // retrieving operand
        int operand = retrieve_operand(operand_specifier, address_mode);

        // if operand specifier is 0xfc15, taking from stdin, else loading byte from memory
        if(operand_specifier == 0xfc15){
                *working_register = scanf("%x");
        }
        else {
                *working_register = operand;
        }
}

void stb(int reg, int operand_specifier, int address_mode){
        // setting working register
        int *working_register = (reg)? &x:&a;

        // retrieving operand
        int operand = retrieve_operand(operand_specifier, address_mode);
        
        // masking as to only interact with bits 8-15
        operand = operand & 0x0f;

        // if operand specifier is 0xfc16, printing ot stdout, else loading into working register
        if(operand_specifier == 0xfc16){
                printf("%c", *working_register);
        }
        else {
                mem[operand] = *working_register;
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
                int instruction_specifier = mem[pc];
                
                // decode the instruction specifier
                int register_bit;
                int operand_specifier;
                int address_mode;

                // instructions >0x05 and <= 0x11 only have a register bit at spec<7>, so we should extract that and set it
                if(instruction_specifier > 0x05 && instruction_specifier <= 0x11){
                        instruction = instruction_specifier & 0xfe;
                        register_bit = instruction_specifier & 0x01;
                        //printf("bbbb bbbr: register bit set to %d\n", register_bit);
                }
                // instructions > 0x11 and <= 0x27 only specify address mode as either direct or indexed with spec<7>
                else if(instruction_specifier > 0x11 && instruction_specifier <= 0x27){
                        instruction = instruction_specifier & 0xfe;
                        address_mode = (instruction_specifier & 0x01) ? 5 : 0;
                        //printf("bbbb bbba: address mode set to %d\n", address_mode);
                }
                // instructions >0x27 and <= 0x5f have a 3 bit address mode at spec<5...7>
                else if(instruction_specifier > 0x27 && instruction_specifier <= 0x5f){
                        instruction = instruction_specifier & 0xf8;
                        address_mode = instruction_specifier & 0x07;
                        //printf("bbbb baaa: address mode set to %d\n", address_mode);
                }
                // instructions > 0x5f and <= 0xff have a 3 bit address mode at spec<5...7> and register bit at spec<4>
                else if(instruction_specifier > 0x5f && instruction_specifier <= 0xff){
                        instruction = instruction_specifier & 0xf0;
                        address_mode = instruction_specifier & 0x07;
                        register_bit = (instruction_specifier & 0x08) >> 3;
                        //printf("bbbb raaa: address mode set to %d and register bit set to %d\n", address_mode, register_bit);
                }
                // instruction <= 0x05 don't have an address mode/register bit
                else {
                        instruction = instruction_specifier;
                }

                // increment the program counter
                // instruction in first condition are unary, otherwise inc by 3
                if(instruction_specifier >= 0x00 && instruction_specifier <= 0x11 || instruction_specifier >= 0x28 && instruction_specifier <= 0x2f){
                        pc++;
                }
                else {
                        // if the instruction is non-unary, retrieving the operand before incrementing the program counter
                        operand_specifier = (mem[pc+1] << 8) | mem[pc+2];
                        pc += 3;
                }

                // execute the instruction fetched
                switch(instruction){
                        case 0x00: // stop
                                break;
                        case 0x01: // ret
                                break;
                        case 0x02: // rettr
                                break;
                        case 0x03: // movspa
                                break;
                        case 0x04: // movflga
                                break;
                        case 0x05: // movaflg
                                break;
                        case 0x06: // not
                                break;
                        case 0x08: // neg
                                break;
                        case 0x0a: // asl
                                break;
                        case 0x0c: // asr
                                break;
                        case 0x0e: // rol
                                break;
                        case 0x10: // ror
                                break;
                        case 0x12: // br
                                break;
                        case 0x14: // brle
                                break;
                        case 0x16: // brlt
                                break;
                        case 0x18: // breq
                                break;
                        case 0x1a: // brne
                                break;
                        case 0x1c: // brge
                                break;
                        case 0x1e: // brgt
                                break;
                        case 0x20: // br
                                break;
                        case 0x22: // brc
                                break;
                        case 0x24: // call
                                break;
                        case 0x26: // nopn
                                break;
                        case 0x28: // nop
                                break;
                        case 0x30: // deci
                                break;
                        case 0x38: // deco
                                break;
                        case 0x40: // hexo
                                break;
                        case 0x48: // stro
                                break;
                        case 0x50: // addsp
                                break;
                        case 0x58: // subsp
                                break;
                        case 0x60: // add
                                break;
                        case 0x70: // sub
                                break;
                        case 0x80: // and
                                break;
                        case 0x90: // or
                                break;
                        case 0xa0: // cpw
                                break;
                        case 0xb0: // cpb
                                break;
                        case 0xc0: // ldw
                                break;
                        case 0xd0: // ldb
                                ldb(register_bit, operand_specifier, address_mode);
                                break;
                        case 0xe0: // stw
                                break;
                        case 0xf0: // stb
                                stb(register_bit, operand_specifier, address_mode);
                                break;
                }
                

        } while(instruction != 0x00);
}