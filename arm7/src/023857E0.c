/* storage 0x023857E0 runtime 0x037FD678 linked 0xCC wram ARM. SND track-channel
 * update (TrackUpdateChannel): 5-term decibel sum, clamped pan, level pitch.
 * Sum order table4+table5+tableA2+shortA+shortA2; level is SDK 4-statement
 * pitch (v5=s6; *=u7<<6; >>=7; +=sC); w18/1A/1C are unsigned (LDRH). */

extern short dat_3805860[];
struct Chn {
    char _0[0xB];
    signed char pan;
    short vol;
    short level;
    char _10[0x28-0x10];
    short a, b, c;
    char _2E[0x50-0x2E];
    struct Chn *next;
};
struct Trk {
    char _0[4];
    unsigned char i4, i5;
    signed char s6;
    unsigned char u7;
    signed char p8, p9;
    short sA;
    short sC;
    char _0E[0x18-0x0E];
    unsigned short w18, w1A, w1C;
    char _1E[0x3C-0x1E];
    struct Chn *head;
};
struct A2 {
    char _0[5];
    unsigned char i5;
    short s6;
};

int fn_023857E0(struct Trk *result, struct A2 *a2) {
    struct Chn *v2;
    int v3;
    int v4;
    int v5;

    v2 = result->head;
    v3 = dat_3805860[result->i4] + dat_3805860[result->i5] + dat_3805860[a2->i5] + result->sA + a2->s6;
    v4 = result->p8 + result->p9;
    v5 = result->s6; v5 *= result->u7 << 6; v5 >>= 7; v5 += result->sC;
    if (v4 < -128) {
        v4 = -128;
    } else if (v4 > 127) {
        v4 = 127;
    }
    while (v2 != 0) {
        v2->vol = (short)v3;
        v2->level = (short)v5;
        v2->pan = (signed char)v4;
        v2->a = result->w18;
        v2->b = result->w1A;
        v2->c = result->w1C;
        v2 = v2->next;
    }
    return (int)result;
}
