extern int func_ov063_0211a3d0();
extern void _ZN9ActorBase18MarkForDestructionEv();
extern void func_0201267c();
void func_ov063_02116dbc(void* c) {
    if (func_ov063_0211a3d0(c) == 0) return;
    _ZN9ActorBase18MarkForDestructionEv(c);
    func_0201267c(0xd5, (char*)c + 0x74);
}
