typedef unsigned short u16;

extern u16 DisableVramBanks(u16*);
extern u16 data_020a6090;

u16 func_02053fa8(void) {
    return DisableVramBanks(&data_020a6090);
}
