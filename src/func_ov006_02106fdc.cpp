//cpp
// NONMATCHING: different op / idiom (div=28). Logic verified correct vs ROM; not
// byte-matchable from C at mwccarm 1.2/sp2p3 (see notes/matching-style.md).
// Counts as decompiled, not matched.
struct C;
extern "C" void func_ov006_021050bc(C* c);

struct C { char dummy; };

typedef void (C::*PMF)(int);
extern PMF data_ov006_02142840[];

extern "C" void func_ov006_02106fdc(C* c) {
    func_ov006_021050bc(c);
    int found = 0;
    int i = 0;
    *(unsigned char*)((char*)c + 0x4000 + 0xfe9) = 1;
    if (*(int*)((char*)c + 0x4000 + 0xcb8) > 0) {
        do {
            unsigned char k = *(unsigned char*)((char*)c + i + 0x4000 + 0xefa);
            (c->*data_ov006_02142840[k])(i);
            i++;
            if (k != 0)
                found++;
        } while (i < *(int*)((char*)c + 0x4000 + 0xcb8));
    }
    if (found != 0)
        return;
    *(int*)((char*)c + 0x4000 + 0xca8) = 4;
    *(unsigned char*)((char*)c + 0x4000 + 0xfdf) = 1;
    *(short*)((char*)c + 0x4e00 + 0xc4) = 0x40;
}
