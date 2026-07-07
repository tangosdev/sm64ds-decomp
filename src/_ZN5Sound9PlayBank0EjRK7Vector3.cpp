//cpp
extern "C" {
struct Vector3 { int x,y,z; };
void _ZN5Sound4PlayEjjRK7Vector3(unsigned int bank, unsigned int id, const Vector3* v);
void _ZN5Sound9PlayBank0EjRK7Vector3(unsigned int id, const Vector3* v) {
    _ZN5Sound4PlayEjjRK7Vector3(0, id, v);
}
}