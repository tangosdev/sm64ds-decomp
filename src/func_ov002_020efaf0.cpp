//cpp
struct BMD_File;
struct PathStuff { void* a; void* file; };  // data_0210d9f0: load [4]

extern PathStuff SNUFIT_BULLET_MODEL_PTR;
extern void func_ov002_020eff90(void);

extern "C" {
void _ZN9ModelBase7SetFileEP8BMD_Fileii(char* thiz, void* file, int b, int d);
void _ZN7PathPtr6FromIDEj(char* thiz, unsigned int id);
int func_ov002_020efedc(char* c);
void func_ov002_020efa54(char* c, int i);
void func_020393c4(char* p, int v);

void func_ov002_020efaf0(char* c)
{
  int i;
  char* m = c + 0x320;
  for (i = 0; i < 3; i++, m += 0x50) {
    _ZN9ModelBase7SetFileEP8BMD_Fileii(m, SNUFIT_BULLET_MODEL_PTR.file, 1, -1);
  }
  _ZN7PathPtr6FromIDEj(c + 0x430, *(unsigned int*)(c + 8) & 0xff);
  if (func_ov002_020efedc(c)) {
    func_ov002_020efa54(c, 0);
  } else {
    func_ov002_020efa54(c, 1);
  }
  {
    *(short*)(c + 0x422) = *(short*)(c + 0x8c);
    *(short*)(c + 0x424) = *(short*)(c + 0x8e);
    *(short*)(c + 0x426) = *(short*)(c + 0x90);
    *(int*)(c + 0x410) = *(int*)(c + 0x5c);
    *(int*)(c + 0x414) = *(int*)(c + 0x60);
    *(int*)(c + 0x418) = *(int*)(c + 0x64);
    *(short*)(c + 0x41c) = *(short*)(c + 0x8c);
    *(short*)(c + 0x41e) = *(short*)(c + 0x8e);
    *(short*)(c + 0x420) = *(short*)(c + 0x90);
  }
  func_020393c4(c + 0x124, (int)&func_ov002_020eff90);
}
}
