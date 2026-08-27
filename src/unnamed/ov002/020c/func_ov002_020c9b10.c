extern int _ZN6Player7SetAnimEji5Fix12IiEj(void* c, unsigned int a, int b, int f, unsigned int g);
int func_ov002_020c9b10(char* c){
    *(unsigned char*)(c+0x6e3)=0xe;
    _ZN6Player7SetAnimEji5Fix12IiEj(c,0x47,0,0x1000,0);
    *(short*)(c+0x6a4)=0x14;
    *(int*)(c+0x98)=0;
    *(int*)(c+0xa8)=0;
    return 0;
}
