// NONMATCHING (TERMINAL-FLOOR): functionally-verified C at the proven compiler floor.
// func_0202ffec @ 0x0202ffec (arm9, size 0x1d8). Quaternion product. 2 words diverge:
// two smull operand-slot swaps (+0x14, +0x50) -- mwccarm puts the second-emitted load in
// Rm, the ROM pins the matrix operand. ~800 spellings, the real permuter (6512 candidates),
// flags, versions, launders all closed (notes 6ay, DB row). Operand-order-only delta:
// functionally identical (multiplication is commutative).
typedef signed int s32;

struct Quat {
    s32 x;
    s32 y;
    s32 z;
    s32 w;
};

/* Fix12 multiply. P casts both operands; Q casts only the first and lets the
   second promote -- the two spellings give different value numbers, and the
   m3*v0 term needs Q to reproduce the ROM's v0/v3 register assignment. */
#define P(a, b) ((s32)((((long long)(a) * (long long)(b)) + 0x800) >> 12))
#define Q(a, b) ((s32)(((long long)(a) * (b) + 0x800) >> 12))

void func_0202ffec(struct Quat *m, struct Quat *v, struct Quat *out)
{
    s32 m0 = m->x;
    s32 m1 = m->y;
    s32 m2 = m->z;
    s32 m3 = m->w;
    s32 v0 = v->x;
    s32 v1 = v->y;
    s32 v2 = v->z;
    s32 v3 = v->w;

    out->x = P(v2, m1) + (Q(m3, v0) + P(v3, m0)) - P(m2, v1);
    out->y = P(m2, v0) + (P(m3, v1) + P(m1, v3)) - P(m0, v2);
    out->z = P(m0, v1) + (P(m3, v2) + P(m2, v3)) - P(m1, v0);
    out->w = P(m3, v3) - P(m0, v0) - P(m1, v1) - P(m2, v2);
}
