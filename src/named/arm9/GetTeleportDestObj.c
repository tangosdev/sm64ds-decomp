struct Obj { int a; int b; };
extern struct Obj* data_0209f330;
struct Obj* GetTeleportDestObj(int i) { return data_0209f330 + i; }
