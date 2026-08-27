extern void func_020733a8(void* obj, int a, int b, void* cb1, void* cb2);
extern void NullDestructor_0203d47c(void);
extern void func_0203d738(void);

void* func_ov006_020ede80(char* this)
{
    func_020733a8(this + 0x18, 5, 8, (void*)func_0203d738, (void*)NullDestructor_0203d47c);
    return this;
}
