struct E22 {
    char b[22];
};
extern unsigned char data_020a0f04;
extern struct E22 data_020a10fc[];
extern struct E22 data_020a10a4[];
/* daPly_c idx's 0x16-byte profile: from data_020a10fc in link mode 3,
   else from data_020a10a4 (four entries each; see func_0203db64). */
void *func_0203d8a0(int idx) {
    if (data_020a0f04 == 3)
        return &data_020a10fc[idx];
    return &data_020a10a4[idx];
}
