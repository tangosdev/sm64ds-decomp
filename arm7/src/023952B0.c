/* storage 0x023952B0 runtime 0x027C7228 size 0x30 main. Constant pool +0x30, 4 bytes.
   Both pragmas + the pp local pin the ROM's register map: &unk_380FFF4 stays in
   ip, *pp loads into r0, s + 0x500 adds in place, fields ldrh to r3/r2/r1. */
#pragma scheduling off
#pragma opt_propagation off
typedef struct {
    char pad[0x500];
    struct {
        char pad2[0xA8];
        unsigned short a, b, c;
    } sub;
} S;
extern S *unk_380FFF4;

int fn_023952B0(void) {
    S **pp = &unk_380FFF4;
    S *s = *pp;
    s->sub.c = s->sub.b + s->sub.c * s->sub.a;
    return (*pp)->sub.c;
}
