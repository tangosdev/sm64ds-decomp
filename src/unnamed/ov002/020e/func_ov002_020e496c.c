extern int _ZNK6Player14GetBodyModelIDEjb(void* thiz, unsigned int a, int b);
int func_ov002_020e496c(char* c)
{
    int id = _ZNK6Player14GetBodyModelIDEjb(c, *(unsigned char*)(c + 0x6db), 1);
    return ((int*)(c + 0xdc))[id];
}
