typedef unsigned int u32;
struct Entry { int x; void* bta; char pad[4]; };
struct Info { char a[0x10]; struct Entry* entries; unsigned char count; };
extern struct Info* LEVEL_FILE;
extern void* _Znwj(u32);
extern void _ZN18TextureTransformer7PrepareER8BMD_FileR8BTA_File(void* bmd, void* bta);
extern void* _ZN18TextureTransformerC1Ev(void*);
extern void _ZN18TextureTransformer7SetFileER8BTA_Filei5Fix12IiEj(void* self, void* bta, int a, int b, u32 c);
void _ZN5Stage23LoadTextureTransformersEv(char* c){
  struct Entry* e = LEVEL_FILE->entries;
  void** slot = (void**)(c+0x8bc);
  void* bmd = *(void**)(c+0x874);
  int i;
  for(i=0;i<(int)LEVEL_FILE->count;i++){
    if(e->bta){
      _ZN18TextureTransformer7PrepareER8BMD_FileR8BTA_File(bmd, e->bta);
      void* t = _Znwj(0x14);
      if(t) t = _ZN18TextureTransformerC1Ev(t);
      *slot = t;
      _ZN18TextureTransformer7SetFileER8BTA_Filei5Fix12IiEj(*slot, e->bta, 0, 0x1000, 0);
    }
    e++;
    slot = (void**)((char*)slot + 0xc);
  }
}
