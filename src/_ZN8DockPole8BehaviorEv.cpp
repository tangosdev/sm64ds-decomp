//cpp
extern "C" {
extern void _ZN9Animation7AdvanceEv(void* c);
extern void Matrix4x3_FromRotationY(void* m, short angle);
}

struct DockPole {
    int Behavior();
};

int DockPole::Behavior()
{
    char* c = (char*)this;
_ZN9Animation7AdvanceEv(c+0x124);
  Matrix4x3_FromRotationY(c+0xf0, *(short*)(c+0x8e));
  *(int*)(c+0x114)=*(int*)(c+0x5c)>>3;
  *(int*)(c+0x118)=*(int*)(c+0x60)>>3;
  *(int*)(c+0x11c)=*(int*)(c+0x64)>>3;
  return 1;

}
