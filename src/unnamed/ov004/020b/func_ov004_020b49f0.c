extern int _Z15ApproachLinear2Rsss(short* dst, short to, short step);
struct S { char pad10[0x10]; short f10; short f12; short f14; char pad16[0x32-0x16]; short f32; };
void func_ov004_020b49f0(struct S* s) {
    if (_Z15ApproachLinear2Rsss((short*)((char*)s + 0x10), s->f14, 0x10))
        s->f32 = 1;
}
