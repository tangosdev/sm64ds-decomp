extern unsigned int _ZN3IRQ7DisableEv(void);
extern void _ZN3IRQ7RestoreEj(unsigned int savedState);
struct Obj { int field0; int field4; int count; };
extern struct Obj data_020a6134;
int func_020589d4(struct Obj* o){
  unsigned int saved = _ZN3IRQ7DisableEv();
  int g = (&data_020a6134)->count;
  int ret;
  if(o->field4 == 0){
    o->field4 = g;
    *(int *)(((int)o + 8) & 0xFFFFFFFFFFFFFFFF) = *(int *)(((int)o + 8) & 0xFFFFFFFFFFFFFFFF) + 1;
    ret = 1;
  } else if(o->field4 == g){
    *(int *)(((int)o + 8) & 0xFFFFFFFFFFFFFFFF) = *(int *)(((int)o + 8) & 0xFFFFFFFFFFFFFFFF) + 1;
    ret = 1;
  } else {
    ret = 0;
  }
  _ZN3IRQ7RestoreEj(saved);
  return ret;
}
