// NONMATCHING: one register-allocation difference and its fallout. Logic
// verified instruction-for-instruction against
// extracted/overlays/overlay_0006.bin at base 0x020bfec0. Counts as
// decompiled, not matched.
//
// dScMgHanachan_c (actor id 0x182, scene 386, "Which Wiggler"). Vtable slot 9,
// Render (func_ov006_020ecee4), calls this UNCONDITIONALLY at 0x020ed0c8 --
// `add r0,r4,#0x4000 / ldr r0,[r0,#0xf60] / bl` -- and that is the only
// arm_call relocation in all of ov006 that reaches 0x020ea914. So this is the
// sole draw of whatever the pointer at scene+0x4f60 addresses, and with the
// body trapped, scene 386 rendered 1200 frames and drew it 0 times.
//
// WHAT IT DRAWS, AND IT IS THE THING THE MINIGAME ASKS ABOUT. The receiver is
// one 0x98-byte wiggler -- the same layout src/func_ov006_020eac38.c walks,
// read at +0x4c..+0x58 (four ints), +0x84 (s16 heading) and +0x94 (u8). This
// body lays that wiggler out at a FIXED pose rather than reading its live
// position: the head segment is planted at (0xb4000, 0x80000) and each earlier
// segment is the one behind it pulled back by its own +0x4c entry plus a flat
// 0x2000 per link. Then five OAM::Render calls draw the five body segments,
// and segment 0 gets a second sprite (data_ov006_021375c0[0], the face) when
// +0x94 is set and func_ov006_020eb7b0 agrees.
//
// That fixed pose is what makes it the QUESTION. src/func_ov006_020eac38.c is
// the same five-segment draw for the wigglers the player is looking at, and it
// takes their positions out of the object at +0x18. This one takes none: it is
// the reference picture the round is asking the player to match.
//
// COMPILER: mwccarm 2004/b56 -O4,p -proc arm946e. Measured rather than
// assumed: a sweep of 25 installed compiler builds times seven -O settings
// times four -proc values is 700 combinations, and exactly 16 reproduce the
// ROM's size of 0x324 at all -- 2004/b56 and the 1.2/base, 1.2/sp2, 1.2/sp2p3
// trio, each at -O3,p, -O4, -O4,p and -O4,s. All sixteen produce the SAME
// output, so the residue below is not a compiler-selection question.
//
// 201 words = 198 instructions (0x020ea914..0x020eac2c) + 3 pool words
// (0x020eac2c..0x020eac38) = 0x324/4. The candidate is the same 0x324 and the
// same 198 instructions, and a word-stream alignment against the ROM (calls
// and pool words wildcarded, the same wildcard tools/match.py applies) puts
// 165 of the 201 identical with 36 differing on each side.
//
// THE ONE CAUSE. Eight values want the eight callee-saved registers and nine
// are live across the segment loop. The ROM gives the last one to the
// loop-invariant -0x4000 and spills the constant -1 to [sp+0x24]; mwcc here
// makes the opposite call, keeps -1 in fp and spills -0x4000 to [sp+0x1c]:
//
//     ROM         rsb r4, r1, #0             ; -0x4000 lives in r4
//                 mvn r0, #0                 ; -1 spilled
//                 str r0, [sp, #0x24]
//                 mov fp, #0x1000
//                 ...
//                 add r0, r4, r0, asr #1     ; one shifted add, r4 as operand
//
//     candidate   rsb r0, r1, #0             ; -0x4000 spilled
//                 str r0, [sp, #0x1c]
//                 mvn fp, #0                 ; -1 lives in fp
//                 mov r4, #0x1000
//                 ...
//                 asr r0, r0, #1             ; the constant folded back in,
//                 sub r0, r0, #0x4000        ; so two instructions
//
// It is self-reinforcing, which is why no lever reaches it: once -0x4000 is
// off the register file mwcc rematerialises it as a `sub` immediate at the two
// shifted-add sites, that drops its use count from four to two, and two uses
// lose the register to -1's three. Everything else in the edit script is that
// difference propagating -- two extra instructions at the shifted adds, two
// fewer where -1 is stored straight out of fp instead of loaded, the
// [sp+0x1c]/[sp+0x20]/[sp+0x24] slots all sliding one word, and nine branch or
// pool displacements moving to match. Nothing computes a different value.
//
// LEVERS ALREADY SPENT, all measured and all recorded so nobody pays twice:
//   - pragma sweep, six pragmas singly and in pairs, 21 builds:
//     opt_strength_reduction off is REQUIRED (it is this TU's inherited lever,
//     the same one src/func_ov006_020eac38.c carries) and every other pragma
//     changes the size. opt_propagation off, the obvious answer to a folded
//     constant, is among them.
//   - five spellings of the -0x4000 base -- literal at all four sites, int
//     local, short local, `register` local, struct member, one-element array:
//     all identical output. mwcc folds it every way it can be written.
//   - forcing -1 into memory instead: as a struct member of `t` (mwcc
//     registerises it anyway), as a one- and two-element array (same), and as
//     a member of the escaping aggregate `s` (that one works, and costs a word).
//   - declaration-order sweep over the four scalar declarations, 24 builds:
//     a flat floor.
//
// The remaining six words are unrelated and are recorded separately: the
// prologue's shift loop is instruction-for-instruction right, including the
// ldr/str/ldr/str schedule that only the pointer-arithmetic form below
// reproduces (a `p[j] = p[j+1]` struct assignment lets mwcc prove the copy
// cannot alias and it hoists both loads), but its two scratch temps come out
// r0/r1 where the ROM has r1/r0.

