typedef struct Decimal
{
  unsigned char sign;
  char unused;
  short exp;
  struct
  {
    unsigned char length;
    unsigned char text[32];
    unsigned char unused;
  } sig;
} Decimal;
void func_02071510(Decimal *result, unsigned long long v)
{
  result->sign = 0;
  result->sig.length = 0;
  if (v != 0)
  {
    do
    {
      result->sig.text[result->sig.length++] = (unsigned char) (v % 10);
      v /= 10;
    }
    while (v != 0);
  }
  {
    unsigned char len = result->sig.length;
    unsigned char *s = result->sig.text;
    unsigned char *e = (s + len) - 1;
    while (s < e)
    {
      unsigned char t = *s;
      *s = *e;
      *e = t;
      s++;
      e--;
    }
  }
  result->exp = (short) (result->sig.length - 1);
}
