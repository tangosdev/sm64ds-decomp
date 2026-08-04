//cpp
// @symbol _ZN14CutsceneObject6RenderEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "CutsceneObject.h"
extern "C" {
void _ZN9ModelBase12ApplyOpacityEj(void* m, unsigned int op, int z);
void func_ov002_020f65b8(void* o);
}
struct M48 { int w[12]; };
extern M48 data_0209b41c;
struct ModelBase {
  virtual void v0(); virtual void v1(); virtual void v2();
  virtual void v3(); virtual void v4(); virtual void m(int arg);
};

int CutsceneObject::Render()
{
  if (unk_008 == 0x19){
    struct { char* p; char* cur; M48* src; } s;
    s.src = &data_0209b41c;
    s.p = *(char**)((char*)&mModel) + 0x1c;
    s.cur = s.p;
    *(M48*)s.cur = *s.src;
    int* tbl = data_ov002_0210bb7c;
    int i = 0;
    int zero = 0;
    do {
      *(int*)(s.p + 0x24) = tbl[0];
      *(int*)(s.p + 0x28) = tbl[1];
      *(int*)(s.p + 0x2c) = tbl[2];
      ((ModelBase*)*(void**)((char*)&mModel))->m(zero);
      tbl += 3;
      i++;
    } while ((unsigned)i < 3u);
    return 1;
  }
  unsigned char op = unk_102;
  if (op == 0) return 1;
  {
    void* a = *(void**)((char*)&mModel);
    if (a != 0){
      a = (void*)((int)a);
      _ZN9ModelBase12ApplyOpacityEj(a, op, 0);
      ((ModelBase*)*(void**)((char*)&mModel))->m((int)((char*)&unk_080));
    } else {
      void* b = *(void**)((char*)&unk_0e0);
      if (b != 0){
        b = (void*)((int)b);
        func_ov002_020f65b8(b);
        _ZN9ModelBase12ApplyOpacityEj(*(void**)((char*)&unk_0e0), unk_102, 0);
        ((ModelBase*)*(void**)((char*)&unk_0e0))->m((int)((char*)&unk_080));
      }
    }
  }
  return 1;
}
