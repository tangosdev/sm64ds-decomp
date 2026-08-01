/* func_ov063_02116190 at 0x02116190
 *
 * Matched byte-for-byte with mwccarm 1.2/sp2p3 (ov063).
 */
extern void* _ZN5Actor13ClosestPlayerEv(void* self);
extern int Vec3_HorzDist(const void* a, const void* b);

struct Flags { unsigned short bit0 : 1; };

int func_ov063_02116190(char* self)
{
    int vec[3];
    void* p;

    if (((struct Flags*)(self + 0x5d4))->bit0) {
        p = _ZN5Actor13ClosestPlayerEv(self);
        if (p != 0 && *(int*)((char*)p + 0x64) >= 0x3e8000)
            return 1;
    } else if (*(int*)(self + 0x60) <= (int)0xff768000) {
        p = _ZN5Actor13ClosestPlayerEv(self);
        vec[0] = -0xbe000;
        vec[1] = (int)0xff66e000;
        vec[2] = 0xbe000;
        if (Vec3_HorzDist(vec, (char*)p + 0x5c) >= 0x73a000)
            return 1;
    }
    return 0;
}