//cpp
extern "C" {
void func_02046008(char* p, int id);
}

extern "C" void _ZN5Model12SetPolygonIDEi(char* self, int id)
{
    func_02046008(self + 8, id);
}
