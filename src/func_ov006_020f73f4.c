extern void func_ov006_020c0aa8(char*);
extern void func_ov004_020b1bc8(char*, int, int, int);
extern void func_ov004_020b6430(void);
extern void func_ov006_020f5620(char *thiz);
extern void func_ov006_020f565c(char *c);
extern void func_ov006_020f5b98(char*);
extern void func_ov006_020c1804(char*);
int func_ov006_020f73f4(char* c){
  func_ov006_020c0aa8(c + 0x4660);
  func_ov004_020b1bc8(c, 0xc, 0xc, 0);
  func_ov004_020b6430();
  func_ov006_020f5620(c);
  func_ov006_020f565c(c);
  func_ov006_020f5b98(c);
  func_ov006_020c1804(c + 0x4f38);
  return 1;
}
