
struct S_0210ff1c
{
  char _0[8];
  int f8;
  int fc;
  char _10[0x34 - 0x10];
  struct 
  {
    int a;
    int b;
  } arr[3];
};
extern void *data_ov006_021379dc;
extern void *data_ov006_021379e8;
extern void *data_ov006_02137a00;
extern void *data_ov006_021379f4;
extern void func_ov004_020afdd0(void *, int, int, int, int);
inline int inline_fn(int arg0, int arg1)
{
  return arg0 + arg1;
}

void func_ov006_0210ff1c(void *self)
{
  struct S_0210ff1c *s = self;
  int i;
  int x;
  int xo;
  for (i = 0, x = 0; i < 3; i++, x += 0x18)
  {
    if (1)
    {
    }
    func_ov004_020afdd0(data_ov006_021379dc, inline_fn(s->f8 >> 12, x - 0x18), s->fc >> 12, -1, 2);
  }

  i = 0;
  x = 0;
second_loop:
  {
    xo = (x - 0x18) & 0xFFFFFFFFFFFFFFFF;
    func_ov004_020afdd0(data_ov006_021379e8, inline_fn(inline_fn(s->f8 >> 12, s->arr[i].a >> 12), xo), inline_fn(s->fc >> 12, s->arr[i].b >> 12), -1, 3);
    func_ov004_020afdd0(data_ov006_02137a00, inline_fn(inline_fn(s->f8 >> 12, s->arr[i].a >> 12), xo), inline_fn(s->fc >> 12, s->arr[i].b >> 12) - 0x10, -1, 3);
    func_ov004_020afdd0(data_ov006_021379f4, inline_fn(inline_fn(s->f8 >> 12, s->arr[i].a >> 12), xo), inline_fn(s->fc >> 12, s->arr[i].b >> 12) - 0x20, -1, 3);
    func_ov004_020afdd0(data_ov006_021379e8, inline_fn(inline_fn(s->f8 >> 12, s->arr[i].a >> 12), xo), inline_fn(s->fc >> 12, s->arr[i].b >> 12) - 0x30, -1, 3);
    x += 0x18;
    i++;
    if (i < 3)
      goto second_loop;
  }

}
