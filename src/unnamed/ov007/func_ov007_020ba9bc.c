extern int func_ov007_020bad8c(void);
extern int func_ov007_020baa10(void);
extern int func_ov007_020c5188(void*, int);
struct G { char* obj; char pad[0x18]; int n; };
extern struct G* data_ov007_02104ba0;
extern char data_ov007_020d7b80[];
void func_ov007_020ba9bc(void){
  func_ov007_020bad8c();
  func_ov007_020baa10();
  *(int*)(data_ov007_02104ba0->obj+0x18) = (int)(data_ov007_020d7b80 + data_ov007_02104ba0->n*0x18);
  func_ov007_020c5188(data_ov007_02104ba0->obj, 0);
}
