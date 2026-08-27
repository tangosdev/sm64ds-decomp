extern int func_ov007_020c0638(void* p, int a, int b, int d);
extern int* data_ov007_0210342c[];
void func_ov007_020b131c(void){
  int* base = data_ov007_0210342c[0];
  int* A = (int*)(base[0xe0/4]) ; // [r0,#0xe0] then [+4]
  int* a = (int*)A[1];
  int* C = (int*)(base[0xcc/4]);
  int* b = (int*)C[1];
  a[0x28/4]=0; a[0x2c/4]=0; a[0x24/4]=0; a[0x20/4]=a[0x24/4];
  b[0x28/4]=0; b[0x2c/4]=0; b[0x24/4]=0; b[0x20/4]=b[0x24/4];
  func_ov007_020c0638(a, 0, -0xc0, 0);
  func_ov007_020c0638(b, 0, -0xc0, 0);
}
