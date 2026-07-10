//cpp
extern "C" {
void func_020393a4(void* p, int v);
unsigned char func_ov080_0212714c(void* a, void* b);
extern int data_ov035_02112c98[];
int _ZN12FallBlockLll8BehaviorEv(char* c){
  func_020393a4(c+0x124, 0x500000);
  return func_ov080_0212714c(c, data_ov035_02112c98) & 0xff;
}
}
