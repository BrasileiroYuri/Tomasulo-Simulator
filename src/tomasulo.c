#include "tomasulo.h"
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

void createQueue(InstructionQueue *queue, size_t size) {
  printf("- Instanciando fila de instruções.\n\t- Size: %zu\n\n\t", size);

  queue->queue = malloc(size * sizeof(Instruction));
  queue->size = size;
}

void createInstructionMemory(InstructionMemory *instrMem, size_t size) {
  printf("- Instanciando memória de instrução.\n\t- Size: %zu\n\n", size);

  instrMem->mem = malloc(size * sizeof(Instruction));
  instrMem->size = size;
}

void initMachine(Machine *mach, MachineConfig mcfg) {
  printf(">>> Iniciando máquina:\n\t");

  //! - [1] Alocando memória necessária.
  create();

  //! - [2] Iniciando componentes.
}

void endMachine(Machine *mach) {};

void createRegFile(RegisterFile *regFile, size_t size) {
  regFile->regs = malloc(size * sizeof(Register));
};

void createDataMemory(DataMemory *dataMem, size_t size) {
  dataMem->mem = malloc(size * sizeof(int));
  dataMem->size = size;
};

void simulation(Machine *mach) { printf(">>> Iniciando simulação:\n\t"); }
