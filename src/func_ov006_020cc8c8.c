
typedef short s16;
typedef struct { int x, y; } G2;
extern void _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(void *self, void *file, int a, int b, unsigned int u);
extern int data_ov006_0213b204[2];
extern G2 data_ov006_0213b21c;
extern void *data_ov006_021405c8[];
extern void *data_ov006_0214057c;
extern void *data_ov006_02140564;
extern int data_ov006_0214055c;

void func_ov006_020cc8c8(char *p) {
    int *cur;
    int *want;
    int z = 0;
    *(int *)(p + 0x40) = z;
    *(void **)(p + 0x20) = data_ov006_021405c8[*(s16 *)(p + 0x52)];
    *(int *)(p + 0x24) = z;
    *(int *)(p + 0x34) = z;
    *(int *)(p + 0x38) = z;
    *(int *)(p + 0x3c) = z;
    cur = (int *)(((long long)(int)(p + 0x64)) & 0xFFFFFFFFFFFFFFFFLL);
    want = data_ov006_0213b204;
    if (cur[0] == want[0] && (cur[1] == want[1] || *(int *)(p + 0x64) == 0)) {
        _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(p + 0x6c, data_ov006_0214057c, 0x40000000, 0x800, 0);
    } else {
        _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(p + 0x6c, data_ov006_02140564, 0, 0x800, 0);
    }
    {
        int *ctr = &data_ov006_0214055c;
        int c = *ctr;
        G2 s = data_ov006_0213b21c;
        *ctr = c + 1;
        *(G2 *)(p + 0x64) = s;
    }
}
