typedef short s16;
typedef int s32;

extern void func_020733a8(char *self, int a, int b, void *f1, void *f2);
extern int GetSoundMode(void);
extern void func_ov004_020b8dc0(char *self, int a, int b, s16 c, s16 e);
extern s32 GetGameLanguage(void);

extern void func_ov004_020b9280(void);
extern void func_ov004_020b9430(void);

extern short data_ov004_020bfe74[];
extern short data_ov004_020bfe88[];

#define SH(base,i) (*(short*)((char*)(base) + (i)*4))

char *func_ov004_020b92c4(char *c) {
    func_020733a8(c, 8, 0x24, (void *)func_ov004_020b9430, (void *)func_ov004_020b9280);

    func_ov004_020b8dc0(c + 0x48, 0, GetSoundMode(), 0xac, 0x38);
    func_ov004_020b8dc0(c, 2, 0, 0x64, 0x38);
    func_ov004_020b8dc0(c + 0x24, 3, 0, 0xf4, 0x38);
    func_ov004_020b8dc0(c + 0xd8, 4, 0, (s16)(SH(data_ov004_020bfe74, GetGameLanguage()) + 0x18), 0x38);
    func_ov004_020b8dc0(c + 0x6c, 1, 0, 0x8c, 0x68);
    func_ov004_020b8dc0(c + 0x90, 1, 0, 0xcc, 0x68);
    func_ov004_020b8dc0(c + 0xfc, 5, 0, (s16)(SH(data_ov004_020bfe88, GetGameLanguage()) + 0x18), 0x68);
    func_ov004_020b8dc0(c + 0xb4, 6, 0, 0x80, 0xa8);

    *(int *)(c + 0x120) = 8;
    *(int *)(c + 0x120) = 0;
    *(unsigned char *)(c + 0x124) = 0;

    return c;
}
