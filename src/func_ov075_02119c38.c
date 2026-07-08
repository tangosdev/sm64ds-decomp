extern unsigned char BOTTOM_SCREEN_RELATED;
extern int data_ov075_0211d71c[];
extern void func_ov075_02116028(void* p, int v);
extern void func_020337ec(void);
extern void func_ov075_02115e8c(void* p, int a, int b, int d, int e);
void func_ov075_02119c38(char* c){
  BOTTOM_SCREEN_RELATED &= ~1;
  func_ov075_02116028(data_ov075_0211d71c, 1);
  func_020337ec();
  BOTTOM_SCREEN_RELATED |= 5;
  func_ov075_02115e8c(c+0x70, 0xa, 0, 0x80, 0xa8);
  *(unsigned char*)(c+0x280) = 1;
  *(unsigned char*)(c+0x282) = 0;
  *(int*)(c+0x26c) = 0;
  *(int*)(c+0x270) = 0x78;
}
