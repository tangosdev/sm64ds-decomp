//cpp
// @symbol _ZN5Stage7LoadFogEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "Stage.h"
extern void _ZN3Fog4InitEt5Fix12IiES1_(char *self, unsigned short color, int nearv, int farv);

void Stage::LoadFog()
{
  unsigned char a = 0;
  unsigned char b = 0x28;
  int i;
  for (i = 0; i < 0x20; i++)
  {
    *((unsigned char *) (((char *)&unk_96c) + i)) = a;
    *((unsigned char *) (((char *)&unk_994) + i)) = b;
    a += 4;
    b += 2;
  }
  *((unsigned char *) ((char *)&unk_98c)) = 0;
  *((unsigned char *) ((char *)&unk_9b4)) = 1;
  *((unsigned char *) ((char *)&unk_9b5)) = 6;
  *((unsigned short *) ((char *)&unk_9b6)) = 0;
  *((unsigned short *) ((char *)&unk_9b8)) = 0x6000;
  if (data_0209f258 == 0)
  {
    return;
  }
  {
    unsigned char *src = (unsigned char *) data_0209f31c;
    char *dst = ((char *)this) + 0x96c;
    int j;
    for (j = 0; j < data_0209f258; j++)
    {
      if (src[0] != 1)
      {
        *((unsigned char *) (dst + 0x20)) = 0;
      }
      else
      {
        unsigned short color = ((src[1] >> 3) | ((src[2] >> 3) << 5)) | ((src[3] >> 3) << 10);
        _ZN3Fog4InitEt5Fix12IiES1_(dst, color, *((unsigned short *) (src + 4)), *((unsigned short *) (src + 6)));
      }
      src += 8;
      dst += 0x28;
    }
  }
}
