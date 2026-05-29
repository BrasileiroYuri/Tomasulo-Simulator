; ====================================================================
; EXEMPLO DOS SLIDES (AULA DE TOMASULO)
; Baseado no IBM 360/91 - Página 23 dos slides (Resumo das Instruções)
; ====================================================================

LD    R6, 34(R2)      ; 1. L.D F6, 34(R2)   - Carrega dado para R6
LD    R2, 45(R3)      ; 2. L.D F2, 45(R3)   - Carrega dado para R2
MUL   R0, R2, R4      ; 3. MUL.D F0, F2, F4 - RAW com a instrução 2 (R2)
SUB   R8, R6, R2      ; 4. SUB.D F8, F6, F2 - RAW com a instrução 1 e 2
DIV   R10, R0, R6     ; 5. DIV.D F10, F0, F6 - RAW com a instrução 3 e 1
ADD   R6, R8, R2      ; 6. ADD.D F6, F8, F2 - RAW (inst. 4 e 2) e WAW/WAR com inst. 1
