struct Obj { int a; int b; };
extern struct Obj* G;
struct Obj* GetTeleportDestObj(int i) { return G + i; }
