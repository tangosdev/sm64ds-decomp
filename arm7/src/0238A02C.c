/* storage 0x0238A02C runtime 0x03801EC4 size 0x4A8 (+ pools to 0x4B4) wram ARM.
 * SND command task (noreturn loop): per-iteration receive via fn_0238155C,
 * 42-case dispatch on a shared halfword, per-case handler calls, ack via
 * fn_0238A4E0(cmd, status).
 * Levers (AM25/AR14/AR56 all stalled on the 5-reg perm + a base spill):
 * - #pragma opt_loop_invariants off: without it the loop-invariant address
 *   exprs v0+1/v0+2 are hoisted to the prologue (extra pinned R7), evicting
 *   the pool-literal base into per-use pool reloads (+0x14 size).
 * - #pragma opt_propagation off: with LICM off the callee-saved coloring
 *   follows first-decl->high, so the five decls are ordered v2,one,zero,v0,vB
 *   for the ROM homes R9,R8,R7,R6,R5.
 * - the five inits stay separate statements in ROM emission order
 *   (v0,vB,zero,v2,one); decl-with-init in either order breaks one of the two.
 * - +4 arms split as ROM shows: vB-var (19,20,21,35,36,37, MOV R0,R5) vs
 *   recomputed (v0+2) (18,24,25,34,40,41, ADD R0,R6,#4); +2 (v0+1) at 23,39. */

extern int fn_0238155C(int *a1, int *a2, int a3);
extern int fn_0238AF68(void);
extern int fn_0238A4E0(int a1, int a2);
extern int fn_0238AE68(int a1);
extern int fn_0238AE3C(int a1);
extern int fn_0238ADE4(int a1);
extern int fn_0238AD8C(int a1);
extern int fn_0238AD0C(int a1);
extern int fn_0238AC68(int a1);
extern int fn_0238ABCC(int a1);
extern int fn_0238AB54(int a1);
extern int fn_0238AAFC(int a1);
extern int fn_0238AAA4(int a1);
extern int fn_0238AA4C(int a1);
extern int fn_0238AE10(int a1);
extern int fn_0238ADB8(int a1);
extern int fn_0238AD60(int a1);
extern int fn_0238ACB8(int a1);
extern int fn_0238AC18(int a1);
extern int fn_0238AB80(int a1);
extern int fn_0238AB28(int a1);
extern int fn_0238AAD0(int a1);
extern int fn_0238AA78(int a1);
extern int fn_0238AA20(int a1);
extern int unk_3809520[];
extern unsigned short unk_3809620[];

#pragma opt_loop_invariants off
#pragma opt_propagation off
void fn_0238A02C(void) {
    int *v2;
    int one;
    int zero;
    unsigned short *v0;
    char *vB;
    v0 = (unsigned short *)0x027FFDE8;
    vB = (char *)v0 + 4;
    zero = 0;
    v2 = unk_3809520;
    one = 1;
    int c16 = 0x10;
    int c17 = 0x11;
    int c18 = 0x12;
    int c19 = 0x13;
    int two = 2;
    int c20 = 0x14;
    int c21 = 0x15;
    int c22 = 0x16;
    int c23 = 0x17;
    int c24 = 0x18;
    int c25 = 0x19;
    int c32 = 0x20;
    int c33 = 0x21;
    int c34 = 0x22;
    int c35 = 0x23;
    int c36 = 0x24;
    int c37 = 0x25;
    int c38 = 0x26;
    int c39 = 0x27;
    int c40 = 0x28;
    int c41 = 0x29;
    int x19;
    for (;;) {
        unsigned short cmd;
        fn_0238155C(v2, &x19, one);
        cmd = unk_3809620[0x50];
        switch (cmd) {
        case 0:
            fn_0238AF68();
            v2[103] = zero;
            fn_0238A4E0(zero, zero);
            break;
        case 1:
            fn_0238AE68(((unsigned)*v0 << 30) >> 31);
            v2[103] = zero;
            fn_0238A4E0(one, zero);
            break;
        case 16:
            fn_0238AE3C((int)v0);
            v2[103] = zero;
            fn_0238A4E0(c16, zero);
            break;
        case 17:
            fn_0238ADE4((int)v0);
            v2[103] = zero;
            fn_0238A4E0(c17, zero);
            break;
        case 18:
            fn_0238AD8C((int)(v0 + 2));
            v2[103] = zero;
            fn_0238A4E0(c18, zero);
            break;
        case 19:
            if (fn_0238AD0C((int)vB) == 0) {
                v2[103] = zero;
                fn_0238A4E0(c19, two);
            } else {
                v2[103] = zero;
                fn_0238A4E0(c19, zero);
            }
            break;
        case 20:
            if (fn_0238AC68((int)vB) == 0) {
                v2[103] = zero;
                fn_0238A4E0(c20, two);
            } else {
                v2[103] = zero;
                fn_0238A4E0(c20, zero);
            }
            break;
        case 21:
            if (fn_0238ABCC((int)vB) == 0) {
                v2[103] = zero;
                fn_0238A4E0(c21, two);
            } else {
                v2[103] = zero;
                fn_0238A4E0(c21, zero);
            }
            break;
        case 22:
            fn_0238AB54((int)v0);
            v2[103] = zero;
            fn_0238A4E0(c22, zero);
            break;
        case 23:
            fn_0238AAFC((int)(v0 + 1));
            v2[103] = zero;
            fn_0238A4E0(c23, zero);
            break;
        case 24:
            fn_0238AAA4((int)(v0 + 2));
            v2[103] = zero;
            fn_0238A4E0(c24, zero);
            break;
        case 25:
            fn_0238AA4C((int)(v0 + 2));
            v2[103] = zero;
            fn_0238A4E0(c25, zero);
            break;
        case 32:
            fn_0238AE10((int)v0);
            v2[103] = zero;
            fn_0238A4E0(c32, zero);
            break;
        case 33:
            fn_0238ADB8((int)v0);
            v2[103] = zero;
            fn_0238A4E0(c33, zero);
            break;
        case 34:
            fn_0238AD60((int)(v0 + 2));
            v2[103] = zero;
            fn_0238A4E0(c34, zero);
            break;
        case 35:
            if (fn_0238ACB8((int)vB) == 0) {
                v2[103] = zero;
                fn_0238A4E0(c35, two);
            } else {
                v2[103] = zero;
                fn_0238A4E0(c35, zero);
            }
            break;
        case 36:
            if (fn_0238AC18((int)vB) == 0) {
                v2[103] = zero;
                fn_0238A4E0(c36, two);
            } else {
                v2[103] = zero;
                fn_0238A4E0(c36, zero);
            }
            break;
        case 37:
            if (fn_0238AB80((int)vB) == 0) {
                v2[103] = zero;
                fn_0238A4E0(c37, two);
            } else {
                v2[103] = zero;
                fn_0238A4E0(c37, zero);
            }
            break;
        case 38:
            fn_0238AB28((int)v0);
            v2[103] = zero;
            fn_0238A4E0(c38, zero);
            break;
        case 39:
            fn_0238AAD0((int)(v0 + 1));
            v2[103] = zero;
            fn_0238A4E0(c39, zero);
            break;
        case 40:
            fn_0238AA78((int)(v0 + 2));
            v2[103] = zero;
            fn_0238A4E0(c40, zero);
            break;
        case 41:
            fn_0238AA20((int)(v0 + 2));
            v2[103] = zero;
            fn_0238A4E0(c41, zero);
            break;
        default:
            v2[103] = zero;
            fn_0238A4E0(cmd, one);
            break;
        }
    }
}
