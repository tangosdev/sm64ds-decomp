//cpp
extern "C" {
extern int _ZN5Model6RenderEPK7Vector3(void*, void*);
}

struct RollingLogTtm {
    int Render();
};

int RollingLogTtm::Render()
{
    char* c = (char*)this;
int b = (*(int*)(c+0xb0) & 0x40000) != 0;
  if(b) return 1;
  _ZN5Model6RenderEPK7Vector3(c+0xd4, 0);
  return 1;

}
