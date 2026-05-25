#include "tomasulo.h"
#include <stdlib.h>

InstructionQueue *init_queue(uint8_t size) {
  InstructionQueue *queue = malloc(sizeof(InstructionQueue));
  queue->queue = malloc(size * sizeof(Instruction));
  queue->size = size;
  return queue;
}
