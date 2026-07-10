//cpp
extern "C" {
struct SharedFilePtr { int a, file; };
extern SharedFilePtr data_ov002_02110a9c;
extern SharedFilePtr data_ov002_02110aa4;
extern void _ZN15TextureSequence8LoadFileER13SharedFilePtr(SharedFilePtr&);
extern void* _ZN5Model8LoadFileER13SharedFilePtr(SharedFilePtr&);
extern int _ZN9ModelBase7SetFileEP8BMD_Fileii(void* self, void* f, int a, int b);
extern void _ZN15TextureSequence7PrepareER8BMD_FileR8BTP_File(void* bmd, void* btp);
extern void _ZN15TextureSequence7SetFileER8BTP_Filei5Fix12IiEj(void* self, void* btp, int a, int c, unsigned int n);

int _ZN9OneUpLogo13InitResourcesEv(char* self){
  unsigned short n;
  {
    unsigned int v = *(unsigned int*)(self+8);
    n = (unsigned short)(v > 8 ? 7 : (v - 1));
  }
  _ZN15TextureSequence8LoadFileER13SharedFilePtr(data_ov002_02110a9c);
  if (_ZN9ModelBase7SetFileEP8BMD_Fileii(self+0xd4, _ZN5Model8LoadFileER13SharedFilePtr(data_ov002_02110aa4), 1, -1) == 0)
    return 0;
  _ZN15TextureSequence7PrepareER8BMD_FileR8BTP_File((void*)data_ov002_02110aa4.file, (void*)data_ov002_02110a9c.file);
  _ZN15TextureSequence7SetFileER8BTP_Filei5Fix12IiEj(self+0x124, (void*)data_ov002_02110a9c.file, 0x40000000, 0, n);
  *(unsigned char*)(self+0x14e) = 0;
  *(int*)(self+0x13c) = *(int*)(self+0x5c);
  *(int*)(self+0x140) = *(int*)(self+0x60);
  *(int*)(self+0x144) = *(int*)(self+0x64);
  *(int*)(self+0xa8) = 0x14000;
  *(int*)(self+0x9c) = -0x2000;
  *(int*)(self+0xa0) = -0x32000;
  *(short*)(self+0x14c) = 0;
  *(int*)(self+0x138) = 0;
  *(int*)(self+0x148) = 0;
  return 1;
}
}
