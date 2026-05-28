#include "../include/tomasulo.h"
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

  //! - [2] Iniciando componentes.
}

void endMachine(Machine *mach) {

  free(mach->instrMem.mem);
  free(mach->dataMem.mem);
  free(mach->regFile.regs);
  free(mach->queue.queue);

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

void simulation(Machine *mach) {
  printf(">>> Iniciando simulação:\n");

  size_t done = 0;
  size_t size = mach->instrMem.size;
  int clock = 1;

  while (done < size) {
    //! Colocar mais uma instrução em Issue...
    //Isso aki em baxio é para testar
    printState(mach, clock);
    done++;
    clock++;
  }

  printf(">>> Finalizando simulação.\n");
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
