//cpp
struct OamAttr; struct Matrix2x2;
struct Base {
    virtual int v00(); virtual int v01(); virtual int v02(); virtual int v03();
    virtual int v04(); virtual int v05(); virtual int v06(); virtual int v07();
    virtual int v08(); virtual int v09(); virtual int v10(); virtual int v11();
    virtual int v12(); virtual int v13(); virtual int v14(); virtual int v15();
    virtual int v16(); virtual int v17(); virtual int v18(); virtual int v19();
    virtual int v20(); virtual int v21(); virtual int v22(); virtual int v23();
    virtual int v24(); virtual int v25(); virtual int m();
};
namespace OAM { void Render(bool a, OamAttr *b, int c, int d, int e, int f, Matrix2x2 *g); }
extern "C" Base *data_ov004_020beb68;

extern "C" void func_ov004_020afcf8(void *arg0, void *arg1, int arg2, void *arg3)
{
    Base *g = data_ov004_020beb68;
    if (g == 0) return;
    if (*(int*)((char*)g + 0x4628) == 0 && g->m() == 2) {
        if (*(unsigned short*)((char*)data_ov004_020beb68 + 0x4664) != 1) return;
        OAM::Render(false, (OamAttr*)arg0, (int)arg1, arg2, -1, -1, (Matrix2x2*)arg3);
        return;
    }
    OAM::Render(false, (OamAttr*)arg0, (int)arg1, arg2, -1, -1, (Matrix2x2*)arg3);
}
