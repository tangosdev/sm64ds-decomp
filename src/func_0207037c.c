extern int func_020589d4(void* p);
extern void func_02058b08(void* p);
extern void func_02058a94(void* p);
extern int func_0206dbf8(int a);

extern int data_020aa0ac;
extern char* data_020a6134[];
extern struct G { char p[0x1c]; int v; } data_020a9fd8;
extern struct G data_020a9ffc;
extern int data_020aa3d4[];

typedef void (*PFN)(int);

int func_0207037c(int idx)
{
  int r4;
  if (idx < 1 || idx > 7) return -1;
  if (func_020589d4(&data_020aa0ac) == 0) {
    data_020a9fd8.v = *(int*)(data_020a6134[2] + 0x6c);
    data_020a9ffc.v = 1;
  } else if (data_020a9fd8.v == *(int*)(data_020a6134[2] + 0x6c)) {
    data_020a9ffc.v += 1;
  } else {
    func_02058b08(&data_020aa0ac);
    data_020a9fd8.v = *(int*)(data_020a6134[2] + 0x6c);
    data_020a9ffc.v = 1;
  }
  r4 = data_020aa3d4[idx - 1];
  if (r4 != 1) data_020aa3d4[idx - 1] = 0;
  data_020a9ffc.v -= 1;
  if (data_020a9ffc.v == 0) func_02058a94(&data_020aa0ac);
  if (r4 == 1 || (r4 == 0 && idx == 1)) return 0;
  if (r4 == 0) func_0206dbf8(0);
  ((PFN)r4)(idx);
  return 0;
}
