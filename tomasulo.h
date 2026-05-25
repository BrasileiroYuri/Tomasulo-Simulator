#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>

typedef const char *String; /* alias para cadeias de caracteres. */
typedef int Value;          /* Nosso valor é um inteiro. */

typedef enum {
  NONE = 0, //! Estado inicial.
  ISSUE,    //! Estado de Issue.
  EXECUTE,  //! Estado de execução.
  WRITE,    //! Estado de escrita.
} State;

/*
 * @brief Registrador.
 */
typedef struct {
  uint8_t num; //! Número do registrador.
  Value value; //! Valor armazenado.
} Register;

/*
 * @brief Struct que representa uma instrução.
 */
typedef struct {
  String name;     //! Nome da instrução, ex: SUB, ADD, etc.
  Register dest;   //! Registrador destino.
  Register src1;   //! Registrador fonte 1.
  Register src2;   //! Registrador fonte 2.
  State currState; //! Estado atual instrução.
} Instruction;

/*
 * @brief Fila de instruções.
 */
typedef struct {
  uint8_t size;
  Instruction *queue;
} InstructionQueue;

/*
 * @brief RS (Reservation Station).
 */
typedef struct ReservationStation {
  String name;                   //! Nome da Reservation Station.
  bool busy;                     //! Define se a unidade está ocupada.
  String op;                     //! Define a operação da instrução atual na RS.
  Value *Vj;                     //! Valor do primeiro operando.
  Value *Vk;                     //! Valor do primeiro operando.
  struct ReservationStation *Qj; //! RS responsável por retornar Vj.
  struct ReservationStation *Qk; //! RS responsável por retornar Vk.
  bool Rj;                       //! Define se Vj está disponível.
  bool Rk;                       //! Define se Vk está disponível.
} ReservationStation;

/*
 * @brief Unidade de Load e Store.
 */
typedef struct {
  String name;     //! Nome da Load/Store unit.
  bool busy;       //! Define se a unidade está ocupada.
  uint8_t address; //! Endereço de escrita ou leitura.
} LSUnit;

/*
 * @brief Lista de registradores.
 * Faz associação baseada em índice, ou seja, se para o índice i, stations[i] !=
 * null, então tem uma RS para escrever no register R onde R.num = i.
 */
typedef struct {
  uint8_t size;                 //! Tamanho da lista abaixo.
  ReservationStation *stations; //! Lista de RS para ordenado por Register.
} RegisterTable;

/*
 * @brief Representa um barramento, ou seja, apenas um canal com uma instrução.
 */
typedef struct {
  Instruction *instr; //! Instrução presente no barramento.
} Bus;

/*
 * @brief Memória de instruções. Essa memória alimenta a InstructionQueue.
 */
typedef struct {
  size_t size;      //! Tamanho alocado.
  Instruction *mem; //! Lista de instruções.
} InstructionMemory;

/*
 * @brief Memória de dados.
 */
typedef struct {
  size_t size; //! Tamanho alocado.
  int *mem;    //! Lista de dados.
} DataMemory;

/*
 * @brief Struct responsável por ser o "top level" da máquina.
 * Contém os componentes e guarda o estado da maquina.
 */
typedef struct {
  InstructionMemory instrMem;   //! Memória de instrução.
  InstructionQueue queue;       //! Fila de instruções.
  ReservationStation *stations; //! Conjunto de RSs da arquitetura.
  LSUnit *lsUnits;              //! Conjunto de Load/Store units.
  RegisterTable regTable;       //! Tabela de registradores.
  Bus cdb;                      //! Barramento CDB.
} Machine;

/*
 * @brief método responsável por alocar memória para a fila.
 * @param size tamanho da fila (quantidade de instruções).
 */
void init_queue(InstructionQueue *, size_t);

void createMemory(Machine *, size_t);
