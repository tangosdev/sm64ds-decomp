//cpp
extern "C" {
void* _ZN5Actor10FindWithIDEj(unsigned int id);
void* _ZN5Actor13ClosestPlayerEv(void* a);
int _ZN6Player17SetNoControlStateEhih(void* p, unsigned char a, int b, unsigned char c);
void func_ov014_02111ebc(void* c, int i);
int func_ov014_02111f54(void* c){
  char* r4=(char*)c;
  void* a=_ZN5Actor10FindWithIDEj(*(unsigned int*)(r4+0x608));
  if(*(unsigned char*)((char*)a+0x31e)!=0) goto fail;
  {
    void* p=_ZN5Actor13ClosestPlayerEv(r4);
    if(_ZN6Player17SetNoControlStateEhih(p,4,-1,0)==0) goto fail;
    func_ov014_02111ebc(r4,3);
    *(unsigned char*)(r4+0x605)=1;
    return 1;
  }
fail:
  return 0;
}
}
