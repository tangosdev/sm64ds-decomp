extern void* data_0209cef4[];
extern int MATRIX_SCRATCH_PAPER[];
extern int VIEW_MATRIX_ASR_3[];
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
      func_0203c178(MATRIX_SCRATCH_PAPER, *(int*)(node+0x10), *(int*)(node+0x14), *(int*)(node+0x18));
      MulMat4x3Mat4x3(MATRIX_SCRATCH_PAPER, *(int**)(node+0xc), MATRIX_SCRATCH_PAPER);
      MATRIX_SCRATCH_PAPER[10] += 0x2000;
      MulMat4x3Mat4x3(MATRIX_SCRATCH_PAPER, VIEW_MATRIX_ASR_3, MATRIX_SCRATCH_PAPER);
      func_02046120(model, *(unsigned char*)(node+0x1c));
      _ZN15ModelComponents6RenderEP9Matrix4x3P7Vector3(model, MATRIX_SCRATCH_PAPER, 0);
      func_02046088(model, *(unsigned char*)(node+0x1c), 1);
      _ZN15ModelComponents6RenderEP9Matrix4x3P7Vector3(model, MATRIX_SCRATCH_PAPER, 0);
      node = *(char**)(node+0x24);
    } while(node);
  }
  data_0209ceec[0] = 1;
}
