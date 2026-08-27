struct Pair { unsigned short v; unsigned short pad; };
extern struct Pair data_0208eeec[];
extern struct Pair data_0208eeee[];

int func_ov007_020b8ec0(int id)
{
    int i = 0;
    for (;;) {
        int v_i = data_0208eeec[i].v;
        int v_i1 = data_0208eeec[i + 1].v;
        if (v_i <= id && id < v_i1) {
            return data_0208eeee[i].v + (id - v_i);
        }
        if (v_i1 >= 0x1f40) {
            return data_0208eeee[i + 1].v + (id - v_i1);
        }
        i++;
    }
}
