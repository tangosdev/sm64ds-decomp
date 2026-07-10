typedef short s16;
extern void func_ov029_02111850(char* c);
extern void func_ov029_021117ac(char* c);
extern char* _ZN5Actor10FindWithIDEj(unsigned int id);
extern void func_ov029_021118c8(char* t);
extern void _ZN12CylinderClsn5ClearEv(void* self);
extern void _ZN12CylinderClsn6UpdateEv(void* self);

int _ZN9ArrowLift8BehaviorEv(char* c) {
    func_ov029_02111850(c);
    if (*(int*)(c+0x158) == 0) return 1;
    func_ov029_021117ac(c);
    if (*(unsigned char*)(c+0x15d) != 0) {
        char* p;
        s16* a = (s16*)(((int)c + 0x8e) & 0xFFFFFFFFFFFFFFFF);
        *a = *a + 0x400;
        p = _ZN5Actor10FindWithIDEj(*(unsigned int*)(c+0x158));
        if (p != 0) {
            if (*(int*)(c+0x60) == *(int*)(p+0x60)) {
                if (*(s16*)(c+0x8e) == 0) *(unsigned char*)(c+0x15d) = 0;
            }
            if (*(int*)(c+0x60) != *(int*)(p+0x334)) {
                *(unsigned char*)(c+0x15d) = 0;
                *(s16*)(c+0x8e) = 0;
            }
        }
    }
    func_ov029_021118c8(c);
    _ZN12CylinderClsn5ClearEv((void*)(c + 0x124));
    _ZN12CylinderClsn6UpdateEv((void*)(c + 0x124));
    return 1;
}
