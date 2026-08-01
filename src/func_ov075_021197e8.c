extern int func_0203da9c(void);
extern int func_ov075_02115098(int c, int arg1);
extern int TouchArea_Update(char* c, int arg1);
extern int func_ov075_02116d9c(int unused, int id);
extern void func_ov001_020ab41c(void* c);
extern void SetNumPlayers(int n);
extern void func_ov075_0211a148(char* c, int a, int b);
extern void func_02012790(int x);
extern void func_ov075_0211505c(int c);
extern char data_ov075_0211d900[];
extern char data_ov075_0211d8b8[];

void func_ov075_021197e8(char* c)
{
    if (*(unsigned char*)(c + 0x282) == 0) {
        *(unsigned char*)(c + 0x282) =
            (unsigned char)(func_ov075_02115098(*(int*)(c + 0x50), func_0203da9c()) != 0);
    }
    if (*(unsigned char*)(c + 0x282) == 0) return;
    if (TouchArea_Update(c + 0x70, -1) != 0 ||
        (func_ov075_02116d9c((int)c, -1) != 0 && *(unsigned char*)(c + 0x281) == 0)) {
        *(unsigned char*)(c + 0xa4) = 0;
        func_ov001_020ab41c(c + 0x70);
        SetNumPlayers(1);
        func_ov075_0211a148(c, (int)data_ov075_0211d900, 0x14);
        func_02012790(0x118);
        return;
    }
    if (TouchArea_Update(c + 0x94, -1) == 0) {
        if (func_ov075_02116d9c((int)c, -1) == 0) return;
        if (*(unsigned char*)(c + 0x281) != 1) return;
    }
    *(unsigned char*)(c + 0x80) = 0;
    func_ov001_020ab41c(c + 0x94);
    func_ov075_0211a148(c, (int)data_ov075_0211d8b8, 0x14);
    func_ov075_0211505c(*(int*)(c + 0x50));
    func_02012790(0x119);
}
