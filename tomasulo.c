#include "tomasulo.h"
#include <stddef.h>
#include <stdlib.h>

void init_queue(InstructionQueue *queue, size_t size) {
  queue = malloc(sizeof(InstructionQueue));
  queue->queue = malloc(size * sizeof(Instruction));
  queue->size = size;
}

void createMemory(Machine *mach, size_t size) {
  mach->instrMem.mem = (Instruction *)malloc(size * sizeof(Instruction));
}
