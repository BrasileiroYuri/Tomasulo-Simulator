#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>

#define RS_TYPE_ADD 1
#define RS_TYPE_MUL 2

typedef const char *String; /* Alias para cadeias de caracteres. */
typedef int Value;          /* Nosso valor é um inteiro. */

typedef enum {
  NONE = 0, //! Estado inicial.
  ISSUE,    //! Estado de Issue.
  EXECUTE,  //! Estado de execução.
  WRITE,    //! Estado de escrita.
  DONE,     //! Instrução completa.
} State;

/*
 * @brief Registrador.
 */
typedef struct {
  uint8_t num; //! Número do registrador.
  Value value; //! Valor armazenado.
} Register;

/*
 * @brief Conjuntos de processadores da máquina.
 */
typedef struct {
  Register *regs; //! Lista de registradores.
} RegisterFile;

/*
 * @brief Struct que representa uma instrução.
 */
typedef struct {
  size_t rsId;     //! Define que tipo de RS pode carregá-la.
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
  size_t type; //! Útil para saber qual Instruction essa RS pode usar.
  String name; //! Nome da Reservation Station.
  bool busy;   //! Define se a unidade está ocupada.
  String op;   //! Define a operação da instrução atual na RS.
  Value Vj;    //! Valor do primeiro operando.
  Value Vk;    //! Valor do primeiro operando.
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
  uint8_t size;                  //! Tamanho da lista abaixo.
  ReservationStation **stations; //! Lista de RS para ordenado por Register.
} RegisterTable;

/*
 * @brief Representa um barramento.
 */
typedef struct {
  String tag;  //! Nome da RS que está broadcastando
  Value value; //! Resultado
  bool valid;  //! Define se tem algo no barramento nesse ciclo
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
  DataMemory dataMem;           //! Memória de dados.
  RegisterFile regFile;         //! Lista de registradores.
  InstructionQueue queue;       //! Fila de instruções.
  ReservationStation *stations; //! Conjunto de RSs da arquitetura.
  size_t numStations;           //! Guarda o total de RS(ADD+MUL)
  LSUnit *lsUnits;              //! Conjunto de Load/Store units.
  size_t numLSUnits;            //! Guarda o total de LOAD/STORE
  RegisterTable regTable;       //! Tabela de registradores.
  Bus cdb;                      //! Barramento CDB.
} Machine;

/*
 * @brief Struct de configuração da arquitetura.
 */
typedef struct {
  size_t instrMemSize;     //! Tamanho da memória de instrução.
  size_t dataMemSize;      //! Tamanho da memória de data.
  size_t RegFileSize;      //! Tamanho da memória de registradores.
  size_t numAddStations;   //! Tamanho da quantidade de "vagas" para soma/subtração
  size_t numMulStations;   //! Tamanho da quantidade de "vagas" para multiplicação/divisão
} MachineConfig;

/*
 * @brief Aloca memória e inicia atributos de uma fila de instruções.
 *
 * @param Fila a ser preenchida.
 * @param Tamanho a ser alocado.
 */
void createQueue(InstructionQueue *, size_t);

/*
 * @brief Cria memória de instruções.
 *
 * @param Fila a ser preenchida.
 * @param Tamanho a ser alocado.
 */
void createInstructionMemory(InstructionMemory *, size_t);

/*
 * @brief Cria memória de dados.
 *
 * @param Memória de dados a ser alocada.
 * @param Tamanho a ser alocado.
 */
void createDataMemory(DataMemory *, size_t);

/*
 * @brief Cria Register File.
 *
 * @param Memória de registradores a ser alocada.
 * @param Tamanho a ser alocado.
 */
void createRegFile(RegisterFile *, size_t);

/*
 * @brief Função para iniciar rotinas de iniciação da máquina, como alocar
 * memória.
 *
 * @param Máquina a ser iniciada.
 * @param Configuração da máquina.
 */
void initMachine(Machine *, MachineConfig);

/*
 * @brief Função para finalizar rotinas de iniciação da máquina.
 *
 * @param Máquina a ser iniciada.
 * @param Configuração da máquina.
 */
void endMachine(Machine *);

/*
 * @brief Função responsável pelo loop de simulação.
 *
 * @param Máquina a ser simulada.
 */
void simulation(Machine *);

/*
 * @brief Lê o arquivo assembly e preenche a fila e memória de instruções.
 *
 * @param Caminho do arquivo .asm.
 * @param Máquina a ser preenchida.
 */
void loadFile(const char *filepath, Machine *mach);

/*
 * @brief Aloca memória e inicializa as Estações de Reserva (Reservation Stations).
 * * Cria dinamicamente as estações para as unidades de Adição/Subtração e 
 * Multiplicação/Divisão baseando-se nas quantidades definidas na configuração. 
 *
 * @param mach Ponteiro para a máquina onde as estações serão instanciadas.
 * @param mcfg Configuração da máquina contendo a quantidade de estações de cada tipo.
 */
void createStations(Machine *mach, MachineConfig mcfg);

/*
 * @brief Imprime o estado atual das Estações de Reserva e Registradores no terminal.
 *
 * @param mach Ponteiro para a máquina.
 * @param clock Ciclo de clock atual.
 */
void printState(Machine *mach, int clock);
