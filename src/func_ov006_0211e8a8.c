typedef unsigned char u8;
typedef short s16;

extern int data_ov006_0212efec[];

extern int* _ZN3G2S13GetBG0CharPtrEv(void);
extern void func_ov006_0211e55c(char* c, int idx);
extern void func_02012718(void* a, int b);

#define A(a) (*(u8*)(((long long)(int)(a)) & 0xFFFFFFFFFFFFFFFFLL))

void func_ov006_0211e8a8(char* c, int idx)
{
    int x;
    int y;
    int count;
    int firstOk;
    int i;
    int* r;
    int word;
    int off;

    if (*(u8*)(c + 0x467c + idx * 0x24) != 0)
        return;

    count = 0;
    firstOk = 0;
    for (i = 0; i < 5; i++) {
        x = (*(int*)(c + idx * 0x24 + 0x4660) >> 12) - data_ov006_0212efec[i * 2];
        y = (*(int*)(c + idx * 0x24 + 0x4664) >> 12) - data_ov006_0212efec[i * 2 + 1];
        r = ((int (*)[8])_ZN3G2S13GetBG0CharPtrEv())[(x >> 3) + ((y >> 3) << 5)];
        word = r[y & 7];
        if (((word >> ((x & 7) << 2)) & 0xf) == 0) {
            count++;
            if (i == 0)
                firstOk++;
        }
    }

    if (count < 2)
        return;
    if (firstOk == 0)
        return;

    *(u8*)(c + 0x467c + idx * 0x24) = 1;
    off = ((char (*)[0x24])0)[idx] - (char*)0;
    *(u8*)&((char (*)[0x24])c)[idx][0x467b] = 0;
    *(u8*)&((char (*)[0x24])c)[idx][0x4678] = 5;
    *(u8*)&((char (*)[0x24])c)[idx][0x4680] = *(u8*)(c + 0x4c21);
    *(s16*)&((char (*)[0x24])c)[idx][0x466e] = 0x40;
    A(c + 0x4c21)++;
    func_ov006_0211e55c(c, idx);
    func_02012718((void*)0x1f0, *(int*)(c + off + 0x4660));
    if (*(u8*)(c + 0x4c26) == 0xff)
        *(u8*)(c + 0x4c26) = (u8)idx;
}
