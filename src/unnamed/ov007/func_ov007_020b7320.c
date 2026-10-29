extern int func_ov007_020c3d40(int, int);

/* The manager's +0x74: the 0x48-byte 2D display record func_ov007_020b7594
 * allocates; characters and palettes as in func_ov007_020b6d40, only the
 * slots this file writes. Each table's count sits in the word before it. */
typedef struct Ov007Display {
    char pad_00[0x20];
    int characterCount; /* 0x20 */
    /* 0x24: func_ov007_020c99d8 character records; func_ov007_020c33d0
     * loads one into OBJ VRAM */
    void **characters;
    int paletteCount; /* 0x28 */
    void **palettes;  /* 0x2c: func_ov007_020c9988 palettes */
} Ov007Display;

typedef char
    Ov007Display_palettes_must_be_at_0x2c[(int)&((Ov007Display *)0)->palettes == 0x2c ? 1 : -1];

/* Sizes the character and palette tables (func_ov007_020b6d40 asks for 4
 * and 6). c keeps the tree's char * spelling; `d` is the typed view. */
void func_ov007_020b7320(char *c, int a, int b) {
    Ov007Display *d = (Ov007Display *)c;
    d->characterCount = a;
    d->paletteCount = b;
    d->characters = (void **)func_ov007_020c3d40(0, a << 2);
    d->palettes = (void **)func_ov007_020c3d40(0, b << 2);
}
