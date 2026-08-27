extern void func_ov002_020f5a6c(char* p);
extern void func_ov002_020f396c(char* p);
extern void func_ov002_020f30d4(char* p);
extern void func_ov002_020f2ea0(char* p);
extern void func_ov002_020f2a68(char* p);
extern void func_ov002_020f2984(char* p);

void func_ov002_020f5ad4(char* c)
{
    *(int*)(c+0x1f0) = 0;
    *(int*)(c+0x1f4) = 0;
    *(int*)(c+0x1f8) = 0;
    *(unsigned char*)(c+0x514) = 0;
    func_ov002_020f5a6c(c);
    func_ov002_020f396c(c);
    func_ov002_020f30d4(c);
    func_ov002_020f2ea0(c);
    func_ov002_020f2a68(c);
    func_ov002_020f2984(c);
}
