#include <stdint.h>

typedef int8_t Reg; /* Nosso registrador é um tipo númerico. int8_t para em
                       instruções com imm o fonte 2 ser -1. */

typedef struct {
  Reg dest; //! Registrador destino.
  Reg src1; //! Registrador fonte 1.
  Reg src2; //! Registrador fonte 2.
} Instruction;

typedef struct {
  Instruction *queue;
} InstructionQueue;
