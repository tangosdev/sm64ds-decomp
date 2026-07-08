typedef unsigned char u8;
typedef unsigned short u16;
typedef short s16;
typedef int s32;
typedef unsigned int u32;

extern s16 ReadUnalignedShort(u8* p);
extern int AngleDiff(int a, int b);
extern void _ZN6Player7SetAnimEji5Fix12IiEj(void* thiz, int a, int b, int c, u32 d);
extern int ApproachAngle(s16* angle, int from, int start, int speed, int max);

extern u16 KS_FRAME_COUNTER;

int func_ov002_020bcd38(char* player, u8* p, int a2, int a3)
{
    s16 cur;

    if (a2 == KS_FRAME_COUNTER) {
        int dt = a3 - a2;
        *(s16*)(player + 0x69c) = (short)AngleDiff(*(s16*)(player + 0x8e), ReadUnalignedShort(p)) / dt;
        *(u8*)(player + 0x6e5) = (u8)dt;
        *(u8*)(player + 0x727) = 0;
        *(u8*)(player + 0x728) = 0;
        _ZN6Player7SetAnimEji5Fix12IiEj(player, 0x48, 0, 0x1000, 0);
    }

    cur = *(s16*)(player + 0x69c);
    ApproachAngle((s16*)(player + 0x8e), ReadUnalignedShort(p), *(u8*)(player + 0x6e5), cur, cur);
    *(s16*)(player + 0x94) = *(s16*)(player + 0x8e);
    return 1;
}
