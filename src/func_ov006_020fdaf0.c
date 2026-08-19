// NONMATCHING: size 0x23c vs target 0x250 (div=codegen). The whole body is present:
// every field load/store uses the ROM's struct offset and all three calls (cstd sqrt,
// func_02012718, func_ov006_020fb8fc) are reproduced with matching arguments. The
// residue is codegen only -- register/stack allocation, mwcc strength-reducing the
// *0x38 index math, and the literal pool landing closer so the pc-relative loads
// differ in displacement. A
// size-exact variant that matches to 7 words exists (banked in nearmiss/db.jsonl) but
// only via int-cast pointer laundering that truncates 64-bit host pointers, so it is
// unfit for the native port; this host-safe form is used instead.
// Logic verified instruction-by-instruction vs the ROM disassembly.
// dScMgPachinko_c (Bob-omb Squad, scene 368): a fired bob-omb i scans the 0x30 target
// slots and, on the first live slot (state byte >= 2) within sqrt-distance 0x18, seeds
// its homing record from the sin/cos table (data_02082214) and the per-slot heading,
// spawns the effect (func_02012718 / func_ov006_020fb8fc), bumps the slot's hit count
// and returns.
typedef short s16;
typedef unsigned short u16;
typedef unsigned char u8;

extern int _ZN4cstd4sqrtEy(unsigned long long v);
extern void func_02012718(void *model, int arg);
extern void func_ov006_020fb8fc(char *o, int a2, int a3, int a4, int a5, int a6);
extern const s16 data_02082214[];
extern void *data_ov006_0212eb94[];
extern const u16 data_ov006_0213d954[];

void func_ov006_020fdaf0(char *o, int i)
{
    int n = i * 0x38;
    int j;
    char *slot = o;

    for (j = 0; j < 0x30; j++, slot += 0x38)
    {
        char *e = slot + 0x4000;
        int dx, dz;
        unsigned long long distsq;

        if (*(u8 *)(e + 0xf0c) == 0)
            continue;
        if (*(u8 *)(e + 0xf0d) < 2)
            continue;

        dz = (*(int *)(o + n + 0x4664) - *(int *)(e + 0xedc)) >> 12;
        distsq = (unsigned long long)((long long)dz * dz);
        dx = (*(int *)(o + n + 0x4660) - *(int *)(e + 0xed8)) >> 12;
        distsq += (unsigned long long)((long long)dx * dx);
        if (_ZN4cstd4sqrtEy(distsq) > 0x18)
            continue;

        {
            int m = j * 0x38;
            char *rec = o + n + 0x4000;
            u16 ang;
            s16 sn, cs;
            int vel;
            int cc;

            *(u8 *)(rec + 0x68f) = 5;
            ang = *(u16 *)(o + m + 0x4f08);
            *(u16 *)(o + n + 0x4684) = ang;

            ang = *(u16 *)(o + n + 0x4684);
            sn = data_02082214[((ang >> 4) << 1) + 1];
            *(int *)(rec + 0x668) = (int)(((long long)sn * 0x2000 + 0x800) >> 12);
            ang = *(u16 *)(o + n + 0x4684);
            cs = data_02082214[(ang >> 4) << 1];
            *(int *)(rec + 0x66c) = (int)(((long long)cs * 0x2000 + 0x800) >> 12);

            *(int *)(o + n + 0x4678) = 0;
            ang = *(u16 *)(o + n + 0x4684);
            vel = *(int *)(o + n + 0x4678);
            sn = data_02082214[((ang >> 4) << 1) + 1];
            *(int *)(rec + 0x670) = (int)(((long long)sn * vel + 0x800) >> 12);
            ang = *(u16 *)(o + n + 0x4684);
            vel = *(int *)(o + n + 0x4678);
            cs = data_02082214[(ang >> 4) << 1];
            *(int *)(rec + 0x674) = (int)(((long long)cs * vel + 0x800) >> 12);

            cc = *(u8 *)(o + m + 0x4f0f);
            if (cc >= 5)
                cc = 5;
            func_02012718(data_ov006_0212eb94[cc], *(int *)(o + n + 0x4660));
            func_ov006_020fb8fc(o, *(int *)(o + n + 0x4660), *(int *)(o + n + 0x4664),
                                2, (int)data_ov006_0213d954[cc], 0);
            (*(u8 *)(o + m + 0x4f0f))++;
            return;
        }
    }
}
