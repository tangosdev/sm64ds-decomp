extern void __cxa_vec_ctor(void* a, int b, int c, void* d, void* e);
extern void func_020731dc(int a, int b, void** node);
extern void func_ov006_020c8dcc(void);
extern int data_ov006_021404d8;
extern void func_ov006_020c8dd0(void);
extern void func_ov006_020c8da8(void);
extern void* data_ov006_021404cc;
void __sinit_ov006_0212f660(void) {
    __cxa_vec_ctor(&data_ov006_021404d8, 3, 0x20, (void*)func_ov006_020c8dd0, (void*)func_ov006_020c8dcc);
    func_020731dc(0, (int)func_ov006_020c8da8, (void**)&data_ov006_021404cc);
}
