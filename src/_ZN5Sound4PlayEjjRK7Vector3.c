//cpp
// @symbol _ZN5Sound4PlayEjjRK7Vector3
//
// Language-mode flip only: the compiler mangles Sound::Play(...), it is no
// longer spelled by hand. Signature and body preserved; no codegen intent.
// See notes/plan-cpp-language-mode.md phase 1 (layout-free SDK namespaces).
//
// Mangling: Play(unsigned, unsigned, Vector3 const&). Callers that still declare
// this as returning unsigned int are unaffected — return type is not mangled.
struct Vector3 { int x, y, z; };

extern "C" {
extern char* func_02050cdc(void);
extern int func_02048720(const Vector3* v, unsigned int a, unsigned int b);
extern void Player_PlaySoundEffect(int x, unsigned int a, unsigned int b);
extern void func_02048908(int x, const Vector3* v);
extern int func_02048a1c(const Vector3* v, unsigned int a, unsigned int b);
extern void func_02048d80(int* g, const Vector3* v);
extern int data_0209b4a4[];
}

namespace Sound {

void Play(unsigned int j1, unsigned int j2, const Vector3& v)
{
    char* s = func_02050cdc();
    int t = *(unsigned char*)(s + 5);
    if (t == 9 || t == 2) {
        int r = func_02048720(&v, j1, j2);
        if (r == 0)
            return;
        Player_PlaySoundEffect(r, j1, j2);
        func_02048908(r, &v);
        return;
    }
    if (func_02048a1c(&v, j1, j2) == 0)
        return;
    Player_PlaySoundEffect((int)data_0209b4a4, j1, j2);
    func_02048d80(data_0209b4a4, &v);
}

}
