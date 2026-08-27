//cpp
extern "C" {
extern int _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(void*,void*,int,int,unsigned int);
extern int data_ov094_02136af8[];
int func_ov094_02135bd4(void* c){
  *(short*)((char*)c+0x100)=0x1e;
  *(int*)((char*)c+0x3e8)=0;
  _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj((char*)c+0x30c, (void*)data_ov094_02136af8[1], 0, 0x1000, 0);
  *(int*)((char*)c+0x3f0)=0x2000;
  return 1;
}
}
