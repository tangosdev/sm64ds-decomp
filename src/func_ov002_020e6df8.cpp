//cpp
typedef int Fix12i;
struct Vector3 { Fix12i x, y, z; };
struct BCA_File;
struct Actor;

struct Anim2 { void* a; void* b; };   // data_02110944: load [4] => second ptr

extern Anim2 data_ov002_02110944;
extern int data_ov002_0210aa0c[3];

extern "C" {
void _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(char* thiz, void* file, int b, Fix12i d, unsigned int e);
void _ZN25MovingCylinderClsnWithPos4InitEP5ActorRK7Vector35Fix12IiES6_jj(char* thiz, char* actor, const Vector3* v, int rad, int e, unsigned int f, unsigned int g);

void func_ov002_020e6df8(char* c)
{
  *(int*)(c + 0x43c) = 9;
  _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(c + 0x30c, data_ov002_02110944.b, 0x40000000, 0x1000, 0);
  _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(c + 0x370, data_ov002_02110944.b, 0x40000000, 0x1000, 0);
  {
    Vector3 v;
    v.x = data_ov002_0210aa0c[0];
    v.y = data_ov002_0210aa0c[1];
    v.z = data_ov002_0210aa0c[2];
    _ZN25MovingCylinderClsnWithPos4InitEP5ActorRK7Vector35Fix12IiES6_jj(
      c + 0x110, c, &v, 0x64000, 0x96000, 1, 0);
  }
  {
    unsigned short* p = (unsigned short*)(((int)c + 0x4a2) & 0xFFFFFFFFFFFFFFFF);
    *p = (*p & ~1) | 1;
    *p = *p | 2;
  }
  *(unsigned char*)(c + 0x49d) = 6;
  *(int*)(c + 0x440) = 6;
}
}
