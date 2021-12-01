#include <stdio.h>

// declaring memory and registers as globals
int mem[0xffff + 1];
int a, x, n, z, v, c, pc, sp;

void mem_dump(int start, int end){
        // dumping first 100 addresses
        for(int i=start;i<end;i++){
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

void movspa(){
	a = sp;
}

void movflga(){
	a = a & 0xff00;
	a = a | (n << 3 + z << 2 + v << 1 + c);
}

void movaflg(){
	n = (a & 0x0008) >> 3;
	z = (a & 0x0004) >> 2;
	v = (a & 0x0002) >> 1;
	c = (a & 0x0001);
}

void br(int operand){
	// branch unconditional
        pc = operand;
}

void brle(int operand){
	// branch if less than or equal
	pc = (n||z)? operand:pc;
}

void brlt(int operand){
	// branch if less than
	pc = (n)? operand:pc;
}

void breq(int operand){
	// branch if equal
	pc = (z)? operand:pc;
}

void brne(int operand){
	// branch if not equal
	pc = (~z)? operand:pc;
}

void brge(int operand){
	// branch if greater than or equal
	pc = (~n)? operand:pc;
}

void brgt(int operand){
	// branch if greater than
	pc = (~n&~z)? operand:pc;
}

void brv(int operand){
	// branch if overflow or something idk
	pc = (v)? operand:pc;
}

void brc(int operand){
	// branch if carry???????
	pc = (c)? operand:pc;
}

void deco(int operand){
        printf("%d", operand);
}

void hexo(int operand){
        printf("%04X", operand);
}

void addsp(int operand){
        sp += operand;
}

void subsp(int operand){
        sp -= operand;
}
void add(int *reg, int operand){
        *reg+=operand;
        //setting the status bits
        n = (*reg<0)? 1:0;
        z = (*reg)? 0:1;
}
void sub(int *reg, int operand){
        *reg-=operand;
        //setting the status bits
        n = (*reg<0)? 1:0;
        z = (*reg)? 0:1;
}
void and(int *reg, int operand){
        *reg = *reg & operand;
        //setting the status bits
        n = (*reg<0)? 1:0;
        z = (*reg)? 0:1;
}

void or(int *reg, int operand){
        *reg = *reg | operand;
        //setting the status bits
        n = (*reg<0)? 1:0;
        z = (*reg)? 0:1;
}

void cpw(int *reg, int operand){
        int t = *reg - operand;
        //setting the status bits
        n = (t<0)? 1:0;
        z = (t)? 0:1;
        if(*reg>0 && operand<0){
                v = (t>0x10000)? 1:0;

        }else if(*reg<0 && operand>0){
                v = (t<-65535)? 1:0;

        }else{
                v = 0;
        }

}


void ldw(int *reg, int operand, int specifier){
        // if operand specifier is 0xfc15 taking from stdin, else loading byte from memory
        if(specifier == 0xfc15){
                *reg = scanf("%x");
        }
        else {
                *reg = operand;
        }

        // setting status bits
        n = (operand < 0)? 1:0;
        z = (operand)? 0:1;
}

void ldb(int *reg, int operand, int specifier){
        // making byte-sized: operand<0..7> <- operand<8..15> for operand > 0x00ff
        operand = (operand > 0x00ff)? (operand & 0xff00) >> 8 : operand & 0x00ff;

        // clearing r<8..15> for assignment
        *reg = *reg & 0xff00;

        // if operand specifier is 0xfc15 taking from stdin, else loading byte from memory
        if(specifier == 0xfc15){
                *reg = *reg | scanf("%x");
        }
        else {
                *reg = *reg | operand;
        }

        // setting status bits
        n = 0;
        z = (*reg)? 0:1;
}

void stw(int *reg, int specifier, int mode){
        // splitting register into low and high
        int low, high;

        low = (*reg  & 0xff00) >> 8;
        high = *reg & 0x00ff;

        // if operand specifier is 0xfc16, printing to stdout, else storing at appropriate place
        if(specifier == 0xfc16){
                printf("%c", *reg);
        }
        else {
                switch(mode){
                // direct                        
                case 1:
                        mem[specifier] = low;
                        mem[specifier+1] = high;
                        break;
                // indirect
                case 2:
                        mem[mem[specifier]] = low;
                        mem[mem[specifier]+1] = high;
                        break;
                // stack-relative
                case 3:
                        mem[sp+specifier] = low;
                        mem[sp+specifier+1] = high;
                        break;
                // stack-relative deferred
                case 4:
                        mem[mem[sp+specifier]] = low;
                        mem[mem[sp+specifier]+1] = high;
                        break;
                // indexed
                case 5:
                        mem[x+specifier] = low;
                        mem[x+specifier+1] = high;
                        break;
                // stack indexed
                case 6:
                        mem[x+sp+specifier] = low;
                        mem[x+sp+specifier+1] = high;
                        break;
                // stack index deferred
                case 7:
                        mem[mem[sp+specifier]+x] = low;
                        mem[mem[sp+specifier]+x+1] = high;
                        break;
                default:
                        printf("!! illegal address mode %d !!", mode);
                }
        }
}

void stb(int *reg, int specifier, int mode){
        // getting r<8..15>
        int byte = *reg & 0x00ff;

        // if operand specifier is 0xfc16, printing to stdout, else storing at appropriate place
        if(specifier == 0xfc16){
                printf("%c", byte);
        }
        else {
                switch(mode){
                // direct                        
                case 1:
                        mem[specifier] = byte;
                        break;
                // indirect
                case 2:
                        mem[mem[specifier]] = byte;
                        break;
                // stack-relative
                case 3:
                        mem[sp+specifier] = byte;
                        break;
                // stack-relative deferred
                case 4:
                        mem[mem[sp+specifier]] = byte;
                        break;
                // indexed
                case 5:
                        mem[x+specifier] = byte;
                        break;
                // stack indexed
                case 6:
                        mem[x+sp+specifier] = byte;
                        break;
                // stack index deferred
                case 7:
                        mem[mem[sp+specifier]+x] = byte;
                        break;
                default:
                        printf("!! illegal address mode %d !!", mode);
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

        mem_dump(0, 100); // to see if the program was successfully loaded (remove in final submission)

        // initialize program counter and stack pointer
        pc = 0;
        sp = 0xfb8f;
        
        // executing instructions until stop is encountered
        int instruction = 0x00;
        do {
                // fetch next instruction
                int instruction_specifier = mem[pc];
                
                // decode the instruction specifier
                int register_bit;
                int *working_register;
                int operand_specifier;
                int address_mode;
                int operand;

                // instructions >0x05 and <= 0x11 only have a register bit at spec<7>, so we should extract that and set it
                if(instruction_specifier > 0x05 && instruction_specifier <= 0x11){
                        instruction = instruction_specifier & 0xfe;
                        register_bit = instruction_specifier & 0x01;
                        working_register = (register_bit)? &x:&a;
                }
                // instructions > 0x11 and <= 0x27 only specify address mode as either direct or indexed with spec<7>
                else if(instruction_specifier > 0x11 && instruction_specifier <= 0x27){
                        instruction = instruction_specifier & 0xfe;
                        address_mode = (instruction_specifier & 0x01) ? 5 : 0;
                }
                // instructions >0x27 and <= 0x5f have a 3 bit address mode at spec<5...7>
                else if(instruction_specifier > 0x27 && instruction_specifier <= 0x5f){
                        instruction = instruction_specifier & 0xf8;
                        address_mode = instruction_specifier & 0x07;
                }
                // instructions > 0x5f and <= 0xff have a 3 bit address mode at spec<5...7> and register bit at spec<4>
                else if(instruction_specifier > 0x5f && instruction_specifier <= 0xff){
                        instruction = instruction_specifier & 0xf0;
                        address_mode = instruction_specifier & 0x07;
                        register_bit = (instruction_specifier & 0x08) >> 3;
                        working_register = (register_bit)? &x:&a;
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
                        // concatenating next two addresses to make the operand
                        operand_specifier = (mem[pc+1] << 8) | mem[pc+2];
                        
                        // if the operand is a signed 2 byte negative, converting it to a signed negative c int
                        if(operand_specifier >= 0x8000 && operand_specifier != 0xfc15 && operand_specifier != 0xfc16){
                                operand_specifier = -1*((~operand_specifier+1) & 0xffff);
                        }

                        // retrieving operand
                        operand = retrieve_operand(operand_specifier, address_mode) & 0xffff;
                                               
                        // incrementing pc
                        pc += 3;
                }

                // execute the instruction fetched
                switch(instruction){
                        case 0x00: // stop
                                // NO IMPLEMENTATION NEEDED
                                break;
                        case 0x01: // ret
                                break;
                        case 0x02: // rettr
                                // NO IMPLEMENTATION NEEDED
                                break;
                        case 0x03: // movspa
                        	movspa();
                                break;
                        case 0x04: // movflga
                        	movflga();
                                break;
                        case 0x05: // movaflg
                        	movaflg();
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
                                br(operand);
                                break;
                        case 0x14: // brle
                        	brle(operand);
                                break;
                        case 0x16: // brlt
                        	brlt(operand);
                                break;
                        case 0x18: // breq
                        	breq(operand);
                                break;
                        case 0x1a: // brne
                        	brne(operand);
                                break;
                        case 0x1c: // brge
                        	brge(operand);
                                break;
                        case 0x1e: // brgt
                        	brgt(operand);
                                break;
                        case 0x20: // brv
                        	brv(operand);
                                break;
                        case 0x22: // brc
                        	brc(operand);
                                break;
                        case 0x24: // call // I CANNOT TAKE MYSELF SERIOUSLY IN ANYTHING I HATE WORKING IN A BUSINESS ENVIRONMENT anyway this is the hard one
                                break;
                        case 0x26: // nopn
                                // NO IMPLEMENTATION NEEDED
                                break;
                        case 0x28: // nop
                                // NO IMPLEMENTATION NEEDED
                                break;
                        case 0x30: // deci
                                break;
                        case 0x38: // deco
                                deco(operand);
                                break;
                        case 0x40: // hexo
                        	hexo(operand);
                                break;
                        case 0x48: // stro
                                break;
                        case 0x50: // addsp
                                addsp(operand);
                                break;
                        case 0x58: // subsp
                                subsp(operand);
                                break;
                        case 0x60: // add
                                add(working_register, operand);
                                break;
                        case 0x70: // sub
                                sub(working_register, operand);
                                break;
                        case 0x80: // and
                                and(working_register, operand);
                                break;
                        case 0x90: // or
                                or(working_register, operand);
                                break;
                        case 0xa0: // cpw
                                cpw(working_register, operand);
                                break;
                        case 0xb0: // cpb
                                break;
                        case 0xc0: // ldw
                                ldw(working_register, operand, operand_specifier);
                                break;
                        case 0xd0: // ldb
                                ldb(working_register, operand, operand_specifier);
                                break;
                        case 0xe0: // stw
                                stw(working_register, operand_specifier, address_mode);
                                break;
                        case 0xf0: // stb
                                stb(working_register, operand_specifier, address_mode);
                                break;
                }
        } while(instruction != 0x00);
}