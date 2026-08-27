extern void func_ov002_020e032c(char* c);
extern void func_ov002_020bdef0(char* c);
extern void func_ov002_020bdd9c(char* c);
extern void func_ov002_020d80d0(char* c);
extern void func_ov002_020bdd2c(char* c);
extern void func_ov002_020cc1f4(char* c);
extern void func_ov002_020de968(char* c);

void func_ov002_020bda48(char* c)
{
    *(unsigned short*)(c+0x6ae) = 0;
    func_ov002_020e032c(c);
    func_ov002_020bdef0(c);
    func_ov002_020bdd9c(c);
    func_ov002_020d80d0(c);
    func_ov002_020bdd2c(c);
    func_ov002_020cc1f4(c);
    func_ov002_020de968(c);
}
