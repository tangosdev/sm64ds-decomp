struct CylinderClsn { int dummy; };
extern void func_ov064_0211a734(void *c);
extern void _ZN12CylinderClsn5ClearEv(struct CylinderClsn *t);
extern void _ZN12CylinderClsn6UpdateEv(struct CylinderClsn *t);
int func_ov064_0211a7ec(char *c) {
    func_ov064_0211a734(c);
    _ZN12CylinderClsn5ClearEv((struct CylinderClsn *)(c + 0x138));
    _ZN12CylinderClsn6UpdateEv((struct CylinderClsn *)(c + 0x138));
    return 1;
}
