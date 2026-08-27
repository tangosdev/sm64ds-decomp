#include "types.h"
extern void *func_02071910(void *a, void *b);
extern void func_02072dac(void *x, void *b);
/* Unprototyped on purpose: the two call sites pass different argument lists.
 * The default-case call keeps the 0x80 test result live in r1 at zero cost
 * (r1 is untouched between the dispatch and the call), which is what pins the
 * entry-loop opcode byte to r0. */
extern void func_020731fc();
extern void func_02071864(void *a, void *b);
extern u8 *ReadSignedVarInt(u8 *p, int *out);
extern u8 *func_02071a50(u8 *p, int *out);

int func_020729f4(char *c)
{
  char *p18 = c + 0x18;
  u8 *ip;
  u8 op;
  int m;
  u8 *p1;
  u8 *p2;
  u8 *p3;
  u8 *p4;
  char *ctx;
  void *x;
  int buf[40];

  for (;;)
  {
    ip = *((u8 **) (c + 8));
    if (ip != 0)
    {
      op = *ip;
      m = op & 0x80;
      if (m == 0)
      {
        break;
      }
    }
    x = func_02071910(p18, c);
    func_02072dac(x, c);
    if ((*((int *) (c + 4))) == 0)
    {
      func_020731fc();
    }
    func_02071864(p18, c);
    if ((*((u8 **) (c + 8))) == 0)
    {
      continue;
    }
    goto tail;
  }

  switch (op & 0x1f)
  {
    case 2:
      p1 = ReadSignedVarInt(ip + 1, &buf[0]);
      *((u8 **) (c + 8)) = p1 + 4;
      break;
    case 3:
      p1 = ReadSignedVarInt(ip + 1, &buf[1]);
      p2 = ReadSignedVarInt(p1, &buf[2]);
      *((u8 **) (c + 8)) = p2 + 4;
      break;
    case 4:
      p1 = ReadSignedVarInt(ip + 1, &buf[3]);
      *((u8 **) (c + 8)) = p1 + 4;
      break;
    case 5:
      p1 = ReadSignedVarInt(ip + 1, &buf[4]);
      p2 = func_02071a50(p1, &buf[5]);
      p3 = func_02071a50(p2, &buf[6]);
      *((u8 **) (c + 8)) = p3 + 4;
      break;
    case 6:
      p1 = ReadSignedVarInt(ip + 1, &buf[7]);
      p2 = ReadSignedVarInt(p1, &buf[8]);
      *((u8 **) (c + 8)) = p2 + 4;
      break;
    case 7:
      p1 = ReadSignedVarInt(ip + 1, &buf[9]);
      p2 = ReadSignedVarInt(p1, &buf[10]);
      *((u8 **) (c + 8)) = p2 + 4;
      break;
    case 8:
      p1 = ReadSignedVarInt(ip + 1, &buf[11]);
      p2 = ReadSignedVarInt(p1, &buf[12]);
      p3 = ReadSignedVarInt(p2, &buf[13]);
      *((u8 **) (c + 8)) = p3 + 4;
      break;
    case 9:
      p1 = ReadSignedVarInt(ip + 1, &buf[14]);
      p2 = ReadSignedVarInt(p1, &buf[15]);
      p3 = func_02071a50(p2, &buf[16]);
      p4 = func_02071a50(p3, &buf[17]);
      *((u8 **) (c + 8)) = p4 + 4;
      break;
    case 10:
      p1 = ReadSignedVarInt(ip + 1, &buf[18]);
      *((u8 **) (c + 8)) = p1 + 4;
      break;
    case 11:
      p1 = ReadSignedVarInt(ip + 1, &buf[19]);
      p2 = ReadSignedVarInt(p1, &buf[20]);
      *((u8 **) (c + 8)) = p2 + 4;
      break;
    case 12:
      p1 = func_02071a50(ip + 5, &buf[21]);
      p2 = ReadSignedVarInt(p1, &buf[22]);
      *((u8 **) (c + 8)) = p2;
      break;
    case 13:
      p1 = ReadSignedVarInt(ip + 1, &buf[23]);
      *((u8 **) (c + 8)) = p1;
      break;
    case 15:
      p1 = func_02071a50(ip + 1, &buf[24]);
      p2 = func_02071a50(p1, &buf[25]);
      p3 = ReadSignedVarInt(p2, &buf[26]);
      *((u8 **) (c + 8)) = p3 + (buf[24] * 4);
      break;
    case 16:
      p1 = ReadSignedVarInt(ip + 1, &buf[27]);
      p2 = ReadSignedVarInt(p1, &buf[28]);
      buf[30] = ((p2[0] | (p2[1] << 8)) | (p2[2] << 16)) | (((u32) p2[3]) << 24);
      p3 = ReadSignedVarInt(p2 + 4, &buf[29]);
      *((u8 **) (c + 8)) = p3 + 4;
      break;
    case 17:
      p1 = ReadSignedVarInt(ip + 1, &buf[31]);
      p2 = ReadSignedVarInt(p1, &buf[33]);
      p3 = ReadSignedVarInt(p2 + 1, &buf[32]);
      p4 = ReadSignedVarInt(p3, &buf[34]);
      *((u8 **) (c + 8)) = p4 + 4;
      break;
    case 18:
      p1 = ReadSignedVarInt(ip + 1, &buf[35]);
      p2 = ReadSignedVarInt(p1 + 1, &buf[36]);
      p3 = func_02071a50(p2, &buf[37]);
      *((u8 **) (c + 8)) = p3 + 4;
      break;
    case 19:
      p1 = ReadSignedVarInt(ip + 1, &buf[38]);
      *((u8 **) (c + 8)) = p1;
      break;
    case 0:
    case 1:
    case 14:
    default:
      func_020731fc(op & 0x1f, m);
      break;
  }

  tail:
  op = (*(*((u8 **) (c + 8)))) & 0x1f;
  ctx = c;
  if (op != 1)
  {
    return op;
  }
  {
    int *pv = &buf[39];
    u8 **curptr = (u8 **) (c + 8);
    do
    {
      ReadSignedVarInt((*((u8 **) (ctx + 8))) + 1, pv);
      *curptr = (*curptr) + buf[39];
      op = (*(*((u8 **) (c + 8)))) & 0x1f;
    }
    while (op == 1);
  }
  return op;
}
