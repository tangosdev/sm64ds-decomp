typedef int Fix12i;
struct Sub { char pad[0x98]; int f98; };
struct C { char pad[0x98]; int f98; char pad2[0x484-0x98-4]; struct Sub* p484; };

void func_ov063_0211a030(struct C* c, int a, int b)
{
    int h = b >> 1;
    int pv = c->p484->f98;
    int m = (int)(((long long)a * h + 0x800) >> 12);
    if (pv < m) {
        c->f98 = m;
        return;
    }
    c->f98 = (int)(((long long)pv * h + 0x800) >> 12);
}
