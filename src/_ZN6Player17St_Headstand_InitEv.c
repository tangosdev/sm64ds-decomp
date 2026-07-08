struct Camera;
extern int _ZN6Player7SetAnimEji5Fix12IiEj(void *c, unsigned int a, int b, int f, unsigned int d);
extern void func_0200d580(struct Camera *thiz, int playerID);
extern struct Camera *CAMERA;
int _ZN6Player17St_Headstand_InitEv(void *c){
  _ZN6Player7SetAnimEji5Fix12IiEj(c, 0x1e, 0x40000000, 0x1000, 0);
  *(unsigned char*)((char*)c+0x6e3)=2;
  *(int*)((char*)c+0x9c)=0;
  *(int*)((char*)c+0x98)=0;
  *(int*)((char*)c+0xa8)=0;
  func_0200d580(CAMERA, *(unsigned char*)((char*)c+0x6d8));
  *(unsigned char*)((char*)c+0x717)=1;
  return 1;
}
