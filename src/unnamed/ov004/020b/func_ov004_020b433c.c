struct S { char pad24[0x24]; int f24; int f28; short f2c; };
void func_ov004_020b433c(struct S* s) {
    s->f24 = 0;
    s->f28 = ((s->f2c + 1) << 2) + 0x1e << 0xc;
}
