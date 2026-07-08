struct Obj;
typedef void (*VFN)(struct Obj*);
struct Obj { VFN* vt; };
extern struct Obj* SCENE_RELATED;
extern unsigned char data_0209d464;
void func_02019100(void){
  struct Obj* o = SCENE_RELATED;
  if (o) o->vt[3](o);
  data_0209d464 = 0;
}
