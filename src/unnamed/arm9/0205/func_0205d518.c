typedef struct { int a; int b; } Pair;

extern int func_0205d644(Pair *out);
extern int func_0205d568(int arg, Pair p);

int func_0205d518(int arg)
{
    Pair p;
    if (func_0205d644(&p) != 0) {
        if (func_0205d568(arg, p) != 0)
            return 1;
    }
    return 0;
}
