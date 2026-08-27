//cpp
extern "C" { int func_ov004_020adbc0(void); int func_ov004_020b1a5c(int, int); }
extern "C" void func_ov006_02100488(char *c){
  if (*(int *)(c + 0x5660) < 2) return;
  func_ov004_020b1a5c(func_ov004_020adbc0(), 6);
}
