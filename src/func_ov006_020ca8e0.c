//cpp
// NONMATCHING: base materialization / addressing (div=27). Logic verified correct vs ROM; not
// byte-matchable from C at mwccarm 1.2/sp2p3 (see notes/matching-style.md).
// Counts as decompiled, not matched.
typedef struct Elem {
  char pad[0x70];
  int f70;
  int f74;
  char pad2[0xdc - 0x78];
} Elem;

extern int data_ov006_021405b8;
extern int data_ov006_021405bc;
extern Elem* data_ov006_02140554;
extern int data_ov006_0213b104;   /* [0] and [4] read */
extern int data_ov006_021405a4;

extern void ApproachLinear(int& ref, int to, int step);
extern "C" void func_ov006_020ca374(char* o, short v);
extern "C" void func_ov006_020c8ecc(char* o);

extern "C" void func_ov006_020ca8e0(void) {
  int i;
  int count;
  Elem* e;
  ApproachLinear(data_ov006_021405bc, data_ov006_021405b8, 1);
  i = 0;
  count = data_ov006_021405bc;
  if (count <= 0)
    return;
  e = data_ov006_02140554;
  do {
    if (e->f70 == (&data_ov006_0213b104)[0] &&
        (e->f74 == (&data_ov006_0213b104)[1] || e->f70 == 0)) {
      func_ov006_020ca374((char*)data_ov006_02140554 + i * 0xdc, (short)(i * 0x14));
      *(int*)((char*)data_ov006_02140554 + i * 0xdc + 0x24) = 0;
      *(int*)((char*)data_ov006_02140554 + i * 0xdc + 0x3c) =
        (int)(((long long)data_ov006_021405a4 * ((i << 11) - 0x1000) + 0x800) >> 12);
      func_ov006_020c8ecc((char*)data_ov006_02140554 + i * 0xdc);
      return;
    }
    i++;
    e++;
  } while (i < count);
}
