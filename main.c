#include "tomasulo.h"
#include <stdio.h>

#define MEM_SIZE 6

int main() {
  printf(">>> Iniciando simulação:\n");

  Machine mach;
  createMemory(&mach, MEM_SIZE);

  return EXIT_SUCCESS;
}
