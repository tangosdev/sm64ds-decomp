extern void FreeGfxSlotsById(int);
typedef struct { char pad[0x1a]; short field_1a; char pad2[4]; } Elem;
extern Elem data_ov004_020bebe8[];
extern char data_ov004_020bf3e8[];

void func_ov004_020b0d30(void) {
    FreeGfxSlotsById(0x1d);
    int i = 0;
    do {
        data_ov004_020bebe8[i].field_1a = 0;
        i++;
    } while (i < 0x40);
    int j = 0;
    char* p = data_ov004_020bf3e8;
    do {
        j++;
        *p++ = 0;
    } while (j < 3);
}
