//cpp
typedef int s32;
extern "C" {
void _ZN3OAM9RenderSubEP7OamAttriiii(void* a, int b, int c, int d, int e);
s32 func_ov004_020ad674(void);
void func_ov004_020af948(void* a, int b, int c, void* m);
extern void* data_ov006_0213e528[];
extern void** data_ov006_0213e5ec[];
void func_ov006_0210c374(char *c);
}
void func_ov006_0210c374(char *c)
{
    _ZN3OAM9RenderSubEP7OamAttriiii(
        data_ov006_0213e528[*(unsigned char*)(c + 0x10)],
        *(short*)(c + 4), *(short*)(c + 6), -1, 1);
    int i;
    int sb = 0xb0;
    int z = 0;
    for (i = 0; i < *(int*)(c + 0x1c); i++) {
        func_ov004_020af948(data_ov006_0213e5ec[func_ov004_020ad674()][2], 0xb0 + i*0x10, sb, (void*)z);
    }
}
