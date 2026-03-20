typedef unsigned char  u8;
typedef unsigned int   u32;
typedef unsigned long  u64;

/* ---------- Boot info ---------- */

typedef struct {
    void* framebuffer;
    u32 width;
    u32 height;
    u32 pitch;
} BootInfo;

static BootInfo* boot;

/* ---------- keys ---------- */

#define KEY_UP     0x48
#define KEY_DOWN   0x50
#define KEY_LEFT   0x4B
#define KEY_RIGHT  0x4D
#define KEY_ESC    0x01
#define KEY_ENTER  0x1C

#define ROT_NORMAL 0
#define ROT_FLIP   1
#define ROT_LEFT   2
#define ROT_RIGHT  3

#define MIN_ROW 5
#define MIN_COL 5

/* ---------- estado del juego ---------- */

static u8 rotation = ROT_NORMAL;
static u8 name_row = 10;
static u8 name_col = 10;

static const char str_name[] = "FabiJere";

/* ==========================================
   FRAMEBUFFER
   ========================================== */

static inline void put_pixel(u32 x, u32 y, u32 color)
{
    u32* fb = (u32*)boot->framebuffer;
    fb[y * (boot->pitch / 4) + x] = color;
}

void clear_screen()
{
    for (u32 y = 0; y < boot->height; y++)
        for (u32 x = 0; x < boot->width; x++)
            put_pixel(x, y, 0x00000000);
}

/* bloque simple = caracter */
void draw_block(u32 px, u32 py)
{
    for (int y = 0; y < 8; y++)
        for (int x = 0; x < 8; x++)
            put_pixel(px + x, py + y, 0x00FFFFFF);
}

/* ==========================================
   UTILIDADES (equivalentes ASM)
   ========================================== */

u32 strlen_simple(const char* s)
{
    u32 n = 0;
    while (*s++) n++;
    return n;
}

/* imprime caracter en posicion texto */
void print_char(u8 col, u8 row)
{
    /* 8x8 font fake */
    draw_block(col * 10, row * 10);
}

void print_string(u8 col, u8 row, const char* s)
{
    while (*s)
    {
        print_char(col, row);
        col++;
        s++;
    }
}

/* ==========================================
   DIBUJO DEL JUEGO
   ========================================== */

void draw_normal()
{
    print_string(name_col, name_row, str_name);
}

void draw_flip()
{
    u32 len = strlen_simple(str_name);

    for (u32 i = 0; i < len; i++)
    {
        print_char(name_col + i,
                   name_row);
    }
}

void draw_vright()
{
    const char* s = str_name;
    u8 row = name_row;

    while (*s && row < 24)
    {
        print_char(name_col, row);
        row++;
        s++;
    }
}

void draw_vleft()
{
    u32 len = strlen_simple(str_name);

    u8 row = name_row;

    for (int i = len - 1; i >= 0 && row < 24; i--)
    {
        print_char(name_col, row);
        row++;
    }
}

void draw_game()
{
    clear_screen();

    switch (rotation)
    {
        case ROT_NORMAL: draw_normal(); break;
        case ROT_FLIP:   draw_flip();   break;
        case ROT_LEFT:   draw_vleft();  break;
        default:         draw_vright(); break;
    }
}

/* ==========================================
   INPUT (lo llena el bootloader)
   ========================================== */

typedef struct {
    u8 scancode;
    u8 ascii;
} KeyEvent;

/* ESTA FUNCION debe implementarla el bootloader */
extern int get_key(KeyEvent* key);

/* ==========================================
   LOOP PRINCIPAL (igual al ASM)
   ========================================== */

void game_loop()
{
    KeyEvent key;

    /* esperar ENTER */
    while (1)
    {
        if (get_key(&key))
            if (key.scancode == KEY_ENTER)
                break;
    }

    draw_game();

    while (1)
    {
        if (!get_key(&key))
            continue;

        if (key.scancode == KEY_ESC)
            return;

        if (key.ascii == 'r' || key.ascii == 'R')
        {
            rotation = ROT_NORMAL;
            draw_game();
            continue;
        }

        switch (key.scancode)
        {
            case KEY_UP:
                rotation = ROT_NORMAL; break;

            case KEY_DOWN:
                rotation = ROT_FLIP; break;

            case KEY_LEFT:
                rotation = ROT_LEFT; break;

            case KEY_RIGHT:
                rotation = ROT_RIGHT; break;

            default:
                continue;
        }

        draw_game();
    }
}

/* ==========================================
   ENTRYPOINT DEL KERNEL
   ========================================== */

void kernel_main(BootInfo* info)
{
    boot = info;

    draw_game();
    game_loop();

    for (;;)
        __asm__("hlt");
}