int func_ov066_021168b0(char* c);
int RandomIntInternal(int* seed);
void func_ov066_02119454(char* c, void* p);
extern int data_0209e650;
extern unsigned char data_ov066_0211ae0c;
extern int data_ov066_0211b03c;
int func_ov066_021189c0(char* c){
  if(func_ov066_021168b0(c) == 0) return 1;
  if((((unsigned int)RandomIntInternal(&data_0209e650) >> 0x1f) & 1) == 0)
    data_ov066_0211ae0c = 2;
  else
    data_ov066_0211ae0c = 1;
  c[0x4d9] = 0;
  func_ov066_02119454(c, &data_ov066_0211b03c);
  return 1;
}
