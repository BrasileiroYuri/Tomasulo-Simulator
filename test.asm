; ============================================================
; TESTE DE TOMASULO - CENÁRIOS: RAW, WAR, WAW, OUT-OF-ORDER
; Banco: R0..R15 | Arquitetura superescalar (2+ issues/cycle)
; ============================================================

LD    R1, 0(R2)      ; Carrega R1 da memória (endereço base em R2)
                      ; [Cenário: Preparação de dados. RAW posterior com I2]

MUL   R5, R1, R3      ; Multiplica R1 (aguarda I1) com R3
                      ; [RAW com I1] - ALTA LATÊNCIA (~4-6 ciclos)
                      ; R5 bloqueado até I1 terminar

ADD   R6, R4, #10     ; Soma R4 + 10 → R6
                      ; [Independente de I1/I2] - BAIXA LATÊNCIA (1 ciclo)
                      ; Deve executar OUT-OF-ORDER antes do MUL

SD    R6, 16(R2)      ; Guarda R6 na memória (end. R2+16)
                      ; [RAW com I3] - R6 pronto após ADD
                      ; Testa forward de resultado direto da ALU

SUB   R4, R5, R8      ; R4 = R5 - R8
                      ; [RAW com I2] - AGUARDA MUL terminar
                      ; Demonstra atraso na reservation station

ADD   R7, R4, R6      ; [ANTI-DEPENDÊNCIA WAR com I5]
                      ; LÊ R4 (escrito por I5) e R6 (escrito por I3)
                      ; WAR: I6 lê R4, I5 ainda não escreveu? NÃO!
                      ; Na verdade: I5 ESCREVE R4, I6 LÊ R4 → RAW inverso?
                      ; Vamos corrigir para WAR verdadeiro:

; ========= VERSÃO CORRIGIDA COM WAR LEGÍTIMO ==========
LD    R9, 32(R2)      ; I7: Carrega R9 da memória
SUB   R10, R9, R11    ; I8: R10 = R9 - R11 (RAW com I7)
MUL   R12, R10, R13   ; I9: R12 = R10 * R13 (RAW com I8, ALTA LATÊNCIA)
ADD   R10, R14, #5    ; I10: R10 = R14 + 5
                      ; [WAR com I8] - I8 lê R10, I10 escreve R10
                      ; Tomasulo renomeia R10 de I10 para evitar bloqueio
                      ; I10 pode executar antes de I8 terminar!

ADD   R15, R12, R10   ; I11: R15 = R12 + R10
                      ; [WAW com I9/I10] e [RAW com I9 e I10]
                      ; WAW: I9 e I10 escrevem R12/R10 respectivamente
                      ; I11 aguarda AMBAS as fontes
                      ; Cenário crítico para renomeação

SD    R15, 48(R2)     ; I12: Guarda R15 na memória
                      ; [RAW com I11] - verifica commit correto
