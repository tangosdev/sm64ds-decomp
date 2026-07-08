extern void Scene_AfterRender(void *scene, unsigned int arg);

void func_ov006_020e700c(void *scene, unsigned int arg)
{
    volatile unsigned short *reg = (volatile unsigned short *)0x04000006;
    int v = *reg;

    if (v > 0xb9 && v <= 0xc0) {
        while ((int)*reg < 0xc0) {
        }
    }

    Scene_AfterRender(scene, arg);
}
