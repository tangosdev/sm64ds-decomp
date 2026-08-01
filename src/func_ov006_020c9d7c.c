/* best near-miss div=2 @ mwccarm 1.2/sp2p3 */
extern int RandomIntInternal(int *seed);
extern void _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(void *thiz, void *bca, int a, int fix, unsigned int b);
extern void func_ov006_020ca2ec(void *c);
extern int data_0209e650;
extern int data_ov006_021405a4;
extern void *data_ov006_02140580;

void func_ov006_020c9d7c(char *c)
{
    unsigned int r;
    int r5;
    *(short*)(((long long)(int)(c + 0x6c))) -= 1;
    if (*(short*)(c + 0x6c) == 0) {
    *(int*)(c + 0x28) = 0x100000;
    r = ((unsigned int)RandomIntInternal(&data_0209e650) & 0x7fffffff) >> 0x13;
    *(int*)(c + 0x24) = ((int)r - 0x800) * 0xc0;
    *(int*)(c + 0x40) = 0;
    r5 = data_ov006_021405a4;
    r = ((unsigned int)RandomIntInternal(&data_0209e650) & 0x7fffffff) >> 0x13;
    *(int*)(c + 0x3c) = (int)(((long long)(((int)r - 0x800) << 1) * r5 + 0x800) >> 12);
    _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(c + 0x78, data_ov006_02140580, 0x40000000, 0x800, 0);
    *(int*)(c + 0x30) = *(int*)(c + 0x24);
    *(int*)(c + 0x34) = *(int*)(c + 0x28);
    *(int*)(c + 0x38) = *(int*)(c + 0x2c);
    func_ov006_020ca2ec(c);
    } else {
        *(int*)(c + 0x28) = 0x100000;
        *(int*)(c + 0x40) = 0;
    }
}
