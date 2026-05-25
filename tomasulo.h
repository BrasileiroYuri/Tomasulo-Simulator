#include <stdint.h>
#include <stdlib.h>

typedef int8_t Reg; /* Nosso registrador é um tipo númerico. int8_t para em
                       instruções com imm o fonte 2 ser -1. */

typedef struct {
  Reg dest; //! Registrador destino.
  Reg src1; //! Registrador fonte 1.
  Reg src2; //! Registrador fonte 2.
} Instruction;

/*
 * Nossa fila de instruções.
 */
typedef struct {
  uint8_t size;
  Instruction *queue;
} InstructionQueue;

/*
 * @brief método responsável por alocar memória para a fila.
 * @param size tamanho da fila (quantidade de instruções).
 */
InstructionQueue *init_queue(uint8_t size);

void init();
void end();
