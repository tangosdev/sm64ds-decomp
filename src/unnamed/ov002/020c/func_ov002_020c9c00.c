extern int _ZN6Player7SetAnimEji5Fix12IiEj(void* c, unsigned int a, int b, int f, unsigned int g);
int func_ov002_020c9c00(char* c){
    if(*(unsigned char*)(c+0x6de)!=0){
        _ZN6Player7SetAnimEji5Fix12IiEj(c,0x54,0x40000000,0x1000,0);
    }
    *(unsigned char*)(c+0x6e3)=0;
    *(int*)(c+0x98)=0;
    *(int*)(c+0xa8)=0;
    return 0;
}
