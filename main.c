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
        int low, high;
        switch(mode){
                // immediate
                case 0:
                        return specifier;
                        break;
                // direct                        
                case 1:
                        low = mem[specifier];
                        high = mem[specifier+1];
                        break;
                // indirect
                case 2:
                        low = mem[mem[specifier]];
                        high = mem[mem[specifier+1]];
                        break;
                // stack-relative
                case 3:
                        low = mem[sp+specifier];
                        high = mem[sp+specifier+1];
                        break;
                // stack-relative deferred
                case 4:
                        low = mem[mem[sp+specifier]];
                        high = mem[mem[sp+specifier+1]];
                        break;
                // indexed
                case 5:
                        low = mem[x+specifier];
                        high = mem[x+specifier+1];
                        break;
                // stack indexed
                case 6:
                        low = mem[x+sp+specifier];
                        high = mem[x+sp+specifier+1];
                        break;
                // stack index deferred
                case 7:
                        low = mem[mem[sp+specifier]+x];
                        high = mem[mem[sp+specifier+1]+x];
                        break;
        }
        return (low << 8) | high;
}

// instructions
void deco(int operand){
        printf("%d", operand);
}

void ldb(int reg, int operand, int operand_specifier){
        // setting working register
        int *working_register = (reg)? &x:&a;

        // making byte-sized: operand<0..7> <- operand<8..15> for operand > 0x00ff
        operand = (operand > 0x00ff)? (operand & 0xff00) >> 8 : operand & 0x00ff;

        // clearing r<8..15> for assignment
        *working_register = *working_register & 0xff00;

        // if operand specifier is 0xfc15, taking from stdin, else loading byte from memory
        if(operand_specifier == 0xfc15){
                *working_register = *working_register | scanf("%x");
        }
        else {
                *working_register = *working_register | operand;
        }

        // setting status bits
        n = 0;
        z = (*working_register)? 0:1;
}

void stb(int reg, int operand, int operand_specifier){
        // setting working register
        int *working_register = (reg)? &x:&a;

        // getting r<8..15>
        int byte = *working_register & 0x00ff;

        // if operand specifier is 0xfc16, printing ot stdout, else loading into working register
        if(operand_specifier == 0xfc16){
                printf("%c", byte);
        }
        else {
                mem[operand] = byte;
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
                int operand;

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
                if(instruction_specifier >= 0x00 && instruction_specifier <= 0x11 || instruction_specifier >= 0x26 && instruction_specifier <= 0x27){
                        pc++;
                }
                else {
                        // if the instruction is non-unary, retrieving the operand before incrementing the program counter
                        operand_specifier = (mem[pc+1] << 8) | mem[pc+2];
                        operand = retrieve_operand(operand_specifier, address_mode);
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
                                deco(operand);
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
                                ldb(register_bit, operand, operand_specifier);
                                break;
                        case 0xe0: // stw
                                break;
                        case 0xf0: // stb
                                stb(register_bit, operand, operand_specifier);
                                break;
                }
        } while(instruction != 0x00);
}