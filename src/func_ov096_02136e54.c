typedef int s32;
typedef unsigned short u16;
typedef signed short s16;

extern s16 data_02082214[];

void func_ov096_02136e54(void* c, int a)
{
    char* cc = (char*)c;
    s16* pang = (s16*)(((long long)(int)(cc + 0x35a)) & 0xFFFFFFFFFFFFFFFFLL);
    s16* pw = (s16*)(((long long)(int)(cc + 0x8e)) & 0xFFFFFFFFFFFFFFFFLL);

    u16 idx = *(u16*)(cc + 0x35a);
    s16 d = data_02082214[(idx >> 4) * 2 + 1];
    s32 r6 = (d + 0x1000) >> 1;
    s32 fp = (s32)(((long long)a * 0x666 + 0x800) >> 12);
    s32 t2 = (s32)(((long long)r6 * 0x4cc + 0x800) >> 12);
    s32 t3 = t2 + 0x1000;
    *(s32*)(cc + 0x80) = (s32)(((long long)t3 * fp + 0x800) >> 12);

    u16 idx2 = *(u16*)(cc + 0x35a);
    s16 d2 = data_02082214[(idx2 >> 4) * 2 + 1];
    s32 u = ((0x1000 - d2) >> 2) + 0x800;
    *(s32*)(cc + 0x84) = (s32)(((long long)u * fp + 0x800) >> 12);

    *(s32*)(cc + 0x88) = *(s32*)(cc + 0x80);
    *pang += 0x200;
    *(s32*)(cc + 0x98) = 0xe000;
    *pw += 0x2c00;
    *(s32*)(cc + 0xd8) = *(s32*)(cc + 0x80) * 0x514;
    *(s32*)(cc + 0xdc) = *(s32*)(cc + 0x84) * 0xfa0;
    *(s32*)(((long long)(int)(cc + 0x80)) & 0xFFFFFFFFFFFFFFFFLL) <<= 1;
    *(s32*)(((long long)(int)(cc + 0x84)) & 0xFFFFFFFFFFFFFFFFLL) <<= 2;
    *(s32*)(((long long)(int)(cc + 0x88)) & 0xFFFFFFFFFFFFFFFFLL) <<= 1;
}
