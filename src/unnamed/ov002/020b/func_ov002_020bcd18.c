extern int Player_ScaleByCharFactor(void*, int);
int func_ov002_020bcd18(void *c){
    *(int*)((char*)c+0x98)=Player_ScaleByCharFactor(c, 0xa000);
    return 1;
}
