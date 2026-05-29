; ============================================================
; TESTE 1: Exemplo clássico de Tomasulo (Hennessy & Patterson)
; Adaptado para RISC genérico com registradores R0-R15
; Cenários: RAW, WAW, execução fora de ordem (DIV alta latência)
; ============================================================

LD    R6, 34(R2)      ; I1: Carrega R6 da memória (endereço base R2 + 34)
                      ; [RAW: Será fonte para I4 e I5]

LD    R2, 45(R3)      ; I2: Carrega R2 da memória (endereço base R3 + 45)
                      ; ⚠️ ATENÇÃO: Sobrescreve R2 (ponteiro base!) 
                      ; [RAW: Fonte para I3]

MUL   R0, R2, R4      ; I3: Multiplica R2 (vem I2) com R4
                      ; [RAW com I2] - ALTA LATÊNCIA (multiplicador típico: 4-6 ciclos)
                      ; R0 fica bloqueado até I2 terminar

SUB   R8, R6, R2      ; I4: Subtrai R2 (I2) de R6 (I1)
                      ; [RAW com I1 e I2] - depende de duas cargas
                      ; Pode executar quando ambas terminarem

DIV   R10, R0, R6     ; I5: Divide R0 (I3) por R6 (I1)
                      ; ⚠️ [RAW com I3 e I1] - LATÊNCIA MUITO ALTA (40 ciclos típico)
                      ; R10 fica bloqueado por ~40 ciclos
                      ; Este é o gargalo do benchmark

ADD   R6, R8, R2      ; I6: Soma R8 (I4) com R2 (I2)
                      ; ⚠️ [WAW com I1] - Escreve no mesmo R6 que I1 escreveu
                      ; ⚠️ [WAR] - I1 leu R6 original, I6 escreve R6 depois
                      ; [RAW com I4 e I2] - depende dos resultados
                      ; A renomeação de registradores deve resolver WAW/WAR
