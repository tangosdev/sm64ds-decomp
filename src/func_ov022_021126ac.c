extern int _ZN9ActorBase18MarkForDestructionEv(void *c);
extern int data_ov025_02112654(void *c);
int func_ov022_021126ac(char *c) {
    int a = *(int*)(c+0x60);
    int b = *(int*)(c+0x118);
    if (a < b) {
        _ZN9ActorBase18MarkForDestructionEv(c);
        return 1;
    }
    unsigned short h = *(unsigned short*)(c+0x100+0x10);
    if (h == 0) {
        _ZN9ActorBase18MarkForDestructionEv(c);
        return 1;
    }
    data_ov025_02112654(c);
    return 1;
}
