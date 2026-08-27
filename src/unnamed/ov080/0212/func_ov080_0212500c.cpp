//cpp
struct dCc_c { void Clear(); void Update(); };
struct dBgCh_Actr { int JustHitGround() const; int IsOnGround() const; };
struct dActor_c { void UpdatePos(dCc_c *cc); };

extern "C" void dBgCh_Actr_UpdateDiscreteNoLava_veneer(dBgCh_Actr *w);
extern "C" void func_ov080_02124acc(char *c);

extern "C" int func_ov080_0212500c(char *c)
{
    dBgCh_Actr_UpdateDiscreteNoLava_veneer((dBgCh_Actr *)(c + 0x180));
    if (((dBgCh_Actr *)(c + 0x180))->JustHitGround()) {
        int v = *(int *)(c + 0xa8) * -0x3c;
        *(int *)(c + 0xa8) = v / 100;
    } else if (((dBgCh_Actr *)(c + 0x180))->IsOnGround()) {
        *(int *)(c + 0xa8) = 0xc000;
    }
    ((dActor_c *)c)->UpdatePos((dCc_c *)(c + 0x14c));
    func_ov080_02124acc(c);
    ((dCc_c *)(c + 0x14c))->Clear();
    ((dCc_c *)(c + 0x14c))->Update();
    return 1;
}
