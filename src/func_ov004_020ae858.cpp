//cpp
// @symbol func_ov004_020ae858
// NONMATCHING: base materialization / addressing, 10 of 118 words at 2004/b56.
// This line is FIRST on purpose. tools/progress.py, tools/validate_merge.py and
// tools/pr_linkcheck.py all test for the marker in the first 200 characters of the
// file, so a marker below a paragraph of prose is a marker the repo cannot see: the
// public matched count would rise by one and credit THIS body, and validate_merge
// would publish matched:true for a function differing in ten words.
/* recovered: renamed to Class_Method, RTTI class fields named */
//
// dScMgBase_c's results-panel LABEL RENDERER: the three buttons a minigame's
// results screen offers, drawn every frame while the panel is up
// (func_ov004_020b04f4 / dScMgBase_c::BeforeRender hands it the whole frame
// when self->unk_4628 != 0).
//
// THE NON-MATCH IN FULL: base materialization / addressing plus the register-coloring
// swap it carries. 10 of 118 words differ at mwccarm 2004/b56 (closest of all 25
// installed builds; 1.2/base, 1.2/sp2 and 1.2/sp2p3 all sit at 12). Logic verified against the
// ROM instruction by instruction, and every other word is identical -- including all
// FOURTEEN relocated words (nine arm_call, five load; config/arm9/overlays/ov004/
// relocs.txt lines 122-135) and the whole nine-word literal pool. Not one of the ten
// is a different OPERATION and none changes what the function computes. Six of the ten
// are one instruction reached through a different base register (the r5/r6 identity
// swap); the other four are the SAME four zero-stores over the SAME four addresses,
// emitted in order 0,3,1,2 off sp instead of 0,1,2,3 off the hoisted base -- a
// permutation within one group, not four different instructions. Detail below.
// Counts as decompiled, not matched.
//
// THE RESIDUAL, exactly: the ROM hoists &mtx into r6 and &pos[0] into r5 and writes the
// four matrix zero-stores through the hoisted base in index order
// (str fp,[r6] / [r6,#4] / [r6,#8] / [r6,#0xc]); we take r5/r6 the other way round and
// fold the same four stores into sp-relative form in order 0,3,1,2. Nothing else differs.
// Swept without closing it (~170 candidates): struct vs array matrix, block- and
// function-scope matrix pointers, pointer-assignment placement, an unrolled zero loop,
// cast-arith addressing, the u64 identity-mask launder on the base and per store,
// opt_propagation / opt_common_subs / opt_strength_reduction off, and the full
// 144-way product of array declaration order x zero-store statement order.
// The launder DOES force the ROM's base-register store form -- and pays for it by
// pulling the 0xffff pool load and the loop-counter init up out of the preheader,
// trading 10 divergences for 30. That is the "base materialization / addressing" floor
// notes/mwccarm-codegen.md already retires from paid refine attempts.
typedef short s16;
typedef unsigned short u16;
typedef unsigned int u32;
typedef long long s64;

struct Pos { s16 x; s16 y; };

extern "C" {
int func_ov004_020b8f18(void *p);
int GetGameLanguage(void);
int func_02053200(int x);
void func_ov004_020aea78(void *rec, int x, int y, void *mtx);
/* 0x020aea30. The config calls this _ZN5Enemy12KillByAttackER5Actor; it is not an
   Enemy method. src/_ZN5Enemy12KillByAttackER5Actor.cpp is the glyph-string walker
   that drives func_ov004_020aea78 over 8-byte records until the 0xffff terminator,
   and it takes four arguments. Spelled with the config's name because that is the
   symbol the reloc resolves to. */
void _ZN5Enemy12KillByAttackER5Actor(void *rec, int x, int y, void *mtx);
extern s16 data_02082214[];              /* FX sin/cos table, stride 2 halfwords */
extern void **data_ov004_020bc020[];     /* button 0's label, per language */
extern void **data_ov004_020bbfbc[];     /* button 1's label, per language */
extern void **data_ov004_020bbfd0[];     /* button 2's label, per language */
extern void *data_ov006_021346bc;        /* the shared plate drawn under every button */
}

extern "C" void func_ov004_020ae858(void *self_)
{
    char *self = (char *)self_;
    void *lbl[3];
    Pos *pos[3];
    void *m0;
    void *m1;
    void *m2;
    int i;
    int mtx[4];

    if (func_ov004_020b8f18(self + 0xf4))
        return;

    lbl[0] = *data_ov004_020bc020[GetGameLanguage()];
    lbl[1] = *data_ov004_020bbfbc[GetGameLanguage()];
    lbl[2] = *data_ov004_020bbfd0[GetGameLanguage()];
    pos[0] = (Pos *)(self + 0x4634);
    pos[1] = (Pos *)(self + 0x4638);
    pos[2] = (Pos *)(self + 0x463c);

    m1 = 0;
    m2 = 0;
    m0 = 0;

    for (i = 0; i < 3; i++) {
        if (i == *(s16 *)(self + 0x4646)) {
            /* the highlighted button: pulse it with a uniform 2x2 scale matrix
               s = 1 / (1.0 + sin(timer)/2), and walk its glyphs inline so every
               record carries the transform */
            u16 *p = (u16 *)lbl[i];
            u16 v;
            int s = data_02082214[((u16)(*(u32 *)(self + 0x4640) >> 1) >> 4) * 2];
            int k = func_02053200((int)((((s64)s << 11) + 0x800) >> 12) + 0x1000);
            mtx[0] = 0;
            mtx[1] = 0;
            mtx[2] = 0;
            mtx[3] = 0;
            mtx[0] = k;
            mtx[3] = k;
            do {
                func_ov004_020aea78(p, pos[i]->x, pos[i]->y, mtx);
                v = p[3];
                p += 4;
            } while (v != 0xffff);
            _ZN5Enemy12KillByAttackER5Actor(data_ov006_021346bc, pos[i]->x, pos[i]->y, m0);
        } else {
            _ZN5Enemy12KillByAttackER5Actor(lbl[i], pos[i]->x, pos[i]->y, m1);
            _ZN5Enemy12KillByAttackER5Actor(data_ov006_021346bc, pos[i]->x, pos[i]->y, m2);
        }
    }
}
