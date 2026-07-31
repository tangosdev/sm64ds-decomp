//cpp
extern "C" {
extern void* data_0209cee8;
}

struct CylinderClsn {
    void Update();
};

void CylinderClsn::Update()
{
    char* c = (char*)this;
if(*(int*)(c+0x18) & 1) return;
  void* h = data_0209cee8;
  *(void**)(c+0x2c) = h;
  if(data_0209cee8) *(void**)((char*)data_0209cee8+0x28) = c;
  data_0209cee8 = c;

}
