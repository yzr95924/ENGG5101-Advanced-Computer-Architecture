#include <stdlib.h>
#include <fcntl.h>
#include <stdint.h>
#include <stdio.h>
#include <sys/wait.h>
#include <string.h>
#include <errno.h>
#include <sys/wait.h>
#include <unistd.h>

#define MAX_MEM_SIZE  128  //The max memory size - (unit: word - 32 bits) 

typedef struct memory{
	uint32_t addr[MAX_MEM_SIZE];
}MEMORY;

typedef struct cpu{
	//Control registers
	uint32_t PC;	// Program counter
	uint32_t IR; 	// Instruction regiser
	uint32_t PSR;	// Processor Status Register
#define PSR_INT_EN      0x1 // Interrupt enable
#define PSR_INT_PEND    0x2 // Interrupt pending

	//General purpose register
	int32_t R[65];	// 4 Registers: R[0-3] (R[4-63] are reserved), R[64]-sp
#define SP  R[64]

    	// Counter
    	uint64_t counter;
}CPU;

typedef struct computer{
	CPU cpu ;
	MEMORY memory;
}COMPUTER;

enum {
    OP_HALT = 0,
    OP_NOP,
    OP_ADDI,
    OP_MOVEREG,
    OP_MOVEI,
    OP_LW,
    OP_SW,
    OP_BLEZ,
    OP_LA,
    OP_PUSH,
    OP_POP,
    OP_ADD,
    OP_JMP,
    OP_IRET,
    OP_PUT,
    OP_NONE = 0xff,
};

int computer_load_init(COMPUTER *, char *);
int cpu_cycle(COMPUTER *);

int print_cpu(COMPUTER * );
int print_memory(COMPUTER * );
int print_instruction(int, uint32_t );

int fetch(COMPUTER * );
int decode(uint32_t , uint8_t * , uint8_t * , uint8_t * , int8_t * );
int execute(COMPUTER *, uint8_t *, uint8_t *, uint8_t *, int8_t *);
int timer_tick(COMPUTER* );
int check_interrupt(COMPUTER* );

int main(int argc, char ** args)
{

	if( argc != 3 ){
		printf("\nUsage: ./icpu ios 16\n");
		printf("\t ios: the os for interrupts; 16: the initial PC\n \n");
		exit(-1);
	}

	COMPUTER comp;

	//Initialize: Load the program into the memory, and initialize all regisrters;
	if ( computer_load_init(&comp, args[1]) < 0 ){    
		printf("Error: computer_poweron_init()\n");
		exit(-1);
	}

	//Set PC and start the cpu execution cycle
	comp.cpu.PC = atoi(args[2]);
	if( comp.cpu.PC >= MAX_MEM_SIZE || comp.cpu.PC < 0){
		printf("Error: start_addr should be in 0-127.\n");
		exit(-1);
	}   


	// Execute CPU cyles: fetch, decode, execution, and increment PC; Repeat
	while(1){
		//printf("\n\nBefore\n");
		//print_cpu(&comp);

		if( cpu_cycle(&comp) < 0 )
			break;

		//printf("After\n");
		//print_cpu(&comp);
	}

  	return 0;
}

int cpu_cycle(COMPUTER * cp)
{
	uint8_t opcode, sreg, treg;
	int8_t  immediate;


	if( fetch(cp) < 0)
		return -1;

	if ( decode(cp->cpu.IR, &opcode, &sreg, &treg, &immediate) < 0)
		return -1;

	if( execute(cp, &opcode, &sreg, &treg, &immediate) < 0 )
		return -1;

    	if (timer_tick(cp) < 0)
        	return -1;

    	if (check_interrupt(cp) < 0)
        	return -1;

	return 0;
}


int fetch(COMPUTER * cp)
{
	//Fetch the instruction to IR from the memory pointed by PC 

	/* Your implemenation here*/
  cp->cpu.IR = cp->memory.addr[cp->cpu.PC];
  if(cp->cpu.IR == NULL){
    return -1;
  }
  print_instruction((int)cp->cpu.PC, cp->cpu.IR);
	return 0;
}

