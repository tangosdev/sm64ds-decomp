struct Obj;
typedef void (*VFN)(struct Obj*);
struct Obj { VFN* vt; };
extern struct Obj* data_0209d4a8;
extern unsigned char data_0209d464;
void func_02019100(void){
  struct Obj* o = data_0209d4a8;
  if (o) o->vt[3](o);
  data_0209d464 = 0;
}
