#ifndef Py_LIMITED_API
#ifndef Py_LONGINTREPR_H
#define Py_LONGINTREPR_H
#ifdef __cplusplus
extern "C" {
#endif


/* This is published for the benefit of "friends" marshal.c and _decimal.c. */

/* Parameters of the integer representation.  There are two different
   sets of parameters: one set for 30-bit digits, stored in an unsigned 32-bit
   integer type, and one set for 15-bit digits with each digit stored in an
   unsigned short.  The value of PYLONG_BITS_IN_DIGIT, defined either at
   configure time or in pyport.h, is used to decide which digit size to use.

   Type 'digit' should be able to hold 2*PyLong_BASE-1, and type 'twodigits'
   should be an unsigned integer type able to hold all integers up to
   PyLong_BASE*PyLong_BASE-1.  x_sub assumes that 'digit' is an unsigned type,
   and that overflow is handled by taking the result modulo 2**N for some N >
   PyLong_SHIFT.  The majority of the code doesn't care about the precise
   value of PyLong_SHIFT, but there are some notable exceptions:

   - PyLong_{As,From}ByteArray require that PyLong_SHIFT be at least 8

   - long_hash() requires that PyLong_SHIFT is *strictly* less than the number
     of bits in an unsigned long, as do the PyLong <-> long (or unsigned long)
     conversion functions

   - the Python int <-> size_t/Py_ssize_t conversion functions expect that
     PyLong_SHIFT is strictly less than the number of bits in a size_t

   - the marshal code currently expects that PyLong_SHIFT is a multiple of 15

   - NSMALLNEGINTS and NSMALLPOSINTS should be small enough to fit in a single
     digit; with the current values this forces PyLong_SHIFT >= 9

  The values 15 and 30 should fit all of the above requirements, on any
  platform.
*/

#if PYLONG_BITS_IN_DIGIT == 30
typedef uint32_t digit;
typedef int32_t sdigit; /* signed variant of digit */
typedef uint64_t twodigits;
typedef int64_t stwodigits; /* signed variant of twodigits */
#define PyLong_SHIFT    30
#define _PyLong_DECIMAL_SHIFT   9 /* max(e such that 10**e fits in a digit) */
#define _PyLong_DECIMAL_BASE    ((digit)1000000000) /* 10 ** DECIMAL_SHIFT */
#elif PYLONG_BITS_IN_DIGIT == 15
typedef unsigned short digit;
typedef short sdigit; /* signed variant of digit */
typedef unsigned long twodigits;
typedef long stwodigits; /* signed variant of twodigits */
#define PyLong_SHIFT    15
#define _PyLong_DECIMAL_SHIFT   4 /* max(e such that 10**e fits in a digit) */
#define _PyLong_DECIMAL_BASE    ((digit)10000) /* 10 ** DECIMAL_SHIFT */
#else
#error "PYLONG_BITS_IN_DIGIT should be 15 or 30"
#endif
#define PyLong_BASE     ((digit)1 << PyLong_SHIFT)
#define PyLong_MASK     ((digit)(PyLong_BASE - 1))

// MGDTODO: This is broken on 32-bit platforms
#define PyLong_LONG_FLAG_SHIFT (PyLong_SHIFT + 1)
#define PyLong_LONG_FLAG ((digit)1 << PyLong_LONG_FLAG_SHIFT)
#define PyLong_NEGATIVE_FLAG_SHIFT (PyLong_SHIFT)
#define PyLong_NEGATIVE_FLAG ((digit)1 << (PyLong_NEGATIVE_FLAG_SHIFT))
#define PyLong_FLAGS_MASK (PyLong_LONG_FLAG | PyLong_NEGATIVE_FLAG)
#define PyLong_HEADER_SIZE (1)

/* Long integer representation.

   Longs are represented by a variable number of "digits".

   The absolute value of a number is equal to SUM(for i=0 through
        abs(ob_size)-1) digit[i] * 2**(SHIFT*i)

   The number of digit array elements used depends on whether there is a single
   digit (compact form) or more than one digit (long form). The form in use is
   indicated by the PyLong_LONG_FLAG bit in the first element.

   Compact form uses a single array element:

      0sxx xxxx xxxx xxxx xxxx xxxx xxxx xxxx

   Long form uses the first array element to store the number of digits (n),
   followed by n elements to store the digits:

      1snn nnnn nnnn nnnn nnnn nnnn nnnn nnnn
      --xx xxxx xxxx xxxx xxxx xxxx xxxx xxxx
      ...

   Negative numbers are indicated with the PyLong_NEGATIVE_FLAG bit in the first element.

   Zero is represented by ob_digit[0] == 0.

   In a normalized number, ob_digit[abs(ob_size)-1 + 1] (the most significant digit)
   is never zero.

   Also, in all cases, for all valid i, 0 <= ob_digit[i] <= MASK.

   The allocation function takes care of allocating extra memory so that
   ob_digit[0] ... ob_digit[abs(ob_size)-1] are actually available. We always
   allocate memory for at least one digit, so accessing ob_digit[0] is always
   safe.
*/

struct _longobject {
    PyObject_HEAD
    digit ob_digit[1];
};

PyAPI_FUNC(PyLongObject*) _PyLong_New(Py_ssize_t);

// Return a copy of src.
PyAPI_FUNC(PyObject*) _PyLong_Copy(PyLongObject *src);

PyAPI_FUNC(PyLongObject*) _PyLong_FromDigits(
    int negative,
    Py_ssize_t digit_count,
    digit *digits);

static inline int
_PyLong_IsCompact(const PyLongObject* op) {
    assert(PyType_HasFeature((op)->ob_base.ob_type, Py_TPFLAGS_LONG_SUBCLASS));
    return !(op->ob_digit[0] & PyLong_LONG_FLAG);
}

#define PyUnstable_Long_IsCompact _PyLong_IsCompact

static inline Py_ssize_t
_PyLong_CompactValue(const PyLongObject *op)
{
    // Conditionally negate without a branch trick from "bit-twiddling hacks".
    // Avraham Plotnitzky, Alfonso De Gregorio, Sean Eron Anderson.
    // https://graphics.stanford.edu/~seander/bithacks.html#ConditionalNegate

    assert(PyType_HasFeature((op)->ob_base.ob_type, Py_TPFLAGS_LONG_SUBCLASS));
    assert(PyUnstable_Long_IsCompact(op));
    // Don't need to fully mask this flag since PyLong_LONG_FLAG is always 0 in
    // this case.
    Py_ssize_t negate = (op->ob_digit[0] >> PyLong_NEGATIVE_FLAG_SHIFT);
    return (((Py_ssize_t)op->ob_digit[0] & PyLong_MASK) ^ -negate) + negate;
}

#define PyUnstable_Long_CompactValue _PyLong_CompactValue


#ifdef __cplusplus
}
#endif
#endif /* !Py_LONGINTREPR_H */
#endif /* Py_LIMITED_API */
