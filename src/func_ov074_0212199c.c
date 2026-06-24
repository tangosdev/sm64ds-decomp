extern int func_ov074_02121a20(void* c, int idx);
extern int func_ov074_02120b24(void* c);
extern void func_02012694(int id, void* p);
extern void _ZN9Animation7AdvanceEv(void* p);
void func_ov074_0212199c(char* c){
  if(func_ov074_02121a20(c, 8)){
    *(int*)(c+0x26c) = func_ov074_02120b24(c);
  } else if(func_ov074_02121a20(c, 0xb)){
    if(((*(unsigned int*)(c+0x268) << 4) >> 0x10) == 8){
      func_02012694(0x15d, c+0x74);
    }
  } else {
    *(int*)(c+0x26c) = 0x1000;
  }
  _ZN9Animation7AdvanceEv(c+0x260);
  _ZN9Animation7AdvanceEv(c+0x3f8);
}
