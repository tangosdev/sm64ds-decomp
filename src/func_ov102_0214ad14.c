extern void func_ov102_0214b384(void *self, int arg);
extern void *_ZN8dActor_c13ClosestPlayerEv(void *self);
extern void func_ov102_0214c0b8(void *self);

void func_ov102_0214ad14(void *c)
{
    func_ov102_0214b384(c, 0x96);
    *(void **)((char *)c + 0x38c) = _ZN8dActor_c13ClosestPlayerEv(c);
    func_ov102_0214c0b8(c);
}
