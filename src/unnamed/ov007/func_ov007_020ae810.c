/* The 10-byte layout row func_ov007_020aea6c's item points at; names as in
 * func_ov007_020b5bf0, only the slots this file reads. */
typedef struct Ov007ItemLayout {
    unsigned short kind; /* 0x0 */
    short homeX;         /* 0x2 */
    short homeY;         /* 0x4 */
} Ov007ItemLayout;

/* The 0x2c-byte record func_ov007_020aea6c allocates; names as in
 * func_ov007_020b5f64, only the slots this file touches. */
typedef struct Ov007Item {
    char pad_00[8];
    int posX; /* 0x08: Q12 */
    int posY; /* 0x0c */
    char pad_10[0x14];
    Ov007ItemLayout *layout; /* 0x24 */
} Ov007Item;

typedef char Ov007Item_layout_must_be_at_0x24[(int)&((Ov007Item *)0)->layout == 0x24 ? 1 : -1];

/* Puts the item at its layout row's home position. c keeps the char *
 * spelling of the callers; `item` is the typed view. */
void func_ov007_020ae810(char* c) {
    Ov007Item *item = (Ov007Item *)c;
    item->posX = (int)item->layout->homeX << 12;
    item->posY = (int)item->layout->homeY << 12;
}
