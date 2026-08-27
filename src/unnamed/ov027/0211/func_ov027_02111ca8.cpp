//cpp
extern "C" {
extern int _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(void*, void*, int, int, unsigned int);
extern void *data_ov027_02113c84[];
int func_ov027_02111ca8(char *c){
  _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(c+0x320, data_ov027_02113c84[1], 0, 0x1000, 0);
  *(int*)(c+0x37c) = 0x1000;
  *(char*)(c+0x3d8) = 0x14;
  *(int*)(c+0x3d0) = 0;
  return 1;
}
}
