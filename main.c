#include "tomasulo.h"

#define MEM_SIZE 6
#define DATA_SIZE 6

int main() {
  MachineConfig mcfg = {
      .instrMemSize = MEM_SIZE,
      .dataMemSize = DATA_SIZE,
  };

  Machine mach;
  initMachine(&mach, mcfg);

  simulation(&mach);

  endMachine(&mach);

  return EXIT_SUCCESS;
}
