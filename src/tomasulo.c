#include "../include/tomasulo.h"
#include <stddef.h>
#include <stdio.h>
#include <string.h>

void createQueue(InstructionQueue *queue, size_t size) {
  printf("\t- Instanciando fila de instruções.\n\t- Size: %zu\n\n", size);

  queue->queue = malloc(size * sizeof(Instruction));
  queue->size = size;
}

void createInstructionMemory(InstructionMemory *instrMem, size_t size) {
  printf("\t- Instanciando memória de instrução.\n\t- Size: %zu\n\n", size);

  instrMem->mem = malloc(size * sizeof(Instruction));
  instrMem->size = size;
}

void initMachine(Machine *mach, MachineConfig mcfg) {
  printf(">>> Iniciando máquina:\n");

  //! - [1] Alocando memória necessária.
  createInstructionMemory(&mach->instrMem, mcfg.instrMemSize);
  createDataMemory(&mach->dataMem, mcfg.dataMemSize);
  createRegFile(&mach->regFile, mcfg.dataMemSize);
  createQueue(&mach->queue, mcfg.instrMemSize);
  createStations(mach, mcfg);
  createRegisterTable(&mach->regTable, mcfg.RegFileSize);

  //! - [2] Iniciando componentes.
}

void endMachine(Machine *mach) {

  free(mach->instrMem.mem);
  free(mach->dataMem.mem);
  free(mach->regFile.regs);
  free(mach->queue.queue);
  free(mach->regTable.stations);

  printf("\n>>> Encerrando máquina.\n");
};

void createRegFile(RegisterFile *regFile, size_t size) {
  printf("\t- Instanciando RegisterFile.\n\t- Size: %zu\n\n", size);
  regFile->regs = malloc(size * sizeof(Register));
};

void createDataMemory(DataMemory *dataMem, size_t size) {
  dataMem->mem = malloc(size * sizeof(int));
  dataMem->size = size;
};

// Retorna true se houver alguma Estacao de Reserva ocupada trabalhando
bool hasBusyStations(Machine *mach) {
    for (size_t i = 0; i < mach->numStations; i++) {
        if (mach->stations[i].busy) {
            return true;
        }
    }
    return false;
}
void simulation(Machine *mach) {
  printf(">>> Iniciando simulação:\n");

  size_t pc = 0;
  int clock = 1;

  while (pc < mach->queue.size || hasBusyStations(mach)) {

    //! [1] ISSUE (Despacho)
    if (pc < mach->queue.size) {
      issueInstruction(mach, &pc);
    }
    
    //! [2] EXECUTE (Execução)
    executeInstructions(mach);

    //! [3] WRITE RESULT (Escrita no CDB)
    writeResult(mach);

    printState(mach, clock);
    
    clock++;
    
    // Trava de segurança limite, para teste
    if (clock > 50) {
        printf("\n[!] Limite de 50 ciclos atingido. Encerrando por seguranca.\n");
        break;
    }
  }

  printf(">>> Finalizando simulação.\n");
}

void createRegisterTable(RegisterTable *regTable, size_t size) {
    printf("\t- Instanciando RegisterTable (Renomeacao).\n");
    regTable->size = size;
    regTable->stations = calloc(size, sizeof(ReservationStation *));
}

//Função para remover comentários do .asm
//Tente sempre comentar o asm para entender melhor as operaçõelse {
void cleanLine(char *line) {
    char *comment = strchr(line, ';');
    if (comment) {
        *comment = '\0';
    }
}

//Funão que converte string do registrador em inteiro 
uint8_t parseRegister(const char *regStr) {
    if (regStr[0] == 'R' || regStr[0] == 'r') {
        return (uint8_t)atoi(regStr + 1);
    }
    return 0; // Se houver erro
}
//Função para carregar o arquivoA

void loadFile(const char *filepath, Machine *mach) {
    FILE *file = fopen(filepath, "r");
    if (!file) {
        perror("Erro ao abrir o arquivo assembly");
        exit(EXIT_FAILURE);
    }

    char line[256];
    size_t count = 0;

    printf(">>> Lendo arquivo: %s\n", filepath);
    //Isso aqui tive que ir de GEPETO
    while (fgets(line, sizeof(line), file) && count < mach->instrMem.size) {
        cleanLine(line);

        if (strlen(line) <= 1) continue;

        char op[16] = {0};
        char arg1[16] = {0}, arg2[16] = {0}, arg3[16] = {0};

        // Lê a operação e separa o resto. 
        int args = sscanf(line, " %s %[^,], %[^,], %s", op, arg1, arg2, arg3);

        if (args >= 2) { 
            Instruction instr;
            instr.currState = NONE;
            
            // strdup aloca memória para a string constante
            instr.name = strdup(op); 

            instr.dest.num = parseRegister(arg1);
            instr.dest.value = 0;

            // Tratamento especial para LD e SD (formato: 0(R2))
            if (strcmp(op, "LD") == 0 || strcmp(op, "SD") == 0) {
                int offset;
                char regBuf[8];
                // Separa "32" e "R2" de "32(R2)"
                if (sscanf(arg2, "%d(%[^)])", &offset, regBuf) == 2) {
                    instr.src1.num = parseRegister(regBuf);
                    instr.src2.num = 255;
                    instr.src2.value = offset; 
                }
            } else {
                instr.src1.num = parseRegister(arg2);
                
                if (arg3[0] == '#') {
                    instr.src2.num = 255; 
                    instr.src2.value = atoi(arg3 + 1);
                } else {
                    instr.src2.num = parseRegister(arg3);
                }
            }

            // Salva a instrução nas memórias da máquina
            mach->instrMem.mem[count] = instr;
            mach->queue.queue[count] = instr;
            count++;
        }
    }

    // Ajusta o tamanho real ocupado pelas instruções
    mach->instrMem.size = count;
    mach->queue.size = count;

    fclose(file);
    printf("\t- %zu instrucoes carregadas na memória.\n\n", count);
}

