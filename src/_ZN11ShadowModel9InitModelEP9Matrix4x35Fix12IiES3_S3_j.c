extern unsigned char data_0209ceec[];
extern void* data_0209cef4[];
void _ZN11ShadowModel9InitModelEP9Matrix4x35Fix12IiES3_S3_j(void* c, int a1, int a2, int a3, int a4, unsigned char a5){
  *(int*)((char*)c+0xc) = a1;
  *(int*)((char*)c+0x10) = a2;
  *(int*)((char*)c+0x14) = a3;
  *(int*)((char*)c+0x18) = a4;
  *(unsigned char*)((char*)c+0x1c) = a5;
  if(data_0209ceec[0]) return;
  *(void**)((char*)c+0x24) = *(void* volatile*)&data_0209cef4[0];
  void* head = data_0209cef4[0];
  if(head) *(void**)((char*)head+0x20) = c;
  data_0209cef4[0] = c;
}
