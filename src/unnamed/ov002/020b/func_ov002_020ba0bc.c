enum Bool { FALSE, TRUE };
extern int _ZN5Event8ClearBitEj();
extern int func_ov002_020ba4d8();
int func_ov002_020ba0bc(char* c){
    unsigned short v = *(unsigned short*)(c+0xc);
    enum Bool b = (enum Bool)(v==0xc);
    if(!b) _ZN5Event8ClearBitEj(*(unsigned char*)(c+0x34e));
    return func_ov002_020ba4d8(c, 3);
}
