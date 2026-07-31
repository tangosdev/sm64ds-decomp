typedef unsigned char u8;

extern u8 data_0209fc58;
extern int data_ov075_0211d810;
extern int data_ov075_0211d870;
extern int data_ov075_0211d8b8;

extern void func_ov075_02119dc4(int a, int* data);
extern int func_0203da9c(void);
extern int TouchArea_Update(void* c, int id);
extern int func_ov075_02116d9c(int unused, int id);
extern void func_ov001_020ab41c(void* c);
extern void func_ov075_0211a148(char* c, int a, int b);
extern void func_02012790(int x);

void func_ov075_02118248(char* self)
{
    int id;
    if (data_0209fc58 != 0) {
        func_ov075_02119dc4((int)self, &data_ov075_0211d810);
        return;
    }
    id = func_0203da9c();
    if (TouchArea_Update(self + 0x70, id) != 0 ||
        (func_ov075_02116d9c((int)self, id) != 0 && *(u8*)(self + 0x281) == 0)) {
        *(u8*)(self + 0xa4) = 0;
        func_ov001_020ab41c(self + 0x70);
        func_ov075_0211a148(self, (int)&data_ov075_0211d870, 0x14);
        func_02012790(0x126);
        return;
    }
    if (TouchArea_Update(self + 0x94, id) == 0) {
        if (func_ov075_02116d9c((int)self, id) == 0)
            return;
        if (*(u8*)(self + 0x281) != 1)
            return;
    }
    *(u8*)(self + 0x80) = 0;
    func_ov001_020ab41c(self + 0x94);
    func_ov075_0211a148(self, (int)&data_ov075_0211d8b8, 0x14);
    func_02012790(0x119);
}
