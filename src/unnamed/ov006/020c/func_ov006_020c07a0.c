extern int func_ov006_020c06dc();
extern int _ZN11ShadowModel9InitModelEP9Matrix4x35Fix12IiES3_S3_j();
extern int _ZN5Model6RenderEPK7Vector3();

void func_ov006_020c07a0(char *t)
{
    func_ov006_020c06dc(t);
    _ZN11ShadowModel9InitModelEP9Matrix4x35Fix12IiES3_S3_j(t + 0x88, t + 0x34, 0x3c000, 0x14000, 0x3c000, 0xc);
    _ZN5Model6RenderEPK7Vector3(t + 0x18, 0);
}
