typedef unsigned int u32;

extern u32 _ZN3IRQ7DisableEv(void);
extern void _ZN3IRQ7RestoreEj(u32 state);
extern void func_0205cd5c(void *arg0, int arg1);
extern void func_020421b4(void);
extern void func_02065170(void);

extern int data_020a0f84;
extern char data_020a1fc0[];

typedef void (*CallbackFn)(void *ctx, int zero, int val);

void func_02040aa4(void)
{
  char *g = data_020a1fc0;
  u32 saved0 = _ZN3IRQ7DisableEv();

  if (data_020a0f84 != 0) {
    int cb1 = *(int *)(g + 0x20);

    if (cb1 != 0) {
      if (cb1 != 0) {
        u32 saved1 = _ZN3IRQ7DisableEv();
        void *ctxArg = *(void **)(data_020a1fc0 + 0x18);
        CallbackFn fn = *(CallbackFn *)(data_020a1fc0 + 0x14);
        fn(ctxArg, 0, cb1);
        _ZN3IRQ7RestoreEj(saved1);
      }
      *(volatile int *)(g + 0x20) = 0;
      *(volatile int *)(g + 0x24) = 0;
    }

    *(int *)(g + 0xc) = 0;

    if (*(int *)data_020a1fc0 == 0) {
      void *p = *(void **)(g + 0x1c);
      char *g2 = g + 0x440;
      if (p != 0) {
        void *arg = *(void **)((char *)p + 8);
        *(void **)(g + 0x1c) = 0;
        func_0205cd5c(arg, 5);
      }

      {
        int cb2 = *(int *)(g2 + 0xc70);
        if (cb2 != 0) {
          u32 saved2 = _ZN3IRQ7DisableEv();
          void *ctxArg2 = *(void **)(data_020a1fc0 + 0x18);
          CallbackFn fn2 = *(CallbackFn *)(data_020a1fc0 + 0x14);
          fn2(ctxArg2, 0, cb2);
          _ZN3IRQ7RestoreEj(saved2);
        }
      }
    } else {
      func_020421b4();
    }

    func_02065170();
    data_020a0f84 = 0;
  }

  _ZN3IRQ7RestoreEj(saved0);
}
