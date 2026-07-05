extern void func_ov075_0211a194(void* self, void* g);
extern int data_0208ee44[];

void func_ov075_02119d78(char* c){
  int* p = (int *)(((int)c + 0x264) & 0xFFFFFFFFFFFFFFFF);
  *p = *p - data_0208ee44[0];
  if(*(int*)(c+0x264) > 0) return;
  func_ov075_0211a194(c, *(void**)(c+0x6c));
}
