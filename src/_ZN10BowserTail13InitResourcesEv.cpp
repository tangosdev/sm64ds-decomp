//cpp
extern "C" {
int _ZN18MovingCylinderClsn4InitEP5Actor5Fix12IiES3_jj(void *self, void *actor, int fix, int t, unsigned int e, unsigned int f);
}

struct BowserTail {
    int InitResources();
};

int BowserTail::InitResources()
{
    char * c = (char *)this;
_ZN18MovingCylinderClsn4InitEP5Actor5Fix12IiES3_jj(c + 0xd4, c, 0x32000, 0x50000, 0x800000, 0x1000);
    return 1;

}
