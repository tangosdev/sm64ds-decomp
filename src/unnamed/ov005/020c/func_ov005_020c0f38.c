extern int func_ov005_020c00b4(int r0, int n);
extern int func_ov005_020c007c(int r0, int r1);
extern int _ZN3OAM9RenderSubEP7OamAttriiii(void* oam, int a, int b, int c, int d);
extern unsigned char data_0209b304;
struct Pair { int a, b; };
extern struct Pair data_ov005_020c22c8[];
extern int data_0208a170;
extern int data_ov005_020c2f60[];
void func_ov005_020c0f38(char* sl){
  int g = (*(int*)(sl+0x9c) >= 0x20) ? 1 : 0;
  int start, end;
  if(data_0209b304 == 1){ start = 6; end = 8; }
  else { start = 0; end = 5; }
  if(*(unsigned char*)(sl+0x54) == 1) return;
  int i;
  for(i = start; i <= end; i++){
    if(func_ov005_020c00b4((int)sl, data_0208a170 + i*4) == 0) continue;
    if(func_ov005_020c007c((int)sl, data_0208a170 + i*4) == 0) continue;
    _ZN3OAM9RenderSubEP7OamAttriiii((void*)data_ov005_020c2f60[g],
        data_ov005_020c22c8[i].a, data_ov005_020c22c8[i].b + 0x30, -1, 0);
  }
}
