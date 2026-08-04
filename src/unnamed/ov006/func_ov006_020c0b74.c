extern int _ZN9Animation8FinishedEv(char *self);
extern int _ZNK9Animation12WillHitFrameEi(char *self, int frame);
extern void _ZN14BlendModelAnim7SetAnimER8BCA_Fileii5Fix12IiEt(char *self, void *file, int a, int b, int c, unsigned short d);
extern void func_ov006_020c1764(char *p);

typedef struct {
    char _pad[0x1d8];
    short field_1d8;
} Obj;

void func_ov006_020c0b74(char *p) {
    if (*(void **)(p + 0x7c) == *(void **)(p + 0x244) && _ZN9Animation8FinishedEv(p + 0x6c)) {
        _ZN14BlendModelAnim7SetAnimER8BCA_Fileii5Fix12IiEt(p + 0x1c, *(void **)(p + 0x24c), 0, 0, 0x800, 0);
        return;
    }
    if (*(void **)(p + 0x7c) == *(void **)(p + 0x24c) && _ZNK9Animation12WillHitFrameEi(p + 0x6c, 0)) {
        *(short *)((long long)(int)(p + 0x1d8)) -= 1;
        if (((Obj *)p)->field_1d8 == 0) {
            _ZN14BlendModelAnim7SetAnimER8BCA_Fileii5Fix12IiEt(p + 0x1c, *(void **)(p + 0x254), 0, 0x40000000, 0x800, 0);
            return;
        }
    }
    if (*(void **)(p + 0x7c) == *(void **)(p + 0x254) && _ZN9Animation8FinishedEv(p + 0x6c)) {
        func_ov006_020c1764(p);
    }
}
