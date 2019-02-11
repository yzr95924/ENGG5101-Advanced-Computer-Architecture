#include <stdlib.h>
#include <fcntl.h> 
#include <stdint.h>
#include <stdio.h>
#include <sys/wait.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>

#define MAX_MEM_SIZE 128 // the max memory size 

typedef struct memory{
  uint32_t addr[MAX_MEM_SIZE];
}MEMORY;

typedef struct cpu{
  //Control registers
  uint32_t PC; // Program counter
  uint32_t IR; // Instruction register
  uint32_t PSR; //Processor Status Register
  #define PSR_INT_EN 0x1 // Interrput enable
  #define PSR_INT_PEND 0x2 // Interrput pending

  //General purpose register
  int32_t R[65]; // 4 Registers R[0-3] R[4-63] are reserved 
  #define SP R[64];

  // Counter
  uint64_t counter;
}CPU;

typedef struct computer
{
  CPU cpu;
  MEMORY memory;
}COMPUTER;

enum{
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
}

int computer_load_init(COMPUTER*, char*);
int cpu_cycle(COMPUTER*);

int print_cpu(COMPUTER*);
int print_memory(COMPUTER*);
int print_instruction(int, uint32_t);

int fetch(COMPUTER*);
int decode(uint32_t, uint8_t*, uint8_t*, uint8_t*,uint8_t*);
int execute(COMPUTER*, uint8_t*, uint8_t*, uint8_t*, uint8_t*);
int timer_tick(COMPUTER*);
int check_interrupt(COMPUTER*);

int main(int argc, char** args)
{

  if(argc != 3){
    printf()
  }
}


int print_instruction(int i, uint32_t inst)
{
  int8_t* p = (int8_t*) &inst;
  int8_t low_addr_value = *p;
  int8_t sec_addr_value = *(p+1);
  int8_t third_addr_value = *(p+2);
  int8_t high_addr_value = *(p+3);
  
  printf("[%d]: Instruction-0x%x;LowAddr-%d, Second-%d, Third-%d, HighAddr-%d\n",
            i, inst, low_addr_value, sec_addr_value, third_addr_value, high_addr_value);
  return 0;
}