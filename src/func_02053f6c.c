typedef unsigned short u16;

extern u16 DisableVramBanks(u16*);
extern u16 data_020a608e;

u16 func_02053f6c(void) {
    return DisableVramBanks(&data_020a608e);
}
