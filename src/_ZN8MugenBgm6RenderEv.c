//cpp
extern "C" {
void _ZN9ModelBase12ApplyOpacityEj(void* m, unsigned int op, int z);
void func_ov002_020f65b8(void* o);
}
struct M48 { int w[12]; };
extern M48 data_0209b41c;
extern int data_ov002_0210bb7c[];
struct ModelBase {
  virtual void v0(); virtual void v1(); virtual void v2();
  virtual void v3(); virtual void v4(); virtual void m(int arg);
};
extern "C" int _ZN8MugenBgm6RenderEv(char* c){
  if (*(int*)(c + 8) == 0x19){
    struct { char* p; char* cur; M48* src; } s;
    s.src = &data_0209b41c;
    s.p = *(char**)(c + 0xdc) + 0x1c;
    s.cur = s.p;
    *(M48*)s.cur = *s.src;
    int* tbl = data_ov002_0210bb7c;
    int i = 0;
    int zero = 0;
    do {
      *(int*)(s.p + 0x24) = tbl[0];
      *(int*)(s.p + 0x28) = tbl[1];
      *(int*)(s.p + 0x2c) = tbl[2];
      ((ModelBase*)*(void**)(c + 0xdc))->m(zero);
      tbl += 3;
      i++;
    } while ((unsigned)i < 3u);
    return 1;
  }
  unsigned char op = *(unsigned char*)(c + 0x102);
  if (op == 0) return 1;
  {
    void* a = *(void**)(c + 0xdc);
    if (a != 0){
      a = (void*)((int)a);
      _ZN9ModelBase12ApplyOpacityEj(a, op, 0);
      ((ModelBase*)*(void**)(c + 0xdc))->m((int)(c + 0x80));
    } else {
      void* b = *(void**)(c + 0xe0);
      if (b != 0){
        b = (void*)((int)b);
        func_ov002_020f65b8(b);
        _ZN9ModelBase12ApplyOpacityEj(*(void**)(c + 0xe0), *(unsigned char*)(c + 0x102), 0);
        ((ModelBase*)*(void**)(c + 0xe0))->m((int)(c + 0x80));
      }
    }
  }
  return 1;
}
