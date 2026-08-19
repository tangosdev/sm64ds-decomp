// NONMATCHING: register colouring + one instruction-schedule swap (65 of 148 words
// differ under 2004/b56). Size is exact (0x250), the instruction count is exact, the
// literal pool and every relocation slot are exact, and the disassembly aligns 1:1 word
// for word: erasing register names leaves the two streams identical apart from a single
// two-load reorder (the [r4,r6,lsl#2] / [r8,lr] loads sit in swapped order). The first
// path (0x00..0x9c, 40 words) is byte-EXACT; the residue is entirely inside the second
// path, one systemic register rotation of the four long-lived pointers (target holds the
// velocity base c+idx*4+0x4000 in r2, the position base c+0x47f8 in r7, the state var in
// r4; the candidate rotates each to ip/r5/r2). Same mnemonics, same immediates, same
// memory offsets. Logic verified against the ROM instruction by instruction. Counts as
// decompiled, not matched.
//
// dScMgLuigi_c ("Wanted!" minigame) per-character physics update. Reached only through a
// function-pointer word at data_ov006_0213cd8c, one call per on-screen character (idx).
// counterA[idx] (self[0x5275+idx]) gates a one-shot launch: on the first tick it seeds the
// two velocity components (velX self[0x4bb8+idx*4], velY self[0x4d98+idx*4]) from the
// per-face tables data_ov006_0212e888/0212e898 (sign chosen by the face index cC =
// self[0x5365+idx]) and clears the bounce state self[0x4f7c+idx*2]. On every later tick it
// integrates position (posX self[0x47f8+idx*4] += velX, posY self[0x49d8+idx*4] += velY)
// and runs a three-state spring on velX using data_ov006_0212e8a8[cC] as the per-step
// acceleration and data_ov006_0212e888[cC] as the amplitude limit, flipping the state at
// each turning point, then hands off to func_ov006_020f1dbc for the per-character finish.
extern void func_ov006_020f1dbc(void *self, int idx);
extern int data_ov006_0212e888[];
extern int data_ov006_0212e898[];
extern int data_ov006_0212e8a8[];

#define VBASE(c, idx) ((char *)(int)(((long long)(int)((c) + (idx) * 4 + 0x4000)) & 0xFFFFFFFFFFFFFFFFLL))

#pragma opt_common_subs off

void func_ov006_020f15ac(char *c, int idx)
{
    unsigned char *counterA = (unsigned char *)(c + 0x5275);
    unsigned char *pA = &counterA[idx];

    if (counterA[idx] == 0) {
        unsigned char *counterC = (unsigned char *)(c + 0x5365);
        unsigned char *pC;
        unsigned char b;
        *(unsigned short *)(c + idx * 2 + 0x4f7c) = 0;
        *pA += 1;
        pC = &counterC[idx];
        b = *pC;
        if (b == 0) {
            *(int *)(c + idx * 4 + 0x4bb8) = data_ov006_0212e888[b];
        } else {
            *(int *)(c + idx * 4 + 0x4bb8) = -data_ov006_0212e888[b];
        }
        *(int *)(c + idx * 4 + 0x4d98) = data_ov006_0212e898[*pC];
        return;
    }

    {
        unsigned int state;
        int *accB = (int *)(c + 0x47f8);
        int *accD = (int *)(c + 0x49d8);
        char *sbase;
        int n;

        {
            int t = accB[idx];
            accB[idx] = t + *(int *)(VBASE(c, idx) + 0xbb8);
        }
        {
            int t = accD[idx];
            accD[idx] = t + *(int *)(VBASE(c, idx) + 0xd98);
        }
        sbase = c + idx * 2 + 0x4f00;
        state = *(unsigned short *)(sbase + 0x7c);
        n = idx * 4;
        if (state == 0) {
            if (*(int *)(VBASE(c, idx) + 0xbb8) > 0) {
                *(int *)(c + 0x4bb8 + n) -= data_ov006_0212e8a8[*(unsigned char *)(c + idx + 0x5365)];
                if (*(int *)(VBASE(c, idx) + 0xbb8) <= 0) {
                    *(int *)(VBASE(c, idx) + 0xbb8) = 0;
                    *(unsigned short *)(sbase + 0x7c) = 2;
                }
            } else if (*(int *)(VBASE(c, idx) + 0xbb8) < 0) {
                *(int *)(c + 0x4bb8 + n) += data_ov006_0212e8a8[*(unsigned char *)(c + idx + 0x5365)];
                if (*(int *)(VBASE(c, idx) + 0xbb8) >= 0) {
                    *(int *)(VBASE(c, idx) + 0xbb8) = 0;
                    *(unsigned short *)(sbase + 0x7c) = 1;
                }
            }
        } else if (state == 1) {
            unsigned char *cC = (unsigned char *)(c + 0x5365);
            *(int *)(c + 0x4bb8 + n) += data_ov006_0212e8a8[cC[idx]];
            if (*(int *)(VBASE(c, idx) + 0xbb8) >= data_ov006_0212e888[cC[idx]]) {
                *(unsigned short *)(sbase + 0x7c) = 0;
            }
        } else {
            unsigned char *cC = (unsigned char *)(c + 0x5365);
            *(int *)(c + 0x4bb8 + n) -= data_ov006_0212e8a8[cC[idx]];
            if (*(int *)(VBASE(c, idx) + 0xbb8) <= -data_ov006_0212e888[cC[idx]]) {
                *(unsigned short *)(sbase + 0x7c) = 0;
            }
        }
    }

    func_ov006_020f1dbc(c, idx);
}
