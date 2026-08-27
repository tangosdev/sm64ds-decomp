extern void* _Znwj(unsigned int sz);
extern void* func_02022298(char *self);
extern int func_ov002_020f5fe4(void *p);
extern int func_ov002_020f6424(char* c);
int func_ov002_020f6448(char *self, unsigned char *arg1)
{
    char *p;
    *(unsigned char*)(self + 0x100) = *arg1;
    p = (char*)_Znwj(0x518);
    if (p)
        func_02022298(p + 0x200);
    *(void**)(self + 0xd8) = p;
    if (*(void**)(self + 0xd8) == 0)
        return 0;
    func_ov002_020f5fe4(*(void**)(self + 0xd8));
    return func_ov002_020f6424(self);
}