typedef struct { int x, y; } Vec2;

extern short data_02082214[];
extern void* data_ov006_02142018[];
extern void* data_ov006_021375c0[];

extern void func_0203d388(int* p, int angle);
extern void func_0203d680(Vec2* out, const Vec2* in, int scale);
extern int func_ov006_020ebb40(char* o, int i);
extern int func_ov006_020ebc08(char* c, int n);
extern int func_ov006_020eb7b0(char* o);
extern int func_02053200(int x);
extern void _ZN3OAM6RenderEbP7OamAttriiii5Fix12IiEi(int b, void* attr, int x, int y, int a, int c, int f, int g);

#pragma opt_strength_reduction off
void func_ov006_020ea914(char* o)
{
    struct { int A, B, urot, va0x, res; } t;
    Vec2 vb0;
    struct { Vec2 pos, va, vb, vout; } s;
    Vec2 p[5];
    int px, py;
    int a2i;
    int i, j, k;
    short rot, ang;

    p[4].x = 0xb4000;
    p[4].y = 0x80000;
    for (j = 3; j >= 1; j--) {
        k = j + 1;
        *(int*)((char*)p + j * 8) = *(int*)((char*)p + k * 8);
        *(int*)((char*)p + j * 8 + 4) = *(int*)((char*)p + k * 8 + 4);
        *(int*)((char*)p + j * 8) -= *(int*)(o + j * 4 + 0x4c) + k * 0x2000;
    }
    p[0] = p[1];
    p[0].x -= *(int*)(o + 0x4c) + 0x2000;

    i = 0;
    vb0.x = 0x2000;
    vb0.y = -0x18000;
    t.A = 0;
    t.B = 0;
    t.va0x = 0;
    for (; i < 5; i++) {
        rot = -0x4000;
        s.pos.x = p[i].x;
        s.pos.y = p[i].y;
        s.va.x = t.va0x;
        s.va.y = 0x1000;
        ang = (short)(*(short*)(o + 0x84) - t.A);
        func_0203d388(&s.va.x, rot);

        if (*(unsigned char*)(o + 0x94) == 0) {
            if (i == 0) {
                rot = (short)(rot + (data_02082214[((unsigned short)ang >> 4) * 2] >> 1));
            } else {
                int sinv = data_02082214[((unsigned short)ang >> 4) * 2];
                rot = (short)(rot + (short)(((long long)sinv * (0x1800 - t.B) + 0x800) >> 12));
            }
        }

        func_0203d680(&s.vout, &s.va, data_02082214[((unsigned short)(ang << 1) >> 4) * 2]);
        s.pos.x += s.vout.x;
        s.pos.y += s.vout.y;

        t.res = func_ov006_020ebb40(o, i);
        px = s.pos.y >> 12;
        py = s.pos.x >> 12;
        _ZN3OAM6RenderEbP7OamAttriiii5Fix12IiEi(
            0,
            data_ov006_02142018[(a2i = func_ov006_020ebc08(o, i), (t.urot = (unsigned short)rot), a2i)],
            py, px,
            t.res, -1, 0x1000,
            t.urot);

        if (i == 0) {
            if (*(unsigned char*)(o + 0x94) != 0 && func_ov006_020eb7b0(o)) {
                int scale;
                s.vb.x = vb0.x;
                s.vb.y = vb0.y;
                func_0203d388(&s.vb.x, rot);
                s.vb.x += s.pos.x;
                s.vb.y += s.pos.y;
                scale = func_02053200((data_02082214[((unsigned short)ang >> 4) * 2] >> 2) + 0xc00);
                _ZN3OAM6RenderEbP7OamAttriiii5Fix12IiEi(
                    0, data_ov006_021375c0[0],
                    s.vb.x >> 12, s.vb.y >> 12,
                    -1, -1, scale, t.urot);
            }
        }

        t.A += 0x1800;
        t.B += 0x200;
    }
}
