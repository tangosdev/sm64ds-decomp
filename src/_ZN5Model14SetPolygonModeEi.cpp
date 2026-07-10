//cpp
extern "C" {
void func_02046234(char* p, int mode);
}

extern "C" void _ZN5Model14SetPolygonModeEi(char* self, int mode)
{
    func_02046234(self + 8, mode);
}
