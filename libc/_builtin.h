#ifndef ELVM_LIBC_BUILTIN_H_
#define ELVM_LIBC_BUILTIN_H_

#ifdef __clang__

#define float int
#define double int

void exit(int c);
int putchar(int c);

#define DEFINE_STUB(fn) void fn() {             \
    for (int i = 0; #fn[i]; i++)                \
      putchar(#fn[i]);                          \
    putchar('\n');                              \
    exit(1);                                    \
  }

DEFINE_STUB(__addsf3)
DEFINE_STUB(__fixsfsi)
DEFINE_STUB(__fixunssfsi)
DEFINE_STUB(__floatsisf)
DEFINE_STUB(__floatunsisf)
DEFINE_STUB(__gesf2)
DEFINE_STUB(__gtsf2)
DEFINE_STUB(__ltsf2)
DEFINE_STUB(__mulsf3)
DEFINE_STUB(__nesf2)

#endif

int __elvm_builtin_mul(int a, int b) {
  int i, e, v;
  if (a < b) {
    v = a;
    a = b;
    b = v;
  }
  if (b == 1)
    return a;
  if (b == 0)
    return 0;
  int d[24];
  int r[24];
  for (i = 0, e = 1, v = a;; i++) {
    d[i] = v;
    r[i] = e;
    v += v;
    int ne = e + e;
    if (ne < e || ne > b)
      break;
    e = ne;
  }

  int x = 0;
  for (;; i--) {
    if (b >= r[i]) {
      x += d[i];
      b -= r[i];
    }
    if (i == 0)
      break;
  }
  return x;
}

unsigned int __elvm_builtin_divmod(unsigned int a,
                                   unsigned int b,
                                   unsigned int* rem) {
  unsigned int d[24];
  unsigned int r[24];
  unsigned int i;
  r[0] = 1;
  for (i = 0;; i++) {
    d[i] = b;
    unsigned int nb = b + b;
    if (nb > a || nb < b)
      break;
    r[i+1] = r[i] + r[i];
    b = nb;
  }

  unsigned int q = 0;
  for (;; i--) {
    unsigned int v = d[i];
    if (a >= v) {
      q += r[i];
      a -= v;
    }
    if (i == 0)
      break;
  }
  *rem = a;
  return q;
}

unsigned int __elvm_builtin_div(unsigned int a, unsigned int b) {
  if (b == 1)
    return a;
  unsigned int r;
  return __elvm_builtin_divmod(a, b, &r);
}

unsigned int __elvm_builtin_mod(unsigned int a, unsigned int b) {
  unsigned int r;
  __elvm_builtin_divmod(a, b, &r);
  return r;
}

const unsigned int __elvm_builtin_bits_table[] = {
  0x800000, 0x400000, 0x200000, 0x100000,
  0x80000, 0x40000, 0x20000, 0x10000,
  0x8000, 0x4000, 0x2000, 0x1000,
  0x800, 0x400, 0x200, 0x100,
  0x80, 0x40, 0x20, 0x10,
  0x8, 0x4, 0x2, 0x1,
};

#define __ELVM_BUILTIN_TO_BIT(v, t) (v >= t ? (v -= t, 1) : 0)

unsigned int __elvm_builtin_and(unsigned int a, unsigned int b) {
  unsigned int r = 0;
  for (int i = 0; i < 24; i++) {
    unsigned int t = __elvm_builtin_bits_table[i];
    unsigned int a1 = __ELVM_BUILTIN_TO_BIT(a, t);
    unsigned int b1 = __ELVM_BUILTIN_TO_BIT(b, t);
    if (a1 && b1)
      r += t;
  }
  return r;
}

unsigned int __elvm_builtin_or(unsigned int a, unsigned int b) {
  unsigned int r = 0;
  for (int i = 0; i < 24; i++) {
    unsigned int t = __elvm_builtin_bits_table[i];
    unsigned int a1 = __ELVM_BUILTIN_TO_BIT(a, t);
    unsigned int b1 = __ELVM_BUILTIN_TO_BIT(b, t);
    if (a1 || b1)
      r += t;
  }
  return r;
}

unsigned int __elvm_builtin_xor(unsigned int a, unsigned int b) {
  unsigned int r = 0;
  for (int i = 0; i < 24; i++) {
    unsigned int t = __elvm_builtin_bits_table[i];
    unsigned int a1 = __ELVM_BUILTIN_TO_BIT(a, t);
    unsigned int b1 = __ELVM_BUILTIN_TO_BIT(b, t);
    if (a1 != b1)
      r += t;
  }
  return r;
}

unsigned int __elvm_builtin_not(unsigned int a) {
  unsigned int r = 0;
  for (int i = 0; i < 24; i++) {
    unsigned int t = __elvm_builtin_bits_table[i];
    unsigned int a1 = __ELVM_BUILTIN_TO_BIT(a, t);
    if (!a1)
      r += t;
  }
  return r;
}

unsigned int __elvm_builtin_shl(unsigned int a, unsigned int b) {
  unsigned int r = 0;
  for (int i = 0; i < b; i++) {
    unsigned int t = __elvm_builtin_bits_table[i];
    unsigned int a1 = __ELVM_BUILTIN_TO_BIT(a, t);
  }
  for (int i = b; i < 24; i++) {
    unsigned int t = __elvm_builtin_bits_table[i];
    unsigned int a1 = __ELVM_BUILTIN_TO_BIT(a, t);
    if (a1)
      r += __elvm_builtin_bits_table[i-b];
  }
  return r;
}

unsigned int __elvm_builtin_shr(unsigned int a, unsigned int b) {
  unsigned int r = 0;
  for (int i = b; i < 24; i++) {
    unsigned int t = __elvm_builtin_bits_table[i-b];
    unsigned int a1 = __ELVM_BUILTIN_TO_BIT(a, t);
    if (a1)
      r += __elvm_builtin_bits_table[i];
  }
  return r;
}

#endif  // ELVM_LIBC_BUILTIN_H_
