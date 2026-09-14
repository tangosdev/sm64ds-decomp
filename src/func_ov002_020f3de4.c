// @symbol func_ov002_020f3de4
/* recovered: cutscene objects (daDemo_c / CutsceneObject block): the facing angle of one 0x4c-byte object record via atan2. */
// NONMATCHING: div 44 of 198 words. mwccarm 2004/b56, --module ov002,
// @ 0x020f3de4 size 0x318. Residue class: ordering (parameter-home class).
// Draft from nearmiss/db.jsonl (stored divergence 44), landed 2026-09-14 under Tango's ruling that the functionally-equivalent C
// drafts live on main with an honest banner so the port and readers have source. Logic is
// verified only as far as the residue class implies: register names and instruction order
// for the shape-exact rows, NOT an independent execution audit for the others. Counts as
// decompiled, not matched; tools/enroll.py leaves it out of the ROM build, which keeps the
// original bytes for this range. A byte-exact match replaces this file and drops the banner.
extern short _ZN4cstd5atan2E5Fix12IiES1_(int y, int x);
extern int func_ov002_020f5a94(void);
extern short data_02082214[];
extern unsigned short data_ov002_021000c0[];

void func_ov002_020f3de4(char *c, int i)
{
    int off = i * 0x4c;
    int idx, dx, dy;
    unsigned short ang;
    unsigned short target;
    unsigned short v;
    int j;

    v = *(unsigned short *)(c + 0x30 + off);
    if (v != 0) {
        *(unsigned short *)(c + 0x30 + off) = v - 1;
        return;
    }

    if (*(unsigned char *)(c + 0x48 + off) == 0) {
        idx = *(unsigned short *)(c + 0x2e + off) >> 4;
        {
            short tv = data_02082214[idx * 2 + 1];
            int spd = *(int *)(c + 8 + off);
            *(int *)(c + 0 + off) += (int)(((long long)tv * spd + 0x800) >> 12);
        }

        idx = *(unsigned short *)(c + 0x2e + off) >> 4;
        {
            short tv = data_02082214[idx * 2];
            int spd = *(int *)(c + 8 + off);
            *(int *)(c + 4 + off) += (int)(((long long)tv * spd + 0x800) >> 12);
        }

        if (*(int *)(c + 8 + off) >= 0x2000)
            *(int *)(c + 8 + off) -= 0x120;

        dx = (*(int *)(c + 0x1c + off) - *(int *)(c + 0 + off)) >> 12;
        dy = (*(int *)(c + 0x20 + off) - *(int *)(c + 4 + off)) >> 12;

        if (dx >= -2 && dx <= 2 && dy >= -2 && dy <= 2) {
            *(unsigned char *)(c + 0x48 + off) += 1;
            *(int *)(c + 0x24 + off) = 0;
            *(unsigned short *)(c + 0x2e + off) = 0xc000;
            return;
        }

        target = _ZN4cstd5atan2E5Fix12IiES1_(dy, dx);
        ang = *(unsigned short *)(c + 0x2e + off);
        if (target > ang) {
            *(unsigned short *)(c + 0x2e + off) += 0x100;
            if (target <= *(unsigned short *)(c + 0x2e + off))
                *(unsigned short *)(c + 0x2e + off) = target;
        } else {
            if (ang <= target)
                return;
            *(unsigned short *)(c + 0x2e + off) -= 0x100;
            if (*(unsigned short *)(c + 0x2e + off) <= target)
                *(unsigned short *)(c + 0x2e + off) = target;
        }
        return;
    }

    {
        short *tbl = (short *)(int)(((long long)(int)data_02082214) & 0xFFFFFFFFFFFFFFFFLL);
        idx = *(unsigned short *)(c + 0x2e + off) >> 4;
        {
            short tv = tbl[idx * 2 + 1];
            int spd = *(int *)(c + 0x10 + off);
            *(int *)(c + 0 + off) = (int)(((long long)tv * spd + 0x800) >> 12) + 0x80000;
        }
        idx = *(unsigned short *)(c + 0x2e + off) >> 4;
        {
            short tv = tbl[idx * 2];
            int spd = *(int *)(c + 0x10 + off);
            *(int *)(c + 4 + off) = (int)(((long long)tv * spd + 0x800) >> 12) + 0x60000;
        }
    }
    *(unsigned short *)(c + 0x2e + off) += *(unsigned short *)(c + 0x42 + off);
    *(int *)(c + 0x24 + off) += *(unsigned short *)(c + 0x42 + off);

    if (i != func_ov002_020f5a94() - 1)
        return;
    if ((unsigned int)*(int *)(c + 0x24) < 0x10000)
        return;

    for (j = 0; j < 4; j++) {
        if (*(unsigned char *)(c + 0x44) != 0) {
            *(unsigned char *)(c + 0x47) = 1;
            *(unsigned short *)(c + 0x3c) = 0;
            *(unsigned char *)(c + 0x48) = 0;
            *(int *)(c + 0x10) = 0x38000;
            *(unsigned short *)(c + 0x2e) = data_ov002_021000c0[j];
            *(int *)(c + 0x24) = 0;
            *(unsigned short *)(c + 0x42) = 0x200;
        }
        c += 0x4c;
    }
}
