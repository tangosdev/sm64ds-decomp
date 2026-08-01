extern int _ZN5Actor10FindWithIDEj(unsigned int id);
unsigned char func_ov063_021163d0(char *c){
    unsigned int id = *(unsigned int*)(c+0x498);
    if(id==0) return 0;
    char *a = (char*)(unsigned int)_ZN5Actor10FindWithIDEj(id);
    if(a==0) return 0;
    return *(unsigned char*)(a+0x153);
}
