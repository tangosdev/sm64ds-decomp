extern int data_0209d574;
extern int data_020a6134[];
extern int data_ov000_020bd4b8[];
extern int func_0201a9ec(int* p);
extern int func_02018a58(int n);
extern int func_02057e7c(int p);
extern int func_02057e84(int p, int v);
extern int func_020189f0(int a);
extern void func_02018a40(unsigned i, int v);
extern int func_0201a9fc(int* p);
extern int func_0201a96c(int* self, int newVal);
void func_ov000_020aa420(void){
  int sb=func_0201a9ec(&data_0209d574);
  func_02018a58(0x80a);
  int r8=data_020a6134[2];
  int r7=func_02057e7c(r8);
  func_02057e84(r8,0x1f);
  int i;
  for(i=0;i<0x80a;i++){
    int v=func_020189f0(data_ov000_020bd4b8[i]);
    if(v) func_02018a40(i,v);
  }
  func_02057e84(r8,r7);
  func_0201a9fc(&data_0209d574);
  func_0201a96c(&data_0209d574,sb);
}
