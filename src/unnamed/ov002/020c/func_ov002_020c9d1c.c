extern int _ZN6Player7SetAnimEji5Fix12IiEj(void* c, unsigned int a, int b, int f, unsigned int g);
int func_ov002_020c9d1c(char* c){
    *(unsigned char*)(c+0x6e3)=4;
    *(short*)(c+0x69c)=0x4000;
    _ZN6Player7SetAnimEji5Fix12IiEj(c,0x47,0,0x1000,0);
    *(int*)(c+0x98)=0;
    *(int*)(c+0xa8)=0;
    return 0;
}
