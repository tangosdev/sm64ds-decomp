#include "types.h"
extern void *data_ov007_02103370[];
extern u16 data_ov007_020d7584[];
extern void *(*data_ov007_02103368)(u16);
extern void *(*data_ov007_0210336c)(void *);
extern int func_ov007_020c3df4(int a, void *b);
extern void DecompressLZ16(void *src, void *dst);
extern void _ZN4CP1514FlushDataCacheEjj(u32 a, u32 b);
extern void _ZN5Model17UpdateFileOffsetsER8BMD_File(void *f);
extern void _ZN9Animation17UpdateFileOffsetsER8BCA_File(void *f);
extern void _ZN15TextureSequence17UpdateFileOffsetsER8BTP_File(void *f);
void *func_ov007_020b3f20(int idx)
{
  void *new_var;
  if (idx == 0)
  {
    return 0;
  }
  if (data_ov007_02103370[idx] == 0)
  {
    void *obj = data_ov007_02103368(data_ov007_020d7584[idx]);
    data_ov007_02103370[idx] = obj;
    obj = (new_var = data_ov007_02103370[idx]);
    switch (idx)
    {
      case 0x26:

      case 0x27:

      case 0x28:

      case 0x29:

      case 0x2a:

      case 0x2c:
      {
        u32 hv = (*((u32 *) obj)) >> 8;
        int sz = func_ov007_020c3df4(0, (void *) hv);
        DecompressLZ16(obj, (void *) sz);
        data_ov007_0210336c(obj);
        _ZN4CP1514FlushDataCacheEjj(sz, hv);
        data_ov007_02103370[idx] = (void *) sz;
      }
        break;

      case 1:

      case 2:

      case 3:

      case 0x16:

      case 0x20:

      case 0x21:

      case 0x22:

      case 0x23:

      case 0x24:
        _ZN5Model17UpdateFileOffsetsER8BMD_File(obj);
        break;

      case 4:

      case 5:

      case 6:

      case 7:

      case 8:

      case 9:

      case 0xa:

      case 0xb:

      case 0xc:

      case 0xd:

      case 0xe:

      case 0xf:
        _ZN9Animation17UpdateFileOffsetsER8BCA_File(obj);
        break;

      case 0x10:

      case 0x11:

      case 0x12:

      case 0x13:

      case 0x14:

      case 0x15:
        _ZN15TextureSequence17UpdateFileOffsetsER8BTP_File(obj);
        break;

      default:
        break;

    }

  }
  return data_ov007_02103370[idx];
}
