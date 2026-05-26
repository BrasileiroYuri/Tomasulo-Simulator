#include "../include/tomasulo.h"

#define MEM_SIZE 32
#define DATA_SIZE 32
#define REGFILE_SIZE 32

int main() {
  MachineConfig mcfg = {
      .instrMemSize = MEM_SIZE,
      .dataMemSize = DATA_SIZE,
      .RegFileSize = REGFILE_SIZE,
  };

  Machine mach;
  initMachine(&mach, mcfg);

  simulation(&mach);

  endMachine(&mach);
  return EXIT_SUCCESS;
}
