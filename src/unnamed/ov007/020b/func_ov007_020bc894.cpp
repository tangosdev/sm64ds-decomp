//cpp
extern "C" {
extern void func_ov007_020c9110(void*);
extern void _ZN15ModelComponents11UpdateBonesEP8BCA_Filei(void*, void*, int);
extern void func_02046bbc(void*, void*, int);
extern void func_020469e8(void*, void*, int);
void func_ov007_020bc894(char* c){
  void* model=*(void**)c;
  char* p;
  short idx;
  p=*(char**)(c+0x18);
  if(p){
    func_ov007_020c9110(p);
    p=*(char**)(c+0x18);
    idx=*(short*)(p+0xc);
    if(idx!=-1){
      _ZN15ModelComponents11UpdateBonesEP8BCA_Filei(model, *(void**)(*(char**)(c+0x14)+idx*4), *(unsigned short*)(p+0x10));
    }
  }
  p=*(char**)(c+0x30);
  if(p){
    func_ov007_020c9110(p);
    p=*(char**)(c+0x30);
    idx=*(short*)(p+0xc);
    if(idx!=-1){
      func_02046bbc(model, *(void**)(*(char**)(c+0x2c)+idx*4), *(unsigned short*)(p+0x10));
    }
  }
  p=*(char**)(c+0x24);
  if(p){
    func_ov007_020c9110(p);
    p=*(char**)(c+0x24);
    idx=*(short*)(p+0xc);
    if(idx!=-1){
      func_020469e8(model, *(void**)(*(char**)(c+0x20)+idx*4), *(unsigned short*)(p+0x10));
    }
  }
}
}
