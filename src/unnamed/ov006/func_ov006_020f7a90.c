extern int data_ov006_0213d570;
extern int data_ov006_0213d574;
extern int data_ov006_021424f4;
extern int data_ov006_021424fc;
extern int data_ov006_02142508;

int func_ov006_020f7a90(void)
{
    int ret = 0;
    short t;
    short f;
    f = (short)(data_ov006_0213d574 >> 12);
    t = (short)data_ov006_0213d570;
    if (t > 20)
        t = 20;
    if (f == t)
    {
        if (data_ov006_021424f4 == 0)
        {
            if (data_ov006_021424fc == 0 || data_ov006_02142508 == 0)
                ret = 1;
        }
    }
    return ret;
}
