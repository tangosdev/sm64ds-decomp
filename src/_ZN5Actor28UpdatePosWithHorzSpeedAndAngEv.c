typedef short s16;
typedef int s32;
typedef long long s64;
extern s16 SINE_TABLE[];
void _ZN5Actor28UpdatePosWithHorzSpeedAndAngEv(void *c){
    s32 v98 = *(s32*)((char*)c+0x98);
    if (v98 == 0) {
        s32 a8 = *(s32*)((char*)c+0xa8);
        s32 v9c = *(s32*)((char*)c+0x9c);
        s32 a0 = *(s32*)((char*)c+0xa0);
        s32 nx = a8 + v9c;
        if (nx >= a0) a0 = nx;
        *(s32*)((char*)c+0xa4) = 0;
        *(s32*)((char*)c+0xa8) = a0;
        *(s32*)((char*)c+0xac) = 0;
        return;
    } else {
        s32 a8;
        s32 v9c;
        s32 a0;
        s64 px;
        s32 j;
        s16 sinv;
        s32 nx;
        s16 cosv;
        s64 pz;
        j = ((s32)*(unsigned short*)((char*)c+0x94) >> 4) << 1;
        sinv = SINE_TABLE[j + 1];
        a8 = *(s32*)((char*)c+0xa8);
        v9c = *(s32*)((char*)c+0x9c);
        a0 = *(s32*)((char*)c+0xa0);
        px = ((s64)v98 * sinv + 0x800) >> 12;
        nx = a8 + v9c;
        if (nx >= a0) a0 = nx;
        cosv = SINE_TABLE[j];
        pz = ((s64)v98 * cosv + 0x800) >> 12;
        *(s32*)((char*)c+0xa4) = (s32)pz;
        *(s32*)((char*)c+0xa8) = a0;
        *(s32*)((char*)c+0xac) = (s32)px;
        return;
    }
}
