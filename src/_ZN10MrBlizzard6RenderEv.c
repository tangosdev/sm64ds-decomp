extern void _ZN5Model12HideMaterialEii(void*,int,int);
extern void _ZN5Model12ShowMaterialEii(void*,int,int);
extern void _ZN5Model6RenderEPK7Vector3(void*,void*);
int _ZN10MrBlizzard6RenderEv(char *t){
  int s=*(int*)(t+0x41c);
  if(s==3) return 1;
  if(s==2){
    if(*(int*)(t+0x400))
      _ZN5Model12HideMaterialEii(t+0x30c,0,2);
    else
      _ZN5Model12ShowMaterialEii(t+0x30c,0,2);
  }
  _ZN5Model6RenderEPK7Vector3(t+0x30c, t+0x80);
  return 1;
}
