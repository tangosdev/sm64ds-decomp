extern int data_ov006_021428cc;
extern int data_ov006_021428d4[];
extern void func_0203d47c(void);
extern void* data_ov006_02142948;
extern void func_020731dc(int a, int b, void** node);
extern void func_ov006_02109530(void* self, void* arg, int n);

void func_ov006_021092e8(void* self)
{
    int b = data_ov006_021428cc & 1;
    if (b == 0) {
        data_ov006_021428d4[0] = 0xc000;
        data_ov006_021428d4[1] = 0xc000;
        func_020731dc((int)data_ov006_021428d4, (int)func_0203d47c, &data_ov006_02142948);
        data_ov006_021428cc |= 1;
    }
    func_ov006_02109530(self, data_ov006_021428d4, 0x80);
    *(unsigned char*)((char*)self + 0x32) = 7;
}
