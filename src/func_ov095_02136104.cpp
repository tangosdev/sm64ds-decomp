//cpp
struct Vector3 { int x,y,z; };
extern "C" {
extern void _ZN5Sound9PlayBank3EjRK7Vector3(unsigned int id, Vector3* v);
extern void _ZN5Actor10EarthquakeERK7Vector35Fix12IiE(void* self, Vector3* v, int fix);
void func_ov095_02136104(char* c){
  *(int*)(c+0xa8) = 0;
  if(*(unsigned short*)(c+0x344) == 0){
    _ZN5Sound9PlayBank3EjRK7Vector3(0x6b, (Vector3*)(c+0x74));
    Vector3 v;
    v.x = *(int*)(c+0x5c);
    v.y = *(int*)(c+0x60);
    v.z = *(int*)(c+0x64);
    _ZN5Actor10EarthquakeERK7Vector35Fix12IiE(c, &v, 0x320000);
  }
  if(*(unsigned char*)(c+0x347) == 1) *(int*)(c+0x32c) = 1;
}
}
