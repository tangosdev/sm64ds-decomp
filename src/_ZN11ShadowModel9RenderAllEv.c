extern void* data_0209cef4[];
extern int data_020a0e68[];
extern int data_0209b3ec[];
extern unsigned char data_0209ceec[];
extern void func_0203c178(int* m, int a, int b, int c);
extern void MulMat4x3Mat4x3(int* dst, int* a, int* b);
extern void func_02046120(int* model, unsigned int b);
extern void _ZN15ModelComponents6RenderEP9Matrix4x3P7Vector3(int* model, int* m, int* v);
extern void func_02046088(int* model, unsigned int b, int f);

void _ZN11ShadowModel9RenderAllEv(void){
  char* node = (char*)data_0209cef4[0];
  if(node){
    do {
      int* model = *(int**)(node+8);
      func_0203c178(data_020a0e68, *(int*)(node+0x10), *(int*)(node+0x14), *(int*)(node+0x18));
      MulMat4x3Mat4x3(data_020a0e68, *(int**)(node+0xc), data_020a0e68);
      data_020a0e68[10] += 0x2000;
      MulMat4x3Mat4x3(data_020a0e68, data_0209b3ec, data_020a0e68);
      func_02046120(model, *(unsigned char*)(node+0x1c));
      _ZN15ModelComponents6RenderEP9Matrix4x3P7Vector3(model, data_020a0e68, 0);
      func_02046088(model, *(unsigned char*)(node+0x1c), 1);
      _ZN15ModelComponents6RenderEP9Matrix4x3P7Vector3(model, data_020a0e68, 0);
      node = *(char**)(node+0x24);
    } while(node);
  }
  data_0209ceec[0] = 1;
}
