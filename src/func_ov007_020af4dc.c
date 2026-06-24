extern void func_ov007_020c1d8c(int i);
extern void func_ov007_020bfacc(int a, int b, void* p);
extern char data_ov007_0210342c[];
extern char func_ov007_020b0520[];
extern char func_ov007_020af580[];

void func_ov007_020af4dc(void) {
  char* g = *(char**)data_ov007_0210342c;
  if (*(int*)(*(char**)(g+8) + 0xc) == 0) {
    func_ov007_020c1d8c(0);
  }
  g = *(char**)data_ov007_0210342c;
  if (*(int*)(*(char**)(g+8) + 0xc) != 0x1e) return;
  if (*(int*)(g+0x10) == 0) {
    func_ov007_020bfacc(2, 1, func_ov007_020b0520);
  } else {
    func_ov007_020bfacc(1, 0, func_ov007_020af580);
  }
  (*(void(**)(int))(*(char**)data_ov007_0210342c + 0x2c))(0x13);
}
