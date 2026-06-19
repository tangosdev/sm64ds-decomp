struct Obj;
extern struct Obj *data_ov004_020beb68;
extern int data_0209b308[];
extern int func_ov004_020adc1c(void);
extern void func_ov004_020adc00(int v);

void _ZN5Enemy20KillByInvincibleCharERK10Vector3_16R6Player(int thiz, int a1, int a2)
{
    char *g = (char*)data_ov004_020beb68;
    if (g == 0) return;
    g += 0x4000;
    *(int*)(g + 0x654) = thiz;
    if (data_0209b308[2] == 5) {
        if (thiz == 0) return;
        if (func_ov004_020adc1c() != 0) {
            if ((unsigned)thiz >= (unsigned)func_ov004_020adc1c()) return;
        }
        func_ov004_020adc00(thiz);
        *((char*)data_ov004_020beb68 + 0x4000 + 0x65c) = 1;
        return;
    }
    if ((unsigned)thiz <= (unsigned)func_ov004_020adc1c()) return;
    func_ov004_020adc00(thiz);
    *((char*)data_ov004_020beb68 + 0x4000 + 0x65c) = 1;
}
