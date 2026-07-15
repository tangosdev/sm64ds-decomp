/* use unsigned loads / different shift form (>>8) cast s16 */
typedef unsigned int u32;
typedef unsigned short u16;
typedef short s16;

extern void MulMat4x3Mat4x3(const void* m1, const void* m0, void* mF);
extern void func_020553a4(void* m);
extern void func_ov080_02125460(void* self);
extern int data_0209b3ec;

void func_ov080_02125fd0(char* self)
{
    int tmp[13];
    int i;
    int* p;

    *(int*)0x4000444 = 0;
    MulMat4x3Mat4x3((void*)(self + 0xd4), &data_0209b3ec, tmp);
    *(int*)0x4000440 = 2;
    func_020553a4(tmp);
    *(int*)0x40004c8 = 0xe0000000;
    *(int*)0x40004cc = 0xc0007fff;
    func_ov080_02125460(self);

    *(int*)0x400046c = 0x20000;
    *(int*)0x400046c = 0x20000;
    *(int*)0x400046c = 0x20000;
    *(int*)0x4000500 = 1;

    i = 0;
    p = (int*)(self + 0x140);
    do {
        int v0, v1, v2;
        s16 s0, s1, s2;
        *(int*)0x4000488 = p[5];
        *(int*)0x4000484 = p[4];
        v0 = p[0];
        v1 = p[1];
        v2 = p[2];
        s0 = (s16)(v0 >> 8);
        s1 = (s16)(v1 >> 8);
        s2 = (s16)(v2 >> 8);
        *(int*)0x400048c = (u16)s0 | ((u16)s1 << 16);
        *(int*)0x400048c = (u16)s2;
        i++;
        p += 6;
    } while (i < 4);

    *(int*)0x4000504 = 0;
    *(int*)0x4000448 = 1;
}