int decode(uint32_t instr, uint8_t * p_opcode, uint8_t * p_sreg, uint8_t * p_treg, int8_t * p_imm)
{
	//For the given instruction 'instr', obtain the following opecode, source/target and immediate value

	/* Your implemenation here*/
  *p_imm = instr & 0xff;
  *p_treg = (instr >> 8) & 0xff;
  *p_sreg = (instr >> 16) & 0xff;
  *p_opcode = (instr >> 24) & 0xff;  
  printf("p_imm: %d\n", *p_imm);
  printf("p_treg: %d\n", *p_treg);
  printf("p_sreg: %d\n", *p_sreg);
  printf("p_opcode: %d\n", *p_opcode);
	return 0;
}

int execute(COMPUTER *cp, uint8_t * p_opcode, uint8_t * p_sreg, uint8_t * p_treg, int8_t * p_imm)
{	
	// Execute the instruction baed on opcode, source/target reg and immediate

	/* Your implemenation here*/
  switch (*p_opcode)
  {
    case OP_HALT:
      printf("This instruction is OP_HALT\n");
      exit(0);
      break;
    case OP_NOP:
      printf("This instruction is OP_NOP\n");
      cp->cpu.PC ++;
      cp->cpu.counter ++;
      break;
    case OP_ADDI:
      printf("This instruction is OP_ADDI\n");
      cp->cpu.R[*p_treg] = cp->cpu.R[*p_sreg] + *p_imm;
      cp->cpu.PC ++;
      cp->cpu.counter ++;
      break;
    case OP_MOVEREG:
      printf("This instruction is OP_MOVEREG\n");
      cp->cpu.R[*p_treg] = cp->cpu.R[*p_sreg];
      cp->cpu.PC ++;
      cp->cpu.counter ++;
      break;
    case OP_MOVEI:
      printf("This instruction is OP_MOVEI\n");
      cp->cpu.R[*p_treg] = *p_imm;
      cp->cpu.PC ++;
      cp->cpu.counter ++;
      break;
    case OP_LW:
      printf("This instruction is OP_LW\n");
      cp->cpu.R[*p_treg] = cp->memory.addr[(cp->cpu.R[*p_sreg] + *p_imm)];
      cp->cpu.PC ++;
      cp->cpu.counter ++;
      break;
    case OP_SW:
      printf("This instruction is OP_SW\n");
      cp->memory.addr[(*p_sreg + *p_imm)] = cp->cpu.R[*p_treg];
      cp->cpu.PC ++;
      cp->cpu.counter ++;
      break;
    case OP_BLEZ:
      printf("This instruction is OP_BLEZ\n");
      if(cp->cpu.R[*p_sreg] <= 0){
        cp->cpu.PC = cp->cpu.PC + 1 + *p_imm;
      }else{
        cp->cpu.PC ++;
      }
      cp->cpu.counter ++;
      break;
    case OP_LA:
      printf("This instruction is OP_LA\n");
      cp->cpu.R[*p_treg] = cp->cpu.PC + 1 + *p_imm;
      cp->cpu.PC ++;
      cp->cpu.counter ++;
      break;
    case OP_ADD:
      printf("This instruction is OP_ADD\n");
      cp->cpu.R[*p_treg] = cp->cpu.R[*p_treg] + cp->cpu.R[*p_sreg];
      cp->cpu.PC ++;
      cp->cpu.counter ++;
      break;
    case OP_JMP:
      printf("This instruction is OP_JMP\n");
      cp->cpu.PC = cp->cpu.PC + 1 + *p_imm;
      cp->cpu.counter ++;
      break;
    case OP_PUSH:
      printf("This instruction is OP_PUSH\n");
      cp->cpu.SP = cp->cpu.SP - 1;
      cp->memory.addr[cp->cpu.SP] = cp->cpu.R[*p_sreg];
      cp->cpu.PC ++;
      cp->cpu.counter ++;
      break;
    case OP_POP:
      printf("This instruction is OP_POP\n");
      cp->cpu.R[*p_treg] = cp->memory.addr[cp->cpu.SP];
      cp->cpu.SP = cp->cpu.SP + 1;
      cp->cpu.PC ++;
      cp->cpu.counter ++;
      break;
    case OP_IRET:
      printf("This instruction is OP_IRET\n");
      // PC <- Pop()
      cp->cpu.PC = cp->memory.addr[cp->cpu.SP];
      cp->cpu.SP = cp->cpu.SP + 1;
      // PSR <- Pop()
      cp->cpu.PSR = cp->memory.addr[cp->cpu.SP];
      cp->cpu.SP = cp->cpu.SP + 1;
      //TODO: PC++?
      cp->cpu.counter ++;
      break;
    case OP_PUT:
      printf("This instruction is OP_PUT\n");
      printf("R[%d]: %d", *p_sreg, cp->cpu.R[*p_sreg]);
      cp->cpu.PC ++;
      cp->cpu.counter ++;
      break;
    default:
      printf("This is an invalid instruction!!!")
      break;
  }

  return 0;
}

