extern void _ZN19CylinderClsnWithPosC1Ev(void*);
extern void _ZN19CylinderClsnWithPos4InitERK7Vector35Fix12IiES4_jj(void* self, void* pos, int fix, int a3, unsigned int a4, unsigned int a5);
extern int data_ov027_02113d10[];
char* func_ov027_021125a8(char* c){
  _ZN19CylinderClsnWithPosC1Ev(c);
  *(int*)(c+0x40)=data_ov027_02113d10[0];
  *(int*)(c+0x44)=data_ov027_02113d10[1];
  *(int*)(c+0x48)=data_ov027_02113d10[2];
  _ZN19CylinderClsnWithPos4InitERK7Vector35Fix12IiES4_jj(c, c+0x40, 0x32000, 0x28000, 0x800002, 0x800000);
  *(char*)(c+0x5e)=0;
  return c;
}
