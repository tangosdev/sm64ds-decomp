/* fn_023881AC: dispatch queued commands to their handlers. */

extern int fn_023889BC(int);
extern int fn_0238B644(int);
extern int fn_023895DC(int);
extern int fn_0238C70C(int);

int fn_023881AC(int result, int a2, int a3)
{
    if (a3 == 0) {
        result -= 4;
        switch (result) {
        case 2:
            result = fn_023889BC(a2);
            break;
        case 5:
            result = fn_0238B644(a2);
            break;
        case 4:
            result = fn_023895DC(a2);
            break;
        case 0:
            result = fn_0238C70C(a2);
            break;
        }
    }
    return result;
}
