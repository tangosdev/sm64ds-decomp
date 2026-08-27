/* func_0206ece0 @ 0x0206ece0 (arm9, size 0x658)
 * MSL printf helper float2str: formats a double backwards into buff_end
 * for %e/%E/%f/%F/%g/%G, handling inf/nan, sign and alternate form.
 * Returns pointer to first character, or NULL if result would exceed 509 chars.
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

extern void func_02070b98(decform *f, int x0, int x1, decimal *d); /* __num2dec */
extern int func_01ff9e2c(int a, int b, int c, int d);              /* double less-than */
extern char *func_020706b0(char *dst, const char *src);           /* strcpy */
extern void func_0206f338(decimal *d, int digits);                /* __round_decimal */
extern unsigned short data_02086a58[];                            /* ctype map */
extern char data_0209a5ec[]; /* "-INF" */
extern char data_0209a5f4[]; /* "-inf" */
extern char data_0209a5fc[]; /* "INF" */
extern char data_0209a600[]; /* "inf" */
extern char data_0209a604[]; /* "-NAN" */
extern char data_0209a60c[]; /* "-nan" */
extern char data_0209a614[]; /* "NAN" */
extern char data_0209a618[]; /* "nan" */

#define ISUPPER(c) (((c) < 0 || (c) >= 0x80) ? 0 : (data_02086a58[c] & 0x200))

char *func_0206ece0(int x0, int x1, char *buff_end, print_format format)
{
    int int_digits;
    int code;
    decform form;
    int prec;
    int sign_opt;
    unsigned char *q;
    int power;
    int n;
    char sign;
    int alt;
    int digits;
    decimal dec;
    volatile unsigned char *p;

    form.style = 0;
    form.digits = 32;

    code = format.conversion_char;
    prec = (int)format.precision;
    alt = format.alternate_form;
    sign_opt = format.sign_options;

    func_02070b98(&form, x0, x1, &dec);

    p = dec.sig.text + dec.sig.length; /* strip off trailing zeroes */

    while (dec.sig.length > 1 && *--p == '0') {
        --dec.sig.length;
        ++dec.exp;
    }

    switch (*dec.sig.text) {
    case '0':
        dec.exp = 0; /* kill spurious exponent */
        break;

    case 'I':
        if (func_01ff9e2c(x0, x1, 0, 0)) {
            p = (unsigned char *)buff_end - 5;
            if (ISUPPER(code))
                func_020706b0((char *)p, data_0209a5ec);
            else
                func_020706b0((char *)p, data_0209a5f4);
        } else {
            p = (unsigned char *)buff_end - 4;
            if (ISUPPER(code))
                func_020706b0((char *)p, data_0209a5fc);
            else
                func_020706b0((char *)p, data_0209a600);
        }
        return (char *)p;

    case 'N':
        if (dec.sgn) {
            p = (unsigned char *)buff_end - 5;
            if (ISUPPER(code))
                func_020706b0((char *)p, data_0209a604);
            else
                func_020706b0((char *)p, data_0209a60c);
        } else {
            p = (unsigned char *)buff_end - 4;
            if (ISUPPER(code))
                func_020706b0((char *)p, data_0209a614);
            else
                func_020706b0((char *)p, data_0209a618);
        }
        return (char *)p;
    }

    dec.exp += dec.sig.length - 1; /* shift decimal point to right of sig */

    p = (unsigned char *)buff_end;
    *--p = 0;

    switch (code) {
    case 'g':
    case 'G':
        if (dec.sig.length > prec)
            func_0206f338(&dec, prec);

        if (dec.exp < -4 || dec.exp >= prec) {
            if (alt)
                --prec;
            else
                prec = dec.sig.length - 1;

            if (code == 'g')
                code = 'e';
            else
                code = 'E';

            goto e_format;
        }

        if (alt)
            prec -= dec.exp + 1;
        else if ((prec = dec.sig.length - (dec.exp + 1)) < 0)
            prec = 0;

        goto f_format;

    case 'e':
    case 'E':
    e_format:
        if (dec.sig.length > prec + 1)
            func_0206f338(&dec, prec + 1);

        power = dec.exp;
        sign = '+';

        if (power < 0) {
            power = -power;
            sign = '-';
        }

        for (n = 0; power || n < 2; ) {
            *--p = power % 10 + '0';
            power /= 10;
            ++n;
        }

        *--p = sign;
        *--p = code;

        if (prec + (buff_end - (char *)p) > 509)
            return 0;

        if (dec.sig.length < prec + 1) {
            n = prec + 2 - dec.sig.length;

            while (--n)
                *--p = '0';
        }

        q = dec.sig.text + dec.sig.length;
        n = dec.sig.length;

        while (--n)
            *--p = *--q;

        if (prec || alt)
            *--p = '.';

        *--p = *dec.sig.text;

        if (dec.sgn)
            *--p = '-';
        else if (sign_opt == sign_always)
            *--p = '+';
        else if (sign_opt == space_holder)
            *--p = ' ';
        break;

    case 'f':
    case 'F':
    f_format:
        if ((digits = dec.sig.length - dec.exp - 1) < 0)
            digits = 0;

        if (digits > prec) {
            func_0206f338(&dec, dec.sig.length - (digits - prec));

            if ((digits = dec.sig.length - dec.exp - 1) < 0)
                digits = 0;
        }

        if ((int_digits = dec.exp + 1) < 0)
            int_digits = 0;

        if (int_digits + digits > 509)
            return 0;

        q = dec.sig.text + dec.sig.length;

        for (n = 0; n < prec - digits; ++n) /* trailing fractional zeroes */
            *--p = '0';

        {
            int i;
            for (i = 0; i < digits && i < dec.sig.length; ++i) /* fractional digits */
                *--p = *--q;

            for (; i < digits; ++i) /* fractional zeroes */
                *--p = '0';
        }

        if (prec || alt)
            *--p = '.';

        if (int_digits) {
            int i;
            for (i = 0; i < int_digits - dec.sig.length; ++i) /* integral zeroes */
                *--p = '0';

            for (; i < int_digits; ++i) /* integral digits */
                *--p = *--q;
        } else
            *--p = '0';

        if (dec.sgn)
            *--p = '-';
        else if (sign_opt == sign_always)
            *--p = '+';
        else if (sign_opt == space_holder)
            *--p = ' ';
        break;
    }

    return (char *)p;
}
