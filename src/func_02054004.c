typedef unsigned short u16;

extern u16 DisableVramBanks(u16*);
extern u16 data_020a608c;

u16 func_02054004(void) {
    return DisableVramBanks(&data_020a608c);
}
