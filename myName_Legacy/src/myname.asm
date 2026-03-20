;el random está fijo
;las rotaciones no se hacen realmente, solo se escribe de lado el texto
BITS 16
ORG 0x8000

%define KEY_UP    0x48
%define KEY_DOWN  0x50
%define KEY_LEFT  0x4B
%define KEY_RIGHT 0x4D
%define KEY_ESC   0x01
%define KEY_ENTER 0x1C

%define ROT_NORMAL 0
%define ROT_FLIP   1
%define ROT_LEFT   2
%define ROT_RIGHT  3

%define MIN_ROW 5
%define MIN_COL 5

;las posiciones por ahora son fijas
start:

randomize:
    MOV  Ah, 00h
    INT  1Ah ;se obtiene el tiempo en ticks desde el boot

    mov ax, dx ;se toma el valor de los ticks
    xor dx, dx ;se limpia el registro dx
    mov cx, 10 
    div cx ;se divide ax por 20, el resultado queda en ax y el resto
    mov al, dl ;se toma el valor del resto para la fila
    add al, MIN_ROW ;se suma el valor minimo para que quede entre 10 y 29

    MOV  BYTE [name_row], AL

    MOV  Ah, 00h
    INT  1Ah ;se obtiene el tiempo en ticks desde el boot

    mov ax, dx ;se toma el valor de los ticks
    xor dx, dx ;se limpia el registro dx
    mov cx, 10 
    div cx 
    mov al, dl ;se toma el valor del resto para la columna
    add al, MIN_COL 
    MOV  BYTE [name_col], AL

;se espera interrupcion de teclado(enter) para comenzar
wait_enter:
    MOV  AH, 0x00
    INT  0x16
    CMP  AH, KEY_ENTER
    JNE  wait_enter

    CALL draw_game

;loop principal, espera teclas para rotar o salir
main_loop:
    MOV  AH, 0x00
    INT  0x16

    CMP  AH, KEY_ESC
    JE   .exit

    CMP  AL, 'r'
    JE   .restart
    CMP  AL, 'R'
    JE   .restart

    CMP  AL, 0x00
    JNE  main_loop

    CMP  AH, KEY_UP
    JE   .up
    CMP  AH, KEY_DOWN
    JE   .down
    CMP  AH, KEY_LEFT
    JE   .left
    CMP  AH, KEY_RIGHT
    JE   .right
    JMP  main_loop

.up:
    MOV  BYTE [rotation], ROT_NORMAL
    JMP  .redraw
.down:
    MOV  BYTE [rotation], ROT_FLIP
    JMP  .redraw
.left:
    MOV  BYTE [rotation], ROT_LEFT
    JMP  .redraw
.right:
    MOV  BYTE [rotation], ROT_RIGHT
    JMP  .redraw
.restart:
    MOV  BYTE [rotation], ROT_NORMAL
    JMP  .redraw
.redraw:
    CALL draw_game
    JMP  main_loop

.exit:
halt:
    MOV  AX, 0x0003
    INT  0x10
    JMP  0x0000:0x7C00


;se establece el cursor (del cursor hay que agarrar la ultima pos)
set_cursor:
    MOV  AH, 0x02
    XOR  BH, BH
    INT  0x10
    RET

;dibuja el juego en la pantalla(las rotaciones realmente no están sirviendo)
draw_game:
    MOV  AX, 0x0003
    INT  0x10

    MOV  AL, [rotation]

    CMP  AL, ROT_NORMAL
    JE   .normal
    CMP  AL, ROT_FLIP
    JE   .flip
    CMP  AL, ROT_LEFT
    JE   .vleft
    JMP  .vright


.normal:
    MOV  DH, [name_row]
    MOV  DL, [name_col]
    CALL set_cursor
    MOV  SI, str_name
    CALL print_string
    RET

;de derecha a izquierda
.flip:
    MOV  SI, str_name
    CALL strlen
    MOV  DH, [name_row]
    MOV  DL, [name_col]
    PUSH SI
    ADD  SI, CX
    DEC  SI
.flip_loop:
    MOV  AL, [SI]
    CALL print_init
    INC  DL
    DEC  SI
    LOOP .flip_loop
    POP  SI
    RET

;vertical para abajo
.vright:
    MOV  SI, str_name
    MOV  DH, [name_row]
    MOV  DL, [name_col]
.vright_loop:
    LODSB
    TEST AL, AL
    JZ   .vright_done
    CALL print_init
    INC  DH
    CMP  DH, 24
    JGE  .vright_done
    JMP  .vright_loop
.vright_done:
    RET

;vertical para arriba
.vleft:
    MOV  SI, str_name
    CALL strlen          ; CX = longitud
    ADD  SI, CX
    DEC  SI              ; SI apunta al último carácter
    MOV  DH, [name_row]
    MOV  DL, [name_col]
.vleft_loop:
    MOV  AL, [SI]
    CALL print_init
    INC  DH              ; baja una fila (igual que vright)
    CMP  DH, 24
    JGE  .vleft_done
    DEC  SI              ; carácter anterior
    LOOP .vleft_loop
.vleft_done:
    RET

;se imprime ya como tal los datos en la pantalla(es lo mismo del boot)
print_init:
    PUSH AX
    PUSH BX
    CALL set_cursor
    MOV  AH, 0x0E
    XOR  BX, BX
    INT  0x10
    POP  BX
    POP  AX
    RET

print_string:
    PUSH AX
    PUSH BX
.loop:
    LODSB
    TEST AL, AL
    JZ   .done
    MOV  AH, 0x0E
    XOR  BX, BX
    INT  0x10
    JMP  .loop
.done:
    POP  BX
    POP  AX
    RET

;longitud de string en SI
strlen:
    PUSH SI
    XOR  CX, CX
.loop:
    LODSB
    TEST AL, AL
    JZ   .done
    INC  CX
    JMP  .loop
.done:
    POP  SI
    RET

;cosas por ahora "fijas"
rotation: DB ROT_NORMAL
name_row: DB 10
name_col: DB 10

str_name: DB "FabiJere", 0

TIMES (4*512)-($-$$) DB 0