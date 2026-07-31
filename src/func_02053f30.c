typedef unsigned short u16;

extern u16 DisableVramBanks(u16*);
extern u16 data_020a609c;

u16 func_02053f30(void) {
    return DisableVramBanks(&data_020a609c);
}
