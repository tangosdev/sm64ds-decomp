enum { false, true };

extern void func_ov098_02138b28(void *c, int i);
extern void _ZN12CylinderClsn5ClearEv(void *c);
extern void func_ov098_02139850(void *self);
extern int _ZN16MeshColliderBase9IsEnabledEv(void *c);
extern void _ZN16MeshColliderBase7DisableEv(void *c);

void func_ov098_02138238(void *thiz)
{
  char *c = (char *)thiz;
  unsigned int flags = *(unsigned int *)(c + 0xb0);
  int t1;
  t1 = flags & 0x80000;
  t1 = t1 != 0;

  if (t1 != false) {
    func_ov098_02138b28(c, 3);
  } else {
    int t2;
    t2 = flags & 0x20000;
    t2 = t2 != 0;
    if (t2 == false) {
      int t3;
      t3 = flags & 0x40000;
      t3 = t3 != 0;
      if (t3 == false) {
        func_ov098_02138b28(c, 4);
      }
    }
  }

  {
    unsigned int flags2 = *(unsigned int *)(c + 0xb0);
    int t4;
    t4 = flags2 & 0x20000;
    t4 = t4 != 0;
    if (t4 == false) {
      void *p = *(void **)(c + 0xd0);
      if (p != 0) {
        int *src = (int *)(int)(((long long)(int)((char *)p + 0x5c)) & 0xFFFFFFFFFFFFFFFFLL);
        *(int *)(c + 0x5c) = src[0];
        *(int *)(c + 0x60) = src[1];
        *(int *)(c + 0x64) = src[2];
      }
    }
  }

  _ZN12CylinderClsn5ClearEv(c + 0x564);
  func_ov098_02139850(c);
  if (!_ZN16MeshColliderBase9IsEnabledEv(c + 0x124)) return;
  _ZN16MeshColliderBase7DisableEv(c + 0x124);
}
