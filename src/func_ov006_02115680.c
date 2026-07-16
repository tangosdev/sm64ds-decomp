void func_ov006_02115680(char *p, int idx)
{
    int i;
    if (*(int *)(p + idx * 4 + 0x4784) == 0)
    {
        i = 0;
        if (*(int *)(p + 0x4674) > 0)
        {
            do
            {
                short *x;
                if (i >= *(int *)(p + 0x4674))
                    x = 0;
                else
                    x = ((short **)(p + 0x4740))[i];
                i++;
                x[0x1a] = -x[0x1a];
            } while (i < *(int *)(p + 0x4674));
        }
    }
    idx = (int)(((long long)idx) & 0xFFFFFFFFFFFFFFFFLL);
    *(int *)(p + idx * 4 + 0x4784) = 0x14;
}