void createStations(Machine *mach, MachineConfig mcfg) {
    // O total de estações de reserva da ALU será a soma das de ADD e MUL
    mach->numStations = mcfg.numAddStations + mcfg.numMulStations;
    
    printf("\t- Instanciando Estacoes de Reserva.\n");
    printf("\t- ADD/SUB: %zu | MUL/DIV: %zu\n\n", mcfg.numAddStations, mcfg.numMulStations);

    mach->stations = malloc(mach->numStations * sizeof(ReservationStation));

    //Inicializando
    for (size_t i = 0; i < mcfg.numAddStations; i++) {
        mach->stations[i].type = RS_TYPE_ADD;
        
        char nameBuf[16];
        sprintf(nameBuf, "ADD%zu", i + 1);
        mach->stations[i].name = strdup(nameBuf); 
        
        mach->stations[i].busy = false;
        mach->stations[i].op = NULL;
        mach->stations[i].Qj = NULL;
        mach->stations[i].Qk = NULL;
        mach->stations[i].Vj = 0;
        mach->stations[i].Vk = 0;
        mach->stations[i].Rj = false;
        mach->stations[i].Rk = false;
    }

    // Inicializando as Estações de MUL/DIV
    for (size_t i = 0; i < mcfg.numMulStations; i++) {
        size_t idx = mcfg.numAddStations + i; 
        
        mach->stations[idx].type = RS_TYPE_MUL;
        
        char nameBuf[16];
        sprintf(nameBuf, "MUL%zu", i + 1);
        mach->stations[idx].name = strdup(nameBuf);
        
        mach->stations[idx].busy = false;
        mach->stations[idx].op = NULL;
        mach->stations[idx].Qj = NULL;
        mach->stations[idx].Qk = NULL;
        mach->stations[idx].Vj = 0;
        mach->stations[idx].Vk = 0;
        mach->stations[idx].Rj = false;
        mach->stations[idx].Rk = false;
    }
}

void printState(Machine *mach, int clock) {
    printf("\n=========================================================================\n");
    printf("                              CICLO DE CLOCK: %d\n", clock);
    printf("=========================================================================\n");
    printf(" ESTAÇÕES DE RESERVA\n");
    printf("-------------------------------------------------------------------------\n");
    printf(" %-5s | %-4s | %-4s | %-4s | %-4s | %-5s | %-5s | %-2s | %-2s\n", 
           "Nome", "Busy", "Op", "Vj", "Vk", "Qj", "Qk", "Rj", "Rk");
    printf("-------------------------------------------------------------------------\n");
    
    for (size_t i = 0; i < mach->numStations; i++) {
        ReservationStation *rs = &mach->stations[i];
        
        String qjName = rs->Qj ? rs->Qj->name : "-";
        String qkName = rs->Qk ? rs->Qk->name : "-";
        
        printf(" %-5s | %-4s | %-4s | %-4d | %-4d | %-5s | %-5s | %-2s | %-2s\n",
               rs->name,
               rs->busy ? "Sim" : "Nao",
               rs->op ? rs->op : "-",
               rs->Vj,
               rs->Vk,
               qjName,
               qkName,
               rs->Rj ? "S" : "N",
               rs->Rk ? "S" : "N");
    }
    printf("=========================================================================\n");
}

// Função auxiliar para saber qual tipo de RS a instrução precisa
int getRequiredRSType(String op) {
    if (strcmp(op, "ADD") == 0 || strcmp(op, "SUB") == 0) return RS_TYPE_ADD;
    if (strcmp(op, "MUL") == 0 || strcmp(op, "DIV") == 0) return RS_TYPE_MUL;
    // TEMPORARIO, vamos mapear LD/SD para ADD
    if (strcmp(op, "LD") == 0 || strcmp(op, "SD") == 0) return RS_TYPE_ADD; 
    return 0;
}
//Função auxiliar apenas para definir a demora do ciclo
int getLatency(String op) {
    if (strcmp(op, "MUL") == 0) return 5;  // Multiplicação demora mais
    if (strcmp(op, "DIV") == 0) return 10; // Divisão demora muito
    return 2; // ADD, SUB, LD, SD demoram 2 ciclos por padrão
}

