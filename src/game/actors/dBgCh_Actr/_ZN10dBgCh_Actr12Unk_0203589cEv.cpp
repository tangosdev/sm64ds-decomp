//cpp
extern "C" void func_02037b5c(void* p);

struct dBgCh_Actr
{
    char pad[0x20];
    void Unk_0203589c();
};

void dBgCh_Actr::Unk_0203589c()
{
    func_02037b5c((char*)this + 0x20);
}
