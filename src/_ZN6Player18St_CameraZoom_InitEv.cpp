//cpp
struct Camera;
extern "C" {
extern int func_0200d064(struct Camera* thiz, int playerID);
extern void func_0200d7e0(struct Camera* thiz, int playerID);
extern int _ZN6Player7SetAnimEji5Fix12IiEj(void*,unsigned int,int,int,unsigned int);
extern struct Camera* CAMERA;
extern unsigned char data_0209f28c;
int _ZN6Player18St_CameraZoom_InitEv(void* c){
  *(int*)((char*)c+0x98)=0;
  *(int*)((char*)c+0xa8)=0;
  struct Camera* cam = CAMERA;
  int pid = *(unsigned char*)((char*)c+0x6d8);
  func_0200d064(cam, pid);
  func_0200d7e0(cam, *(unsigned char*)((char*)c+0x6d8));
  _ZN6Player7SetAnimEji5Fix12IiEj(c,0x47,0,0x1000,0);
  data_0209f28c=0;
  return 1;
}
}
