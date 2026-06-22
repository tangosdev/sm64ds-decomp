struct S { unsigned short flags; unsigned short pad; short val; };

void func_020551f0(struct S* s, int v){
  unsigned short h = s->flags;
  if (v < 0) {
    if ((h & 0xc0) == 0x80) s->flags = (h & ~0xc0) | 0xc0;
    s->val = (short)(-v);
  } else {
    if ((h & 0xc0) == 0xc0) s->flags = (h & ~0xc0) | 0x80;
    s->val = (short)v;
  }
}
