extern unsigned short data_0209b274;
extern int func_020094a8(void *p);
extern unsigned int ReadUnalignedInt(unsigned char *p);
extern void func_02009414(void *p);

int func_020082a0(void *self, const void *src, int id)
{
    if (id == data_0209b274) {
        func_020094a8(self);
        *(int *)((char *)self + 0x14c) = (int)ReadUnalignedInt((unsigned char *)src);
        *(int *)((char *)self + 0x150) = (int)ReadUnalignedInt((unsigned char *)src + 4);
    }
    func_02009414(self);
    return 1;
}