int timer_tick(COMPUTER* cp)
{
	//Increment counter by one; when "counter%5000 == 0", set up the interrupt pending 
        //bit if the interrupt enable bit is 1

	/* Your implemenation here*/
  cp->cpu.counter ++;
  if(cp->cpu.counter % 5000 == 0){
    //TODO:
  }
	return 0;
}

int check_interrupt(COMPUTER* cp)
{
	//If the interrupt enable bit and the interrupt pending bit are both one,
        //       1. Save PSR and PC onto the stack by using the code below:
        //			cp->cpu.SP -= 1;
        //			cp->memory.addr[cp->cpu.SP] = cp->cpu.PSR;
        //			cp->cpu.SP -= 1;
        //			cp->memory.addr[cp->cpu.SP] = cp->cpu.PC;
	//       2. Clear up the interrupt pending bit (=0);
        //       3. Disable the interrupt (the interrupt enable bit =s 0) so no nested interrupts
        //       4. Jump to the interrupt handler (the address is stored at memory address 0)


	/* Your implemenation here*/

    	return 0;
}

int computer_load_init(COMPUTER * cp, char * file)
{
    //load the image file
    int fd;
    int ret;

    // open the file
    if ( ( fd = open( file, O_RDONLY ) ) < 0 ){
        printf("Error: open().\n");
        exit(-1);
    }

    // read from the program file (the program file <= 256 bytes) into the memory
    if( ( ret = read( fd, &cp->memory, MAX_MEM_SIZE*4)) < 0 ){
        printf("Error: read().\n");
        exit(-1);
    }else if ( ret > (MAX_MEM_SIZE*4) ) {
        printf("Error: read() - Program is too big. \n");
        exit(-1);
    }

    //Initialize all registers
    cp->cpu.SP=0;   //Stack pointer
    cp->cpu.PC=0;	//Program counter
    cp->cpu.IR=0; 	//Instruction regiser
    cp->cpu.PSR=0;	//Processor Status Register

    //General purpose register
    cp->cpu.R[0]=0;	// General register No. 0
    cp->cpu.R[1]=0;	// General register No. 1
    cp->cpu.R[2]=0;	// General register No. 2
    cp->cpu.R[3]=0;	// General register No. 3

    cp->cpu.counter = 0;
    return 0;
}

int print_cpu(COMPUTER * cp)
{
    printf("CPU Registers: SP-%d, PC-%d, IR-0x%x, PSR-0x%x, R[0]-0x%x, R[1]-0x%x, R[2]-0x%x, R[3]-0x%x\n",
                cp->cpu.SP, cp->cpu.PC, cp->cpu.IR, cp->cpu.PSR, cp->cpu.R[0], cp->cpu.R[1], cp->cpu.R[2], cp->cpu.R[3]);

    return 0;

}

int print_memory(COMPUTER * cp)
{
    //print the memory contents
    int i;
    for (i =0; i< MAX_MEM_SIZE; i++){
        //printf("%x\n", cp->memory.addr[i]);
        print_instruction(i, cp->memory.addr[i]);
    }

    return 0;
}

int print_instruction(int i, uint32_t inst)
{
	int8_t * p = (int8_t *) &inst;

	int8_t low_addr_value = *p;
	int8_t sec_addr_value = *(p+1);
	int8_t third_addr_value = *(p+2);
	int8_t high_addr_value = *(p+3);
	/* Intel32 is little endian - the least significant byte first (lowest address) and the most significant byte last (highest address) */ 
	printf("[%d]: Instruction-0x%x;LowAddr-%d,Second-%d,Third-%d,HighAddr-%d\n",
                    i, inst,low_addr_value, sec_addr_value, third_addr_value, high_addr_value); 
	return 0;
}

