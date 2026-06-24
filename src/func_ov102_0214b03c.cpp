//cpp
extern "C" {
int _ZNK9Animation12WillHitFrameEi(void*, int);
int func_0201267c(int, void*);
int func_ov102_0214bf64(void*);
int func_ov102_0214be1c(void*);
int func_ov102_0214bd90(void*);
int func_ov102_0214bcc8(void*);
int func_ov102_0214bbd8(void*);
int func_ov102_0214ba30(void*);
extern int data_ov102_0214e9c0[];
void func_ov102_0214b03c(char* c){
  if(*(int*)(c+0x3dc) < 2 && *(int*)(c+0x360) == data_ov102_0214e9c0[1]){
    if(_ZNK9Animation12WillHitFrameEi(c+0x350, 0) != 0 || _ZNK9Animation12WillHitFrameEi(c+0x350, 0x10) != 0){
      func_0201267c(0x132, c+0x74);
    }
  }
  switch(*(int*)(c+0x3dc)){
  case 0: func_ov102_0214bf64(c); return;
  case 1: func_ov102_0214be1c(c); return;
  case 2: func_ov102_0214bd90(c); return;
  case 3: func_ov102_0214bcc8(c); return;
  case 4: func_ov102_0214bbd8(c); return;
  case 5: func_ov102_0214ba30(c); return;
  }
}
}
