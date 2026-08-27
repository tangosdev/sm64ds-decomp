extern int ApproachAngle(void*,int,int,int,int);
extern int func_ov074_021203e4(void* c, int i);
extern int func_ov074_021216f4(void* c);
int func_ov074_0211fbd0(char* c){
    if(ApproachAngle((char*)c+0x5f6, 0, 0xa, 0x100, 1)==0)
        return func_ov074_021203e4(c, 3);
    return func_ov074_021216f4(c);
}