bool issueInstruction(Machine *mach, size_t *pc) {
    if (*pc >= mach->queue.size) return false; 

    Instruction *instr = &mach->queue.queue[*pc];
    int requiredType = getRequiredRSType(instr->name);

    ReservationStation *freeRS = NULL;
    for (size_t i = 0; i < mach->numStations; i++) {
        if (mach->stations[i].type == requiredType && mach->stations[i].busy == false) {
            freeRS = &mach->stations[i];
            break; 
        }
    }

    // Se não achou RS livre, ocorre um STALL
    if (freeRS == NULL) {
        return false; 
    }

    freeRS->busy = true;
    freeRS->op = instr->name;

    if (instr->src1.num != 255) { // Se for um registrador válido
        ReservationStation *producer = mach->regTable.stations[instr->src1.num];
        if (producer == NULL) { // Valor está pronto no Register File
            freeRS->Vj = mach->regFile.regs[instr->src1.num].value;
            freeRS->Qj = NULL;
            freeRS->Rj = true;
        } else { // Valor vai ser produzido por outra RS
            freeRS->Qj = producer;
            freeRS->Rj = false;
        }
    }

    if (instr->src2.num == 255) { // É um valor imediato
        freeRS->Vk = instr->src2.value;
        freeRS->Qk = NULL;
        freeRS->Rk = true;
    } else { // É um registrador
        ReservationStation *producer = mach->regTable.stations[instr->src2.num];
        if (producer == NULL) { // Pronto no RegFile
            freeRS->Vk = mach->regFile.regs[instr->src2.num].value;
            freeRS->Qk = NULL;
            freeRS->Rk = true;
        } else { // Aguardando outra RS
            freeRS->Qk = producer;
            freeRS->Rk = false;
        }
    }

    // Instruções SD (Store) não escrevem em registradores, então ignoramos
    if (strcmp(instr->name, "SD") != 0) {
        mach->regTable.stations[instr->dest.num] = freeRS;
    }
    
    freeRS->cyclesLeft = getLatency(instr->name);

    instr->currState = ISSUE; // Atualiza status da instrução
    (*pc)++;                  // Avança para a próxima instrução na fila

    return true;
}

void executeInstructions(Machine *mach) {
    for (size_t i = 0; i < mach->numStations; i++) {
        ReservationStation *rs = &mach->stations[i];
        
        if (rs->busy && rs->Rj && rs->Rk && rs->cyclesLeft > 0) {
            
            rs->cyclesLeft--; // Consome 1 ciclo de clock
            
            // Se terminou a execução neste ciclo, calcula o resultado!
            if (rs->cyclesLeft == 0) {
                if (strcmp(rs->op, "ADD") == 0) rs->result = rs->Vj + rs->Vk;
                else if (strcmp(rs->op, "SUB") == 0) rs->result = rs->Vj - rs->Vk;
                else if (strcmp(rs->op, "MUL") == 0) rs->result = rs->Vj * rs->Vk;
                else if (strcmp(rs->op, "DIV") == 0) {
                    if (rs->Vk != 0) rs->result = rs->Vj / rs->Vk;
                    else rs->result = 0; // Evita divisão por zero
                }
                // Para simplificar LD/SD no simulador base, passamos o valor da memória/offset
                else if (strcmp(rs->op, "LD") == 0 || strcmp(rs->op, "SD") == 0) {
                    rs->result = rs->Vj + rs->Vk; 
                }
            }
        }
    }
}

//Função que Retorna true se  escreveu no CDB 
bool writeResult(Machine *mach) {
    for (size_t i = 0; i < mach->numStations; i++) {
        ReservationStation *rs = &mach->stations[i];
        
        if (rs->busy && rs->Rj && rs->Rk && rs->cyclesLeft == 0) {
            
            Value outResult = rs->result;


            for (size_t j = 0; j < mach->numStations; j++) {
                ReservationStation *waiter = &mach->stations[j];
                if (waiter->busy) {
                    if (waiter->Qj == rs) {
                        waiter->Vj = outResult;
                        waiter->Qj = NULL;
                        waiter->Rj = true;
                    }
                    if (waiter->Qk == rs) {
                        waiter->Vk = outResult;
                        waiter->Qk = NULL;
                        waiter->Rk = true;
                    }
                }
            }

            for (size_t r = 0; r < mach->regTable.size; r++) {
                if (mach->regTable.stations[r] == rs) {
                    mach->regFile.regs[r].value = outResult;
                    mach->regTable.stations[r] = NULL;       
                }
            }

            rs->busy = false;
            rs->op = NULL;
            
            // Com 1 CDB, apenas 1 instrução escreve por ciclo.
            return true; 
        }
    }
    return false; 
}

// Retorna true se houver alguma Estacao de Reserva ocupada trabalhando
