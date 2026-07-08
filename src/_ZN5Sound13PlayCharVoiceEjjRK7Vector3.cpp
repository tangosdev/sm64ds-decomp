//cpp
struct Vector3 { int x, y, z; };

extern "C" {

extern unsigned char data_02075250[];

unsigned int _ZN5Sound4PlayEjjRK7Vector3(unsigned int a, unsigned int b, const Vector3 &v);

unsigned int _ZN5Sound13PlayCharVoiceEjjRK7Vector3(unsigned int a, unsigned int b, const Vector3 &v)
{
    return _ZN5Sound4PlayEjjRK7Vector3(1, b + data_02075250[a], v);
}

}
