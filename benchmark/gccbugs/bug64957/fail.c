

int a;
int b;

int
main ()
{
  a = 0;
  for (; a < 2; a++)
    {
      b ^= 5;
      b |= 1;
    }

  if (b != 1)
  {
    __builtin_abort ();
  }

  return 0;
}