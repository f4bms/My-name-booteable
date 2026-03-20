; Bootloader x86 (16-bit) - Carga el juego en memoria y ejecuta

ORG 0x7C00                  ; Dirección estándar del bootloader
BITS 16                     ; Modo real (16 bits)

%define GAME_OFFSET 0x8000  ; Dirección donde carga el juego
%define SECTOR_AMOUNT 4     ; Cantidad de sectores a leer (4 * 512 bytes)

start:
    ; Inicializa segmentos y pila
    XOR AX, AX
    MOV DS, AX
    MOV ES, AX
    MOV SS, AX
    MOV SP, 0x7C00

    ; Imprime mensaje
    MOV SI, msg_boot
    CALL print

    ; Carga el juego desde disco
    CALL load_sectors

    ; Salta al código del juego
    JMP GAME_OFFSET

halt:
    HLT
    JMP halt

; Imprime una cadena que termina al llegar a 0
; Entrada: SI = dirección de la cadena
print:
    PUSH AX
    PUSH BX
;loop principal, imprime el mensaje
printloop:
    LODSB
    TEST AL, AL
    JZ printdone
    MOV AH, 0x0E
    XOR BX, BX
    INT 0x10 ;interrupcion de video para impresion del mensaje
    JMP printloop

printdone:
    POP BX
    POP AX
    RET

; Carga sectores del disco a memoria (INT 13h)
load_sectors:
    MOV DL, 0x80            ; para que qemu use el disco como tal
    MOV AH, 0x02            ; Función: leer sectores
    MOV AL, SECTOR_AMOUNT
    MOV CH, 0               ; Cilindro
    MOV CL, 2               ; Sector (comienza en 2)
    MOV DH, 0               ; Cabeza
    MOV BX, GAME_OFFSET
    INT 0x13
    RET

msg_boot: DB 'Holap presiona enter para iniciar', 0x0D, 0x0A, 0

TIMES 510-($-$$) DB 0       ; Rellena hasta byte 510
DW 0xAA55                   ; Firma mágica del bootloader