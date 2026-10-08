/* func_0206f46c @ 0x0206f46c (arm9, size 0x3b4)   [mwccarm 2004/b56]
 * MSL printf helper hex_double2str: formats a double backwards into buff_end
 * as the C99 %a/%A hexadecimal form "0x1.<hex digits>p<exp>", handling
 * zero/inf/nan, sign and alternate form. Sibling of float2str (func_0206ece0),
 * which does %e/%f/%g. Returns a pointer to the first character, or NULL if the
 * requested precision would exceed 509 characters.
 */
typedef struct {
    unsigned char justification_options;
    unsigned char sign_options;
    unsigned char precision_specified;
    unsigned char alternate_form;
    unsigned char argument_options;
    unsigned char conversion_char;
    char fill_char;
    char pad7;
    int field_width;
    int precision;
} print_format;

typedef struct {
    char style;
    char unused;
    short digits;
} decform;

typedef struct {
    unsigned char sgn;
    unsigned char unused;
    short exp;
    struct {
        unsigned char length;
        unsigned char text[36];
    } sig;
} decimal;

enum { only_minus = 0, sign_always = 1, space_holder = 2 };
enum { left_justification = 0, right_justification = 1, zero_fill = 2 };

extern void func_02070b98(decform *f, double x, decimal *d); /* __num2dec */
extern double func_020707cc(double x);                       /* fabs */
extern char *func_020706b0(char *dst, const char *src);      /* strcpy */
extern char *func_0206fb08(long num, char *buff_end, print_format format);

extern char data_0209a5ec[]; /* "-INF" */
extern char data_0209a5f4[]; /* "-inf" */
extern char data_0209a5fc[]; /* "INF" */
extern char data_0209a600[]; /* "inf" */
extern char data_0209a604[]; /* "-NAN" */
extern char data_0209a60c[]; /* "-nan" */
extern char data_0209a614[]; /* "NAN" */
extern char data_0209a618[]; /* "nan" */
extern char data_0209a61c[]; /* "0x0p0" */

char *func_0206f46c(double x, char *buff_end, print_format format)
{
    decform form;
    print_format exp_format;
    decimal dec;
    char *p;
    unsigned char digit;
    unsigned long exponent;
    int prec;
    int sign_opt;
    int alt;
    int code;
    int i;
    unsigned char *q;
    unsigned char *r;
    int n;
    int bitpos;

    prec = format.precision;
    code = format.conversion_char;
    alt = format.alternate_form;
    sign_opt = format.sign_options;

    if (prec > 509)
        return 0;

    form.style = 0;
    form.digits = 32;

    func_02070b98(&form, x, &dec);

    if (func_020707cc(x) == 0.0) {
        p = buff_end - 6;
        func_020706b0(p, data_0209a61c);
        return p;
    }

    if (dec.sig.text[0] == 'I') {
        if (dec.sgn) {
            p = buff_end - 5;
            if (code == 'A')
                func_020706b0(p, data_0209a5ec);
            else
                func_020706b0(p, data_0209a5f4);
        } else {
            p = buff_end - 4;
            if (code == 'A')
                func_020706b0(p, data_0209a5fc);
            else
                func_020706b0(p, data_0209a600);
        }
        return p;
    } else if (dec.sig.text[0] == 'N') {
        if (dec.sgn) {
            p = buff_end - 5;
            if (code == 'A')
                func_020706b0(p, data_0209a604);
            else
                func_020706b0(p, data_0209a60c);
        } else {
            p = buff_end - 4;
            if (code == 'A')
                func_020706b0(p, data_0209a614);
            else
                func_020706b0(p, data_0209a618);
        }
        return p;
    }

    exp_format.justification_options = right_justification;
    exp_format.sign_options = sign_always;
    exp_format.precision_specified = 0;
    exp_format.alternate_form = 0;
    exp_format.argument_options = 0;
    exp_format.field_width = 0;
    exp_format.precision = 1;
    exp_format.conversion_char = 'd';

    q = (unsigned char *)&x;

    for (i = 0; i < 4; i++) {
        digit = q[i];
        q[i] = q[7 - i];
        q[7 - i] = digit;
    }

    exponent = (((unsigned char *)&x)[0] << 25 | ((unsigned char *)&x)[1] << 17);
    exponent = (exponent >> 21) & 0x7ff;

    p = func_0206fb08(exponent - 1023, buff_end, exp_format);

    if (code == 'a')
        *--p = 'p';
    else
        *--p = 'P';

    bitpos = prec * 4 + 11;
    r = (unsigned char *)&x;

    for (n = prec; n >= 1; n--) {
        if (bitpos < 64) {
            digit = r[bitpos >> 3] >> (7 - (bitpos & 7));

            if ((bitpos & ~7) != ((bitpos - 4) & ~7))
                digit |= (r[(bitpos >> 3) - 1] << 8) >> (7 - (bitpos & 7));

            digit &= 0xf;

            if (digit < 10)
                digit += '0';
            else if (code == 'a')
                digit += 'a' - 10;
            else
                digit += 'A' - 10;
        } else {
            digit = '0';
        }

        *--p = digit;
        bitpos -= 4;
    }

    if (prec || alt)
        *--p = '.';

    *--p = '1';

    if (code == 'a')
        *--p = 'x';
    else
        *--p = 'X';

    *--p = '0';

    if (dec.sgn)
        *--p = '-';
    else if (sign_opt == sign_always)
        *--p = '+';
    else if (sign_opt == space_holder)
        *--p = ' ';

    return p;
}
