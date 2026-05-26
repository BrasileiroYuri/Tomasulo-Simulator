#include "tomasulo.h"
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

void initQueue(InstructionQueue *queue, size_t size) {
  printf("- Instanciando fila de instruções.\n\t- Size: %zu\n\n\t", size);

  queue->queue = malloc(size * sizeof(Instruction));
  queue->size = size;
}

void createInstructionMemory(InstructionMemory *instrMem, size_t size) {
  printf("- Instanciando memória de instrução.\n\t- Size: %zu\n\n\t", size);

  instrMem->mem = (Instruction *)malloc(size * sizeof(Instruction));
  instrMem->size = size;
}

void initMachine(Machine *mach, MachineConfig mcfg) {
  createInstructionMemory(&mach->instrMem, mcfg.instrMemSize);
}

void simulation(Machine *mach) { printf(">>> Iniciando simulação:\n\t"); }
