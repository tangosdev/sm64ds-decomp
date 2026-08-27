//cpp
extern "C" {
extern int data_ov002_021101b4[];
extern void _ZN6Player11ChangeStateERNS_5StateE(void*,void*);
int func_ov002_020cc01c(char* c){
  if (*(unsigned short*)(c+0x6b8) == 0) return 0;
  _ZN6Player11ChangeStateERNS_5StateE(c, data_ov002_021101b4);
  return 1;
}
}
