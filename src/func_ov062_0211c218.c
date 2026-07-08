extern int _ZN14BlendModelAnim7SetAnimER8BCA_Fileii5Fix12IiEt(void *anim, void *file, int a, int b, int c, unsigned short u);
extern int RandomIntInternal(int *seed);
struct G { int w[2]; };
extern struct G data_ov062_0211e104;
extern int RNG_STATE;
extern signed char LEVEL_ID;

int func_ov062_0211c218(char *c)
{
    *(short *)(c + 0x44a) = *(short *)(c + 0x8e);
    *(int *)(c + 0x43c) = 0;
    *(short *)(c + 0x100) = (((unsigned int)RandomIntInternal(&RNG_STATE) >> 8) & 3) * 0x64 + 0x12c;
    if (*(unsigned char *)(c + 0x447) == 1) {
        *(short *)(c + 0x100) = 0;
        *(unsigned char *)(c + 0x447) = 0;
    }
    *(int *)(c + 0x440) = 0;
    _ZN14BlendModelAnim7SetAnimER8BCA_Fileii5Fix12IiEt(c + 0x334, (void *)data_ov062_0211e104.w[1], 4, 0, 0x1000, 0);
    if (*(unsigned char *)(c + 0x448) == 2 ||
        (unsigned char)(signed char)((signed char)LEVEL_ID - 0x18) <= 1) {
        *(int *)(c + 0x440) = 2;
        *(short *)(c + 0x100) = 0;
    }
    return 1;
}
