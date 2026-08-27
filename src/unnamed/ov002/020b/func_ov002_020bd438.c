extern unsigned int ReadUnalignedInt(unsigned char *p);
extern void func_02012694(unsigned int, void*);
int func_ov002_020bd438(unsigned char *c, unsigned char *p){
    unsigned int v=ReadUnalignedInt(p);
    func_02012694(v, c+0x74);
    return 1;
}
