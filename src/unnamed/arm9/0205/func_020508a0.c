typedef int s32;
extern void func_02052438(s32 *p);
extern int func_02052420(s32 *p);
extern void func_020502b8(void);
extern int func_02052458(s32 *p);
extern void func_0205aa68(void *a, int b, int c);

struct G {
    s32 f0;       /* 0x0 */
    s32 f4;       /* 0x4 */
    char pad8[0x30];
    s32 f38;      /* 0x38 */
    char pad3c[0x20 - 0x3c + 0x38]; /* placeholder, fix below */
};

extern char data_020a5634[];

void func_020508a0(void)
{
    char *g = data_020a5634;
    s32 *sub;
    int v;
    if (*(s32 *)(g + 0) == 0) return;
    if (*(s32 *)(g + 4) != 0) return;
    sub = (s32 *)(g + 0x38);
    func_02052438(sub);
    if (*(s32 *)(g + 0x48) != 0) {
        if (func_02052420(sub) != 0) {
            func_020502b8();
            return;
        }
    }
    v = func_02052458(sub) >> 8;
    if (v == *(s32 *)(g + 0x4c)) return;
    func_0205aa68(*(void **)(g + 0x20), v, 0);
    *(s32 *)(g + 0x4c) = v;
}
