extern int func_02012694(int, void *);
extern void _ZN9Animation7AdvanceEv(void *);
extern int func_ov091_02133710(void *);
int func_ov090_02133830(char *c) {
    int val = *(int*)(c + 0x364);
    val = (val >> 12) << 16;
    val = (unsigned int)val >> 16;
    if (val == 0) {
        func_02012694(9, c + 0x74);
    }
    *(int*)(c + 0x368) = 0x1000;
    _ZN9Animation7AdvanceEv(c + 0x35c);
    func_ov091_02133710(c);
    return 1;
}
