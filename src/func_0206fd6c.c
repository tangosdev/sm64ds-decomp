/* func_0206fd6c @ 0x0206fd6c (arm9, size 0x5dc)
 * MSL printf helper parse_format: parses a %-conversion specification
 * (flags, field width, precision, length modifier, conversion char)
 * into a print_format struct. Returns pointer past the specifier.
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

enum { only_minus = 0, sign_always = 1, space_holder = 2 };
enum { left_justification = 0, right_justification = 1, zero_fill = 2 };
enum {
    normal_argument = 0,
    char_argument,
    short_argument,
    long_argument,
    intmax_argument,
    size_t_argument,
    ptrdiff_argument,
    long_long_argument,
    long_double_argument,
    wchar_argument
};

extern unsigned short data_02086a58[];

#define isdigit_(c) ((c) < 0 || (c) >= 0x80 ? 0 : data_02086a58[c] & 8)

unsigned char *func_0206fd6c(unsigned char *format_string, char **arg, print_format *format)
{
    print_format f;
    unsigned char *s = format_string;
    int c;
    int flag_found;

    f.justification_options = right_justification;
    f.sign_options          = only_minus;
    f.precision_specified   = 0;
    f.alternate_form        = 0;
    f.argument_options      = normal_argument;
    f.field_width           = 0;
    f.precision             = 0;

    s = (unsigned char *)(((long long)(int)(format_string + 1)) & 0xFFFFFFFFFFFFFFFFLL);
    if ((c = *s) == '%') {
        f.conversion_char = (unsigned char)c;
        *format = f;
        return s + 1;
    }

    do {
        flag_found = 1;

        switch (c) {
        case '-':
            f.justification_options = left_justification;
            break;
        case '+':
            f.sign_options = sign_always;
            break;
        case ' ':
            if (f.sign_options != sign_always)
                f.sign_options = space_holder;
            break;
        case '#':
            f.alternate_form = 1;
            break;
        case '0':
            if (f.justification_options != left_justification)
                f.justification_options = zero_fill;
            break;
        default:
            flag_found = 0;
            break;
        }

    } while (flag_found && (c = *++s, 1));

    if (c == '*') {
        *arg += 4;
        if ((f.field_width = *(int *)(*arg - 4)) < 0) {
            f.justification_options = left_justification;
            f.field_width = -f.field_width;
        }
        c = *++s;
    } else {
        while (isdigit_(c)) {
            f.field_width = (f.field_width * 10) + (c - '0');
            c = *++s;
        }
    }

    if (f.field_width > 509) {
        f.conversion_char = 0xFF;
        *format = f;
        return s + 1;
    }

    if (c == '.') {
        f.precision_specified = 1;

        c = *++s;

        if (c == '*') {
            *arg += 4;
            if ((f.precision = *(int *)(*arg - 4)) < 0)
                f.precision_specified = 0;
            c = *++s;
        } else {
            while (isdigit_(c)) {
                f.precision = (f.precision * 10) + (c - '0');
                c = *++s;
            }
        }
    }

    flag_found = 1;

    switch (c) {
    case 'h':
        f.argument_options = short_argument;
        if (*(s + 1) == 'h') {
            f.argument_options = char_argument;
            c = *++s;
        }
        break;
    case 'l':
        f.argument_options = long_argument;
        if (*(s + 1) == 'l') {
            f.argument_options = long_long_argument;
            c = *++s;
        }
        break;
    case 'L':
        f.argument_options = long_double_argument;
        break;
    case 'j':
        f.argument_options = intmax_argument;
        break;
    case 'z':
        f.argument_options = size_t_argument;
        break;
    case 't':
        f.argument_options = ptrdiff_argument;
        break;
    default:
        flag_found = 0;
        break;
    }

    if (flag_found)
        c = *++s;

    f.conversion_char = (unsigned char)c;

    switch (c) {
    case 'd':
    case 'i':
    case 'o':
    case 'u':
    case 'x':
    case 'X':
        if (f.argument_options == long_double_argument) {
            f.conversion_char = 0xFF;
            break;
        }
        if (!f.precision_specified)
            f.precision = 1;
        else if (f.justification_options == zero_fill)
            f.justification_options = right_justification;
        break;

    case 'f':
    case 'F':
        if (f.argument_options == short_argument ||
            (unsigned char)(f.argument_options + 0xFC) <=
                (unsigned char)(long_long_argument - intmax_argument)) {
            f.conversion_char = 0xFF;
            break;
        }
        if (!f.precision_specified)
            f.precision = 6;
        break;

    case 'a':
    case 'A':
        if (!f.precision_specified)
            f.precision = 13;
        if (f.argument_options == short_argument ||
            f.argument_options == intmax_argument ||
            f.argument_options == size_t_argument ||
            f.argument_options == ptrdiff_argument ||
            f.argument_options == long_long_argument ||
            f.argument_options == char_argument)
            f.conversion_char = 0xFF;
        break;

    case 'g':
    case 'G':
        if (f.precision == 0)
            f.precision = 1;
    case 'e':
    case 'E':
        if (f.argument_options == short_argument ||
            f.argument_options == intmax_argument ||
            f.argument_options == size_t_argument ||
            f.argument_options == ptrdiff_argument ||
            f.argument_options == long_long_argument ||
            f.argument_options == char_argument)
            f.conversion_char = 0xFF;
        else if (!f.precision_specified)
            f.precision = 6;
        break;

    case 'p':
        f.conversion_char = 'x';
        f.alternate_form = 1;
        f.argument_options = long_argument;
        f.precision = 8;
        break;

    case 'c':
        if (f.argument_options == long_argument)
            f.argument_options = wchar_argument;
        else if (f.precision_specified || f.argument_options != normal_argument)
            f.conversion_char = 0xFF;
        break;

    case 's':
        if (f.argument_options == long_argument)
            f.argument_options = wchar_argument;
        else if (f.argument_options != normal_argument)
            f.conversion_char = 0xFF;
        break;

    case 'n':
        if (f.argument_options == long_double_argument)
            f.conversion_char = 0xFF;
        break;

    default:
        f.conversion_char = 0xFF;
        break;
    }

    *format = f;
    return s + 1;
}
