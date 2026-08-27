extern unsigned int __aeabi_uidiv(unsigned int a, unsigned int b);

int func_ov070_02121a64(char* c)
{
    switch (*(int*)c) {
    case 0:
        if (*(unsigned int*)(c + 0xc) < *(unsigned int*)(c + 8)) {
            unsigned int* p = (unsigned int*)(((int)c + 0xc));
            *p = *p + 1;
        }
        break;
    case 1:
        {
            unsigned int* p = (unsigned int*)(((int)c + 0xc));
            *p = *p + 1;
            *p = *p % *(unsigned int*)(c + 8);
        }
        break;
    }
    return (*(unsigned int**)(c + 4))[*(unsigned int*)(c + 0xc)];
}
