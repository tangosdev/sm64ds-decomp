typedef struct C C;
extern int DecIfAbove0_Byte();
extern void func_ov002_020ba4d8(C *c, int i);
void func_ov002_020ba3a8(C *c){
  if(*(unsigned char*)((char*)c+0x350)){
    if(DecIfAbove0_Byte((char*)c+0x34f)!=0) return;
    func_ov002_020ba4d8(c,1);
    return;
  }
  *(unsigned char*)((char*)c+0x34f)=5;
}
