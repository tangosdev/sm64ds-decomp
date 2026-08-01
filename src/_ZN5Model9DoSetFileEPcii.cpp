//cpp
// @symbol _ZN5Model9DoSetFileEPcii
/* recovered: named members + shared header, real C++ method */
#include "Model.h"
extern "C" {
int _ZN5Model23AddToCommonModelDataArrER8BMD_File(void*);
unsigned int func_02046564(void*);
void* _ZN6Memory13operator_new2Ej(unsigned int);
int func_020462d0(void*, void*, void*);
int _ZN15ModelComponents21UpdateVertsUsingBonesEv(void*);
int func_02016b24(void*, int);
int _ZN5Model12SetPolygonIDEi(void*, int);
}

int Model::DoSetFile(char * file_, int a, int b)
{
    void* file = (void*)file_;

  void* buffer;
  _ZN5Model23AddToCommonModelDataArrER8BMD_File(file);
  *(void**)((char*)&unk_04c) = _ZN6Memory13operator_new2Ej(func_02046564(file));
  buffer = *(void**)((char*)&unk_04c);
  if (buffer == 0) return 0;
  func_020462d0((char*)((void*)this)+8, file, buffer);
  _ZN15ModelComponents21UpdateVertsUsingBonesEv((char*)&unk_008);
  if (a != 0) func_02016b24(((void*)this), 0x8000);
  if (b < 0) return 1;
  _ZN5Model12SetPolygonIDEi(((void*)this), b & 0xff);
  return 1;
}
