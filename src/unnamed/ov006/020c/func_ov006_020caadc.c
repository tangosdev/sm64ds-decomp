typedef struct { int x, y, z; } Vec3;
extern void func_0203cd80(Vec3* m, short angle);
extern void func_0203ccd4(Vec3* m, short angle);
extern void func_ov006_020c8e90(char* o);
extern void func_ov006_020c8a30(void);
extern int data_ov006_021405bc;
extern char* data_ov006_02140554;

void func_ov006_020caadc(void) {
  Vec3 v; Vec3 w;
  int i, n, off;
  v.x=0; v.y=0; v.z=0xfffff008;
  func_0203cd80(&v, -0x2000); func_0203ccd4(&v, -0x3000);
  *(volatile unsigned*)0x40004c8 = (((short)v.x >> 3) & 0x3ff) | ((((short)v.y >> 3) & 0x3ff) << 10) | ((((short)v.z >> 3) & 0x3ff) << 20);
  w.x=0; w.y=0; w.z=0xfffff008;
  func_0203cd80(&w, -0x2000);
  *(volatile unsigned*)0x40004c8 = ((((short)w.x >> 3) & 0x3ff) | ((((short)w.y >> 3) & 0x3ff) << 10) | ((((short)w.z >> 3) & 0x3ff) << 20)) | 0x40000000;
  *(volatile unsigned*)0x40004cc = 0x7fff;
  *(volatile unsigned*)0x40004cc = 0x40007fff;
  
  n = data_ov006_021405bc;
  i = 0;
  if (n > 0) {
    off = 0;
    do {
      func_ov006_020c8e90(data_ov006_02140554 + off);
      n = data_ov006_021405bc;
      i++;
      off += 0xdc;
    } while (i < n);
  }
  func_ov006_020c8a30();
}
