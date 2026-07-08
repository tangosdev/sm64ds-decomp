
typedef unsigned char u8;
typedef unsigned short u16;
typedef short s16;
typedef int s32;
typedef unsigned int u32;
typedef long long s64;
extern s16 SINE_TABLE[];
struct Ent
{
  int out0;
  int out4;
  char pad8[8];
  int vx;
  char pad14[0x1a];
  u16 angle;
  char pad30[0x1c];
};
void func_ov002_020f3ae8(char *base, int idx)
{
  struct Ent *arr = (struct Ent *) base;
  int a;
  int b;
  a = SINE_TABLE[((arr[idx].angle >> 4) * 2) + 1];
  b = arr[idx].vx;
  arr[idx].out0 = ((int) (((((s64) a) * b) + 0x800) >> 0xc)) + 0xe0000;
  a = SINE_TABLE[(arr[idx].angle >> 4) * 2];
  b = arr[idx].vx;
  arr[idx].out4 = ((int) (((((s64) a) * b) + 0x800) >> 0xc)) + 0x66000;
  if (arr[idx].vx > 0x18000)
  {
    arr[idx].vx = arr[idx].vx - 0x10;
  }
  arr[idx].angle += 0x200;
}
