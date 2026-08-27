extern void *func_ov007_020c3df4(int a, int b);
extern void func_ov007_020c10ec(void *c);
extern void Sprite_SetAnimation(void *c, int r1, int r2, int r3, int sp0);

void *func_ov007_020c11ac(int n){
  char *o = (char*)func_ov007_020c3df4(0, 0x44);
  if(n == 0){
    *(void**)(o+0x18) = 0;
  } else {
    *(void**)(o+0x18) = func_ov007_020c3df4(0, n*4);
  }
  func_ov007_020c10ec(o);
  *(unsigned char*)(o+0x1c) = (unsigned char)n;
  Sprite_SetAnimation(o, 0, 1, 0, 0);
  return o;
}
