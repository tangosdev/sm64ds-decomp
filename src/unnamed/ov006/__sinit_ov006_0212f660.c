extern void __cxa_vec_ctor(void* a, int b, int c, void* d, void* e);
extern void func_020731dc(void *object, void *destructor, void **node);
extern void Puff_ElemDtor(void);
extern int sPuffs;
extern void Puff_Nothing(void);
extern void Puff_CleanupAll(void);
extern void* data_ov006_021404cc;
void __sinit_ov006_0212f660(void) {
    __cxa_vec_ctor(&sPuffs, 3, 0x20, (void *)Puff_Nothing, (void *)Puff_ElemDtor);
    func_020731dc(0, (void *)Puff_CleanupAll, (void**)&data_ov006_021404cc);
}
