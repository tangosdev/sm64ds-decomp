//cpp
// NONMATCHING: pure r8/sb register-coloring swap (div=9). Instruction stream and
// size are byte-identical except `this` homes to sb and `c+0x4000` to r8 where the
// ROM has r8/sb. Exhausted: 120 decl-order perms, stmt-order, 6q value-web swap,
// u64 launder variants, #pragma opt_{strength_reduction,propagation,common_subs,
// scheduling,peephole}, C vs C++ PMF forms, sibling access shapes. Same residual
// on 1.2/{base,sp2,sp2p3}. Floor class: regperm (callee-saved this/base swap).
// Hand to permuter / 6q follow-ups if a new lever lands for r8/sb this-pointer homes.
struct C;
extern "C" void func_ov006_021050bc(C* c);

struct C { char dummy; };

typedef void (C::*PMF)(int);
extern PMF data_ov006_02142840[];

#pragma opt_strength_reduction off
extern "C" void func_ov006_02106fdc(C* arg) {
    register unsigned char* base;
    C* c;
    unsigned char k;
    int found;
    int i;
    c = (C*)((unsigned int)arg & 0xFFFFFFFFFFFFFFFFull);
    func_ov006_021050bc(c);
    found = 0;
    i = 0;
    base = (unsigned char*)c + 0x4000;
    *(base + 0xfe9) = 1;
    if (*(int*)(base + 0xcb8) > 0) {
        do {
            k = *((unsigned char*)c + i + 0x4000 + 0xefa);
            (c->*data_ov006_02142840[k])(i);
            i++;
            if (k != 0)
                found++;
        } while (i < *(int*)(base + 0xcb8));
    }
    if (found != 0)
        return;
    *(int*)((char*)c + 0x4000 + 0xca8) = 4;
    *(unsigned char*)((char*)c + 0x4000 + 0xfdf) = 1;
    *(short*)((char*)c + 0x4e00 + 0xc4) = 0x40;
}
