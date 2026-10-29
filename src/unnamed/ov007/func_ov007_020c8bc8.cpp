//cpp
typedef long long s64;

/* The 0x34-byte screen transition record (data_ov007_02104bd8 slot 0);
 * names as in func_ov007_020c8ac0 and func_ov007_020c8498, only the slots
 * this file reads. From +0x2c on it holds the effect's arguments; these are
 * the blend-alpha ones (effects 2 and 3). */
struct Ov007Transition {
    int effect;           /* 0x00: 1 mosaic, 2/3 blend alpha, 4 brightness */
    unsigned int screens; /* 0x04: bit 0 main engine, bit 1 sub */
    char pad_08[0x28];
    unsigned char evaFrom; /* 0x30: eased to evaTo */
    unsigned char evbFrom; /* 0x31: eased to evbTo */
    unsigned char evaTo;   /* 0x32 */
    unsigned char evbTo;   /* 0x33 */
};

typedef char Ov007Transition_size_must_be_0x34[sizeof(Ov007Transition) == 0x34 ? 1 : -1];

/* Eases EVA and EVB by the Q12 progress t and writes BLDALPHA on the chosen
 * screens; effect 3 makes EVB the complement of EVA. r0 keeps the tree's
 * char * spelling; `tr` is the typed view. */
extern "C" void func_ov007_020c8bc8(char *r0, int t) {
    Ov007Transition *tr = (Ov007Transition *)r0;
    unsigned char c1 = tr->evaTo;
    unsigned char c0 = tr->evaFrom;
    int v2 = c1;
    if (t < 0x1000) {
        if (t <= 0)
            v2 = c0;
        else
            v2 = (int)(((s64)(0x1000 - t) * (c0 << 12) + (s64)t * (c1 << 12)) >> 12) >> 12;
    }
    {
        unsigned char d1 = tr->evbTo;
        unsigned char d0 = tr->evbFrom;
        int vip = d1;
        if (t < 0x1000) {
            if (t <= 0)
                vip = d0;
            else
                vip = (int)(((s64)(0x1000 - t) * (d0 << 12) + (s64)t * (d1 << 12)) >> 12) >> 12;
        }
        if (tr->effect == 3)
            vip = 0x10 - v2;
        if (tr->screens & 1)
            *(volatile unsigned short *)0x4000052 = (unsigned short)(v2 | (vip << 8));
        if (tr->screens & 2)
            *(volatile unsigned short *)0x4001052 = (unsigned short)(v2 | (vip << 8));
    }
}
