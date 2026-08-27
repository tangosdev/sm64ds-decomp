/* func_0206e4a4 @ 0x0206e4a4 (arm9, size 0x83c)
 * MSL __pformatter: the printf formatting engine. Walks the format string,
 * hands each %-specifier to parse_format (func_0206fd6c), pulls the matching
 * argument off the va_list, renders it with the long/float/hex-float helpers
 * and emits the result through the caller's WriteProc, applying field width,
 * justification and fill. Returns the character count, or -1 if WriteProc fails.
 */
typedef unsigned int size_t;

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

extern unsigned short data_0209a5e4[];
extern char data_0209a5e8[];

extern char *_ZN4cstd6strchrEPKcc(const char *s, int c);
extern int _ZN4cstd6strlenEPKc(const char *s);
extern unsigned char *func_0206fd6c(const unsigned char *format_string, char **arg, print_format *format);
extern char *func_0206f820(long long num, char *buff_end, print_format format);
extern char *func_0206fb08(long num, char *buff_end, print_format format);
extern char *func_0206ece0(long double num, char *buff_end, print_format format);
extern char *func_0206f46c(long double num, char *buff_end, print_format format);
extern int func_0206e184(char *dst, unsigned short *src, unsigned int max);
extern char *func_0206e2cc(char *p, int c, unsigned int n);

#define MAX_BUFF 512

