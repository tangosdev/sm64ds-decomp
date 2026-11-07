/* fn_023886C0: stage an address/size pair, report errors. */

extern int word_3809424[];

extern int fn_023883FC(unsigned short, unsigned short);

int fn_023886C0(int a1, int a2)
{
    if (a1 == 0)
        return fn_023883FC(3, 2);
    if (a2 == 0)
        return fn_023883FC(3, 2);
    word_3809424[9] = a1;
    word_3809424[10] = a2;
    return fn_023883FC(3, 0);
}
