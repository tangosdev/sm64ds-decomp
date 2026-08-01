extern int _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(char* anim, void* file, int a, int b, unsigned int u);
extern struct S data_ov085_021305c0;
int func_ov085_0212bc14(char *p){
  _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(p+0x300, *(void**)((char*)&data_ov085_021305c0+4), 0, 0x1000, 0);
  *(int*)(p+0x42c)=*(int*)(p+0x5c);
  *(int*)(p+0x430)=*(int*)(p+0x60);
  *(int*)(p+0x434)=*(int*)(p+0x64);
  *(int*)(p+0x41c)=0;
  *(short*)(p+0x100)=0;
  return 1;
}
