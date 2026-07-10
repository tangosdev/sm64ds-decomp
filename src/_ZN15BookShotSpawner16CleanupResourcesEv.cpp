//cpp
class SharedFilePtr {
public:
    void Release();
};

extern void UnloadBlueCoinModel(char *c);
extern int data_ov020_02114aa0;
extern int data_ov020_02114ab8;

extern "C" int _ZN15BookShotSpawner16CleanupResourcesEv(char *c) {
    ((SharedFilePtr *)&data_ov020_02114aa0)->Release();
    ((SharedFilePtr *)&data_ov020_02114ab8)->Release();
    UnloadBlueCoinModel(c);
    return 1;
}
