extern int func_ov007_020c9214();
/* c: an ov007 model record (animation tables); no class or header names it, so raw. */
void func_ov007_020bc2e4(void* c, int i, int a2){
  unsigned short** arr = *(unsigned short***)((char*)c+0x2c);
  unsigned short* p = arr[i];
  if(p==0) return;
  func_ov007_020c9214(*(void**)((char*)c+0x30), i, 1, *p, a2, 0x1000);
}
