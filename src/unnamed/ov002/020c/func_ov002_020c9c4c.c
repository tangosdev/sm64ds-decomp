typedef int Fix12i;
extern int _ZN6Player7SetAnimEji5Fix12IiEj(void*, unsigned int, int, Fix12i, unsigned int);
extern unsigned int data_ov002_020ff118[];

void func_ov002_020c9c4c(char* c) {
    _ZN6Player7SetAnimEji5Fix12IiEj(c, data_ov002_020ff118[*(unsigned char*)(c+0x6e3)], 0x40000000, 0x1000, 0);
    *(unsigned char*)(((int)c + 0x6e3)) += 7;
    *(char*)(c+0x743) = 1;
    *(char*)(c+0x716) = 1;
    *(char*)(c+0x713) = 0;
    *(int*)(c+0x9c) = 0;
    *(int*)(c+0x98) = 0;
    *(int*)(c+0xa8) = 0;
}
