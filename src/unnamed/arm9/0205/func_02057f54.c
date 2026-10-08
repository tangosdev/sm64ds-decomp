struct Node58;
extern int ARMProcessorMode(void);
extern struct Node58 *func_0205801c(void);
extern int ARMSaveContext(void *ctx);
extern void ARMRestoreContext(void *ctx);

struct CtxState {
  unsigned short m0;
  unsigned short m2;
  unsigned short m4;
  unsigned short m6;
  void *m8;
  int mc;
  void (*m10)(void*, void*);
};
extern struct CtxState data_020a6134;
extern void **data_020a612c;
extern void (*data_020a6128)(void*, void*);

void func_02057f54(void){
  void *r6;
  struct Node58 *r5;
  struct CtxState *s = &data_020a6134;
  if (s->m4 == 0){
    if (ARMProcessorMode() != 0x12) goto cont;
  }
  s->m0 = 1;
  return;
cont:
  r6 = data_020a612c[0];
  r5 = func_0205801c();
  if (r6 == (void*)r5) return;
  if (r5 == 0) return;
  if (ARMSaveContext(r6) != 0) return;
  if (data_020a6128 != 0) data_020a6128(r6, r5);
  if (s->m10 != 0) s->m10(r6, r5);
  data_020a6134.m8 = r5;
  ARMRestoreContext(r5);
}
