/* storage 0x02385578 runtime 0x037FD410 linked 0x224 wram ARM. SND sequence
 * voice note-on (TrackPlayNote): tie-flag head check, inst-type switch
 * 1/4->0xFFFF 2->0x3F00 3->0xC000, length divide via _s32_div_f (/),
 * SND_AllocExChannel(..., ChannelCallback, track), SND_NoteOn, list
 * head-insert, envAttack/Decay/Sustain/Release 0xFF guards, portamento sweep.
 * Needs #pragma opt_propagation off for the prologue coloring; the k/t/ha/hb
 * head block (240*len5 split out, MUL/>>8 split around k) fuses into the
 * ROM's MLA with the MUL landing in R1. Pool: 0xFFFF + fn_023858AC. */

extern int fn_02386D18(int a1, int a2, int a3, void *a4);
extern int fn_02384AD4(int a1, int a2, int a3, void *a4, void *a5);
extern int fn_02386C10(int a1, int a2, int a3, int a4, int a5, void *a6);
extern int fn_023843C0(int *a1);
extern int fn_02384564(int result, int a2);
extern int fn_02384548(int a1, int a2);
extern int fn_02384540(int result, int a2);
extern int fn_02384524(int *a1, int a2);
extern int fn_023858AC(int a1);

#define BIT(x, s) (((unsigned int)(x) << (s)) >> 31)

struct Chn {
    char f0[8];
    unsigned char midiKey;
    unsigned char velocity;
    char fA[10];
    int sweepCounter;
    int sweepLength;
    char f1C[6];
    unsigned char priority;
    char f23[15];
    short sweepPitch;
    char f34[28];
    int next;
};

#pragma opt_propagation off
int fn_02385578(unsigned char *track, int player, int midiKey, unsigned char velocity, int length) {
    struct Chn *chn = 0;
    int v10;
    int v11;
    int k;
    int t;
    unsigned short ha;
    unsigned short hb;
    int result;
    int allowedChannels;
    int len;
    unsigned char inst[12];
    {
        int len5 = length;
        ha = *(unsigned short *)(player + 24);
        hb = *(unsigned short *)(player + 26);
        t = ha * hb;
        k = 240 * len5;
        v10 = t >> 8;
        v11 = (k + (v10 - 1)) / v10;
    }
    if (BIT(track[0], 28) != 0) {
        chn = *(struct Chn **)(track + 60);
        if (chn != 0) {
            chn->midiKey = (unsigned char)midiKey;
            chn->velocity = velocity;
        }
    }
    if (chn == 0) {
        result = fn_02386D18(*(int *)(player + 32), *(unsigned short *)(track + 2), midiKey, inst);
        if (result == 0)
            return result;
        result = inst[0];
        switch (result) {
        case 1:
        case 4:
            allowedChannels = 0xFFFF;
            break;
        case 2:
            allowedChannels = 0x3F00;
            break;
        case 3:
            allowedChannels = 0xC000;
            break;
        default:
            return result;
        }
        allowedChannels &= *(unsigned short *)(track + 30);
        result = fn_02384AD4(allowedChannels,
                             *(unsigned char *)(player + 4) + track[18],
                             BIT(track[0], 24),
                             (void *)fn_023858AC, track);
        chn = (struct Chn *)result;
        if (result == 0)
            return result;
        if (BIT(track[0], 28) != 0)
            len = 0;
        else
            len = v11;
        if (fn_02386C10((int)chn, midiKey, velocity, len, *(int *)(player + 32), inst) == 0) {
            chn->priority = 0;
            return fn_023843C0((int *)chn);
        }
        chn->next = *(int *)(track + 60);
        *(int *)(track + 60) = (int)chn;
    }
    if (track[14] != 0xFF)
        fn_02384564((int)chn, track[14]);
    if (track[15] != 0xFF)
        fn_02384548((int)chn, track[15]);
    if (track[16] != 0xFF)
        fn_02384540((int)chn, track[16]);
    if (track[17] != 0xFF)
        fn_02384524((int *)chn, track[17]);
    chn->sweepPitch = *(short *)(track + 22);
    if (BIT(track[0], 26) != 0)
        chn->sweepPitch += (short)((track[20] - midiKey) << 6);
    if (track[21] == 0) {
        chn->sweepLength = v11;
    } else {
        int swp = track[21] * track[21];
        int v19 = chn->sweepPitch;
        if (v19 < 0)
            v19 = -v19;
        swp *= v19;
        chn->sweepLength = swp >> 11;
    }
    chn->sweepCounter = 0;
    return 0;
}
