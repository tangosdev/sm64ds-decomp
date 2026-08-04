extern void func_ov004_020afdd0(void* a0, int a1, int a2, int a3, int a4);
extern void* data_ov006_021382e0;
extern void* data_ov006_021382f8;
void func_ov006_02113e54(char *c){
  int r2;
  if (*(unsigned char*)(c+0x30)==0) return;
  if (*(unsigned char*)(c+0x121)==0) return;
  r2 = 1;
  if (*(unsigned char*)(c+0x3a)==1) r2 = 3;
  if ((*(int*)(*(int*)(c+4)+8) & 0xff) == 0) {
    int x = *(int*)(c+8);
    int y = *(int*)(c+0xc);
    func_ov004_020afdd0(*(void**)&data_ov006_021382e0, (x+0x800)>>12, (y+0x800)>>12, -1, r2);
  } else {
    int x = *(int*)(c+8);
    int y = *(int*)(c+0xc);
    func_ov004_020afdd0(*(void**)&data_ov006_021382f8, (x+0x800)>>12, (y+0x800)>>12, -1, r2);
  }
}
