typedef unsigned short u16;

extern u16 DisableVramBanks(u16*);
extern u16 data_020a609a;

u16 func_02053f44(void) {
    return DisableVramBanks(&data_020a609a);
}
