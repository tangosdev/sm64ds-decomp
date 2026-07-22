typedef unsigned short u16;
struct Entry
{
  u16 id;
  u16 pad2;
  u16 pad4;
  u16 pad6;
  u16 count8;
  u16 offA;
};
struct Node0
{
  char pad0[0x18];
  char *m18;
  char *pad1c;
  char *m20;
  char *pad24;
  char *m28;
};
struct Ctx
{
  struct Node0 *node;
  char *base;
};
struct Desc
{
  u16 field0;
  u16 pad2;
  u16 *field4;
  u16 field8;
  u16 padA;
  u16 *fieldC;
  u16 *field10;
  u16 *field14;
  u16 *field18;
  u16 count;
  u16 pad1e;
  struct Entry *entries;
};
extern void Crash(void);
void func_02046bbc(struct Ctx *ctx, struct Desc *desc, unsigned int sb)
{
  int i;
  if (sb >= desc->field0)
  {
    Crash();
  }
  for (i = 0; i < desc->count; i++)
  {
    int j;
    u16 *p;
    struct Entry *e;
    char *dst;
    int k;
    int v0;
    u16 idx0;
    char *item;
    int flags;
    e = &desc->entries[i];
    if (e->id == 0xffff)
    {
      Crash();
    }
    dst = ctx->base + (e->id * 0x30);
    j = e->count8 - 1;
    p = ((&desc->field10[e->offA]) + e->count8) - 1;
    for (;;)
    {
      if ((*p) <= sb)
      {
        break;
      }
      p--;
      j--;
      if (j < 0)
      {
        Crash();
      }
    }

    k = j + e->offA;
    idx0 = desc->field14[k];
    v0 = idx0;
    idx0 = *((u16 *) (((char *) desc->field4) + (v0 * 8)));
    item = ctx->node->m18 + (idx0 * 0x14);
    *((int *) (dst + 0x1c)) = *((int *) ((((char *) ctx->node->m28) + (e->id * 0x30)) + 0x20));
    *((int *) (((unsigned int) (dst + 0x1c)) & 0xFFFFFFFFFFFFFFFFULL)) |= *((int *) (item + 0x10));
    flags = ((*((unsigned int *) (item + 0x10))) >> 26) & 7;
    if (desc->field8 != 0)
    {
      u16 t = desc->field18[k];
      int c = *((u16 *) (((char *) desc->fieldC) + (t * 8)));
      if (c != (-1))
      {
        char *a = ctx->node->m20 + (c * 0x10);
        if (flags == 2)
        {
          *((int *) (dst + 0x20)) = (*((unsigned int *) (a + 0xc))) >> 3;
        }
        else
        {
          *((int *) (dst + 0x20)) = (*((unsigned int *) (a + 0xc))) >> 4;
        }
      }
    }
  }

}
