#include "../include/tomasulo.h"
#include <stdio.h>

void createQueue(InstructionQueue *queue, size_t size) {
  printf("\t- Instanciando fila de instruções.\n\t- Size: %zu\n\n", size);

  queue->queue = malloc(size * sizeof(Instruction));
  queue->size = size;
}

void createInstructionMemory(InstructionMemory *instrMem, size_t size) {
  printf("\t- Instanciando memória de instrução.\n\t- Size: %zu\n\n", size);

  instrMem->mem = malloc(size * sizeof(Instruction));
  instrMem->size = size;
}

void initMachine(Machine *mach, MachineConfig mcfg) {
  printf(">>> Iniciando máquina:\n");

  //! - [1] Alocando memória necessária.
  createInstructionMemory(&mach->instrMem, mcfg.instrMemSize);
  createDataMemory(&mach->dataMem, mcfg.dataMemSize);
  createRegFile(&mach->regFile, mcfg.dataMemSize);
  createQueue(&mach->queue, mcfg.instrMemSize);

  //! - [2] Iniciando componentes.
}

void endMachine(Machine *mach) {

  free(mach->instrMem.mem);
  free(mach->dataMem.mem);
  free(mach->regFile.regs);
  free(mach->queue.queue);

  printf("\n>>> Encerrando máquina.\n");
};

void createRegFile(RegisterFile *regFile, size_t size) {
  printf("\t- Instanciando RegisterFile.\n\t- Size: %zu\n\n", size);
  regFile->regs = malloc(size * sizeof(Register));
};

void createDataMemory(DataMemory *dataMem, size_t size) {
  dataMem->mem = malloc(size * sizeof(int));
  dataMem->size = size;
};

void simulation(Machine *mach) {
  printf(">>> Iniciando simulação:\n");

  size_t done = 0;
  size_t size = mach->instrMem.size;

  while (done < size) {
    //! Colocar mais uma instrução em Issue...
  }

  printf(">>> Finalizando simulação.\n");
}
