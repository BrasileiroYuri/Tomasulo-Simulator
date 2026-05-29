#include "../include/tomasulo.h"
#include <stdio.h>

#define MEM_SIZE 32
#define DATA_SIZE 32
#define REGFILE_SIZE 32

int main(int argc, char *argv[]) {
  if (argc < 2) {
      fprintf(stderr, "Uso incorreto.\nExecute: %s <caminho_do_arquivo.asm>\n", argv[0]);
      return EXIT_FAILURE;
  }

  const char *arquivoAsm = argv[1];

  MachineConfig mcfg = {
      .instrMemSize = MEM_SIZE,
      .dataMemSize = DATA_SIZE,
      .RegFileSize = REGFILE_SIZE,
      .numAddStations = 3,
      .numMulStations = 2,
      .latPattern = 2,   // 4 ciclos para ADD, SUB, LD, SD
      .latMUL = 10,      // 10 ciclos para MUL
      .latDIV = 40       // 40 ciclos para DIV
  };

  Machine mach;

  initMachine(&mach, mcfg);

  loadFile(arquivoAsm, &mach);

  simulation(&mach);

  endMachine(&mach);
  return EXIT_SUCCESS;
}