int func_0206e4a4(int (*WriteProc)(void *, const char *, size_t), void *WriteProcArg,
                  const unsigned char *format_str, char *arg)
{
    int num_chars;
    long long long_long_num;
    const unsigned char *curr_format;
    int n;
    int chars_written;
    long long_num;
    long double long_double_num;
    unsigned char *buff_ptr;
    unsigned char fill_char = ' ';
    char blank;
    print_format format;
    char conv_buff[MAX_BUFF];

    num_chars = 0;

    while (*format_str) {
        curr_format = (unsigned char *)_ZN4cstd6strchrEPKcc((const char *)format_str, '%');

        if (curr_format == 0) {
            num_chars += (n = _ZN4cstd6strlenEPKc((const char *)format_str));
            if (n && !(*WriteProc)(WriteProcArg, (const char *)format_str, n))
                return -1;
            break;
        }

        num_chars += (n = curr_format - format_str);
        if (n && !(*WriteProc)(WriteProcArg, (const char *)format_str, n))
            return -1;

        format_str = func_0206fd6c(curr_format, &arg, &format);

        switch (format.conversion_char) {
        case 'd':
        case 'i':
            if (format.argument_options == long_argument)
                long_num = *(long *)((arg += sizeof(long)) - sizeof(long));
            else if (format.argument_options == long_long_argument)
                long_long_num = *(long long *)((arg += sizeof(long long)) - sizeof(long long));
            else if (format.argument_options == intmax_argument)
                long_num = (long)*(long long *)((arg += sizeof(long long)) - sizeof(long long));
            else if (format.argument_options == size_t_argument)
                long_num = *(long *)((arg += sizeof(long)) - sizeof(long));
            else if (format.argument_options == ptrdiff_argument)
                long_num = *(long *)((arg += sizeof(long)) - sizeof(long));
            else
                long_num = *(int *)((arg += sizeof(int)) - sizeof(int));

            if (format.argument_options == short_argument)
                long_num = (short)long_num;
            if (format.argument_options == char_argument)
                long_num = (signed char)long_num;

            if (format.argument_options == long_long_argument) {
                if (!(buff_ptr = (unsigned char *)func_0206f820(long_long_num, conv_buff + MAX_BUFF, format)))
                    goto invalid_conversion;
            } else {
                if (!(buff_ptr = (unsigned char *)func_0206fb08(long_num, conv_buff + MAX_BUFF, format)))
                    goto invalid_conversion;
            }
            n = (unsigned char *)&conv_buff[MAX_BUFF - 1] - buff_ptr;
            break;

        case 'o':
        case 'u':
        case 'x':
        case 'X':
            if (format.argument_options == long_argument)
                long_num = *(long *)((arg += sizeof(long)) - sizeof(long));
            else if (format.argument_options == long_long_argument)
                long_long_num = *(long long *)((arg += sizeof(long long)) - sizeof(long long));
            else if (format.argument_options == intmax_argument)
                long_num = (long)*(long long *)((arg += sizeof(long long)) - sizeof(long long));
            else if (format.argument_options == size_t_argument)
                long_num = *(long *)((arg += sizeof(long)) - sizeof(long));
            else if (format.argument_options == ptrdiff_argument)
                long_num = *(long *)((arg += sizeof(long)) - sizeof(long));
            else
                long_num = *(int *)((arg += sizeof(int)) - sizeof(int));

            if (format.argument_options == short_argument)
                long_num = (unsigned short)long_num;
            if (format.argument_options == char_argument)
                long_num = (unsigned char)long_num;

            if (format.argument_options == long_long_argument) {
                if (!(buff_ptr = (unsigned char *)func_0206f820(long_long_num, conv_buff + MAX_BUFF, format)))
                    goto invalid_conversion;
            } else {
                if (!(buff_ptr = (unsigned char *)func_0206fb08(long_num, conv_buff + MAX_BUFF, format)))
                    goto invalid_conversion;
            }
            n = (unsigned char *)&conv_buff[MAX_BUFF - 1] - buff_ptr;
            break;

        case 'f':
        case 'F':
        case 'e':
        case 'E':
        case 'g':
        case 'G':
            if (format.argument_options == long_double_argument)
                long_double_num = *(long double *)((arg += sizeof(long double)) - sizeof(long double));
            else
                long_double_num = *(double *)((arg += sizeof(double)) - sizeof(double));
            if (!(buff_ptr = (unsigned char *)func_0206ece0(long_double_num, conv_buff + MAX_BUFF, format)))
                goto invalid_conversion;
            n = (unsigned char *)&conv_buff[MAX_BUFF - 1] - buff_ptr;
            break;

        case 'a':
        case 'A':
            if (format.argument_options == long_double_argument)
                long_double_num = *(long double *)((arg += sizeof(long double)) - sizeof(long double));
            else
                long_double_num = *(double *)((arg += sizeof(double)) - sizeof(double));
            if (!(buff_ptr = (unsigned char *)func_0206f46c(long_double_num, conv_buff + MAX_BUFF, format)))
                goto invalid_conversion;
            n = (unsigned char *)&conv_buff[MAX_BUFF - 1] - buff_ptr;
            break;

        case 's':
            if (format.argument_options == wchar_argument) {
                unsigned short *ws = *(unsigned short **)((arg += sizeof(void *)) - sizeof(void *));
                if (ws == 0)
                    ws = data_0209a5e4;
                if (func_0206e184(conv_buff, ws, MAX_BUFF) < 0)
                    goto invalid_conversion;
                buff_ptr = (unsigned char *)conv_buff;
            } else {
                buff_ptr = *(unsigned char **)((arg += sizeof(void *)) - sizeof(void *));
            }
            if (buff_ptr == 0)
                buff_ptr = (unsigned char *)data_0209a5e8;
            if (format.alternate_form) {
                n = *buff_ptr++;
                if (format.precision_specified && n > format.precision)
                    n = format.precision;
            } else if (format.precision_specified) {
                unsigned char *e;
                n = format.precision;
                if ((e = (unsigned char *)func_0206e2cc((char *)buff_ptr, 0, n)) != 0)
                    n = e - buff_ptr;
            } else {
                n = _ZN4cstd6strlenEPKc((const char *)buff_ptr);
            }
            break;

        case 'n': {
            void *p = *(void **)((arg += sizeof(void *)) - sizeof(void *));
            switch (format.argument_options) {
            case normal_argument:
                *(int *)p = num_chars;
                break;
            case short_argument:
                *(short *)p = num_chars;
                break;
            case long_argument:
                *(long *)p = num_chars;
                break;
            case long_long_argument:
                *(long long *)p = num_chars;
                break;
            }
            continue;
        }

        case 'c':
            conv_buff[0] = (char)*(int *)((arg += sizeof(int)) - sizeof(int));
            buff_ptr = (unsigned char *)conv_buff;
            n = 1;
            break;

        case '%':
            conv_buff[0] = '%';
            buff_ptr = (unsigned char *)conv_buff;
            n = 1;
            break;

        case 0xFF:
        default:
        invalid_conversion:
            num_chars += (n = _ZN4cstd6strlenEPKc((const char *)curr_format));
            if (n && !(*WriteProc)(WriteProcArg, (const char *)curr_format, n))
                return -1;
            return num_chars;
        }

        chars_written = n;

        if (format.justification_options != left_justification) {
            fill_char = (format.justification_options == zero_fill) ? '0' : ' ';

            if ((*buff_ptr == '+' || *buff_ptr == '-' || *buff_ptr == ' ') && fill_char == '0') {
                if (!(*WriteProc)(WriteProcArg, (const char *)buff_ptr, 1))
                    return -1;
                buff_ptr++;
                n--;
            }

            while (chars_written < format.field_width) {
                if (!(*WriteProc)(WriteProcArg, (const char *)&fill_char, 1))
                    return -1;
                chars_written++;
            }
        }

        if (n && !(*WriteProc)(WriteProcArg, (const char *)buff_ptr, n))
            return -1;

        if (format.justification_options == left_justification) {
            while (chars_written < format.field_width) {
                blank = ' ';
                if (!(*WriteProc)(WriteProcArg, &blank, 1))
                    return -1;
                chars_written++;
            }
        }

        num_chars += chars_written;
    }

    return num_chars;
}
