#ifndef Py_INTERNAL_LONG_H
#define Py_INTERNAL_LONG_H
#ifdef __cplusplus
extern "C" {
#endif

#ifndef Py_BUILD_CORE
#  error "this header requires Py_BUILD_CORE define"
#endif

#include "pycore_bytesobject.h"   // _PyBytesWriter
#include "pycore_global_objects.h"// _PY_NSMALLNEGINTS
#include "pycore_runtime.h"       // _PyRuntime

// MGDTODO: Put this in the right place
#define PyLong_IS_LONG_MASK ((digit)1 << 31)
#define PyLong_IS_NEGATIVE_MASK ((digit)1 << 30)
#define PyLong_FLAGS_MASK (PyLong_IS_LONG_MASK | PyLong_IS_NEGATIVE_MASK)
#define PyLong_LONG_HEADER_DIGITS (2)

/*
 * Default int base conversion size limitation: Denial of Service prevention.
 *
 * Chosen such that this isn't wildly slow on modern hardware and so that
 * everyone's existing deployed numpy test suite passes before
 * https://github.com/numpy/numpy/issues/22098 is widely available.
 *
 * $ python -m timeit -s 's = "1"*4300' 'int(s)'
 * 2000 loops, best of 5: 125 usec per loop
 * $ python -m timeit -s 's = "1"*4300; v = int(s)' 'str(v)'
 * 1000 loops, best of 5: 311 usec per loop
 * (zen2 cloud VM)
 *
 * 4300 decimal digits fits a ~14284 bit number.
 */
#define _PY_LONG_DEFAULT_MAX_STR_DIGITS 4300
/*
 * Threshold for max digits check.  For performance reasons int() and
 * int.__str__() don't checks values that are smaller than this
 * threshold.  Acts as a guaranteed minimum size limit for bignums that
 * applications can expect from CPython.
 *
 * % python -m timeit -s 's = "1"*640; v = int(s)' 'str(int(s))'
 * 20000 loops, best of 5: 12 usec per loop
 *
 * "640 digits should be enough for anyone." - gps
 * fits a ~2126 bit decimal number.
 */
#define _PY_LONG_MAX_STR_DIGITS_THRESHOLD 640

#if ((_PY_LONG_DEFAULT_MAX_STR_DIGITS != 0) && \
   (_PY_LONG_DEFAULT_MAX_STR_DIGITS < _PY_LONG_MAX_STR_DIGITS_THRESHOLD))
# error "_PY_LONG_DEFAULT_MAX_STR_DIGITS smaller than threshold."
#endif

// _PyLong_NumBits.  Return the number of bits needed to represent the
// absolute value of a long.  For example, this returns 1 for 1 and -1, 2
// for 2 and -2, and 2 for 3 and -3.  It returns 0 for 0.
// v must not be NULL, and must be a normalized long.
// (size_t)-1 is returned and OverflowError set if the true result doesn't
// fit in a size_t.
//
// Export for 'math' shared extension.
PyAPI_FUNC(size_t) _PyLong_NumBits(PyObject *v);


/* runtime lifecycle */

extern PyStatus _PyLong_InitTypes(PyInterpreterState *);
extern void _PyLong_FiniTypes(PyInterpreterState *interp);


/* other API */

#define _PyLong_SMALL_INTS _Py_SINGLETON(small_ints)

// _PyLong_GetZero() and _PyLong_GetOne() must always be available
// _PyLong_FromUnsignedChar must always be available
#if _PY_NSMALLPOSINTS < 257
#  error "_PY_NSMALLPOSINTS must be greater than or equal to 257"
#endif

// Return a reference to the immortal zero singleton.
// The function cannot return NULL.
static inline PyObject* _PyLong_GetZero(void)
{ return (PyObject *)&_PyLong_SMALL_INTS[_PY_NSMALLNEGINTS]; }

// Return a reference to the immortal one singleton.
// The function cannot return NULL.
static inline PyObject* _PyLong_GetOne(void)
{ return (PyObject *)&_PyLong_SMALL_INTS[_PY_NSMALLNEGINTS+1]; }

static inline PyObject* _PyLong_FromUnsignedChar(unsigned char i)
{
    return (PyObject *)&_PyLong_SMALL_INTS[_PY_NSMALLNEGINTS+i];
}

// _PyLong_Frexp returns a double x and an exponent e such that the
// true value is approximately equal to x * 2**e.  e is >= 0.  x is
// 0.0 if and only if the input is 0 (in which case, e and x are both
// zeroes); otherwise, 0.5 <= abs(x) < 1.0.  On overflow, which is
// possible if the number of bits doesn't fit into a Py_ssize_t, sets
// OverflowError and returns -1.0 for x, 0 for e.
//
// Export for 'math' shared extension
PyAPI_DATA(double) _PyLong_Frexp(PyLongObject *a, Py_ssize_t *e);

extern PyObject* _PyLong_FromBytes(const char *, Py_ssize_t, int);

// _PyLong_DivmodNear.  Given integers a and b, compute the nearest
// integer q to the exact quotient a / b, rounding to the nearest even integer
// in the case of a tie.  Return (q, r), where r = a - q*b.  The remainder r
// will satisfy abs(r) <= abs(b)/2, with equality possible only if q is
// even.
//
// Export for '_datetime' shared extension.
PyAPI_DATA(PyObject*) _PyLong_DivmodNear(PyObject *, PyObject *);

// _PyLong_Format: Convert the long to a string object with given base,
// appending a base prefix of 0[box] if base is 2, 8 or 16.
// Export for '_tkinter' shared extension.
PyAPI_DATA(PyObject*) _PyLong_Format(PyObject *obj, int base);

// Export for 'math' shared extension
PyAPI_DATA(PyObject*) _PyLong_Rshift(PyObject *, size_t);

// Export for 'math' shared extension
PyAPI_DATA(PyObject*) _PyLong_Lshift(PyObject *, size_t);

extern PyObject* _PyLong_Add(PyLongObject *left, PyLongObject *right);
extern PyObject* _PyLong_Multiply(PyLongObject *left, PyLongObject *right);
extern PyObject* _PyLong_Subtract(PyLongObject *left, PyLongObject *right);

// Export for 'binascii' shared extension.
PyAPI_DATA(unsigned char) _PyLong_DigitValue[256];

/* Format the object based on the format_spec, as defined in PEP 3101
   (Advanced String Formatting). */
extern int _PyLong_FormatAdvancedWriter(
    _PyUnicodeWriter *writer,
    PyObject *obj,
    PyObject *format_spec,
    Py_ssize_t start,
    Py_ssize_t end);

extern int _PyLong_FormatWriter(
    _PyUnicodeWriter *writer,
    PyObject *obj,
    int base,
    int alternate);

extern char* _PyLong_FormatBytesWriter(
    _PyBytesWriter *writer,
    char *str,
    PyObject *obj,
    int base,
    int alternate);

// Argument converters used by Argument Clinic

// Export for 'select' shared extension (Argument Clinic code)
PyAPI_FUNC(int) _PyLong_UnsignedShort_Converter(PyObject *, void *);

// Export for '_testclinic' shared extension (Argument Clinic code)
PyAPI_FUNC(int) _PyLong_UnsignedInt_Converter(PyObject *, void *);

// Export for '_blake2' shared extension (Argument Clinic code)
PyAPI_FUNC(int) _PyLong_UnsignedLong_Converter(PyObject *, void *);

// Export for '_blake2' shared extension (Argument Clinic code)
PyAPI_FUNC(int) _PyLong_UnsignedLongLong_Converter(PyObject *, void *);

// Export for '_testclinic' shared extension (Argument Clinic code)
PyAPI_FUNC(int) _PyLong_Size_t_Converter(PyObject *, void *);

/* Long value tag bits:
 * 31: Is long?  Is the representation that stores more than one digit?
 * 30: Is negative
 */
// MGDTODO: Copy defines here?

/* The functions _PyLong_IsCompact and _PyLong_CompactValue are defined
 * in Include/cpython/longobject.h, since they need to be inline.
 *
 * "Compact" values have at least one bit to spare,
 * so that addition and subtraction can be performed on the values
 * without risk of overflow.
 *
 * The inline functions need tag bits.
 * For readability, rather than do `#define NON_SIZE_BITS _PyLong_NON_SIZE_BITS`
 * we define them to the numbers in both places and then assert that
 * they're the same.
 */
// MGDTODO: Restore the new equivalents here

/* All *compact" values are guaranteed to fit into
 * a uint32_t with at least one bit to spare.
 */

/* Return 1 if the argument is compact int */
static inline int
_PyLong_IsNonNegativeCompact(const PyLongObject* op) {
    assert(PyLong_Check(op));
    return (op->ob_digit[0] & PyLong_FLAGS_MASK) == (digit)0;
}


static inline int
_PyLong_BothAreCompact(const PyLongObject* a, const PyLongObject* b) {
    assert(PyLong_Check(a));
    assert(PyLong_Check(b));
    return ((a->ob_digit[0] | b->ob_digit[0]) & PyLong_IS_LONG_MASK) == 0;
}

static inline bool
_PyLong_IsZero(const PyLongObject *op)
{
    return op->ob_digit[0] == 0;
}

static inline bool
_PyLong_IsNegative(const PyLongObject *op)
{
    return op->ob_digit[0] & PyLong_IS_NEGATIVE_MASK;
}

static inline bool
_PyLong_IsPositive(const PyLongObject *op)
{
    assert(PyLong_Check(op));

    // MGDTODO: Optimize
    return op->ob_digit[0] != 0 && ((op->ob_digit[0] & PyLong_IS_NEGATIVE_MASK) == 0);
}

static inline Py_ssize_t
_PyLong_DigitCount(const PyLongObject *op)
{
    // assert(PyLong_Check(op));
    if (_PyLong_IsCompact(op)) {
        return _PyLong_IsZero(op) ? 0 : 1;
    } else {
        return (Py_ssize_t)op->ob_digit[1] | (Py_ssize_t)(op->ob_digit[0] & PyLong_MASK) << 32;
    }
}

static inline int
_PyLong_NonCompactSign(const PyLongObject *op)
{
    assert(PyLong_Check(op));
    assert(!_PyLong_IsCompact(op));
    return (op->ob_digit[0] & PyLong_IS_NEGATIVE_MASK) ? -1 : 1;
}

static inline int
_PyLong_CompactSign(const PyLongObject *op)
{
    assert(PyLong_Check(op));
    assert(_PyLong_IsCompact(op));
    if (op->ob_digit[0] == 0) {
        return 0;
    }
    return (op->ob_digit[0] & PyLong_IS_NEGATIVE_MASK) ? -1 : 1;
}

/* Equivalent to _PyLong_DigitCount(op) * _PyLong_NonCompactSign(op) */
static inline Py_ssize_t
_PyLong_SignedDigitCount(const PyLongObject *op)
{
    assert(PyLong_Check(op));
    if (op->ob_digit[0] == 0) {
        return 0;
    } else if (_PyLong_IsCompact(op)) {
        return (op->ob_digit[0] & PyLong_IS_NEGATIVE_MASK) ? -1 : 1;
    } else {
        return (Py_ssize_t)_PyLong_DigitCount(op) * _PyLong_NonCompactSign(op);
    }
}

/* Do a and b have the same sign? */
static inline int
_PyLong_SameSign(const PyLongObject *a, const PyLongObject *b)
{
    if (a->ob_digit[0] == 0 && b->ob_digit[0] == 0) {
        return 1;
    }
    return !((a->ob_digit[0] ^ b->ob_digit[0]) & PyLong_IS_NEGATIVE_MASK);
}

static inline void
_PyLong_SetSign(PyLongObject *op, int sign)
{
    assert(-1 <= sign && sign <= 1);
    switch (sign) {
        case 0:
            op->ob_digit[0] = 0;
            break;
        case 1:
            op->ob_digit[0] |= ~PyLong_IS_NEGATIVE_MASK;
            break;
        case -1:
            op->ob_digit[0] |= PyLong_IS_NEGATIVE_MASK;
            break;
    }
}

static inline void
_PyLong_SetSignAndDigitCount(PyLongObject *op, int sign, Py_ssize_t size)
{
    assert(size >= 0);
    assert(-1 <= sign && sign <= 1);
    assert(sign != 0 || size == 0);
    if (size == 0) {
        assert(sign == 0);
        op->ob_digit[0] = 0;
    } else if (size == 1) {
        // assert(op->ob_digit[0] != 0);
        // MGDTODO: Optimize
        if (_PyLong_IsCompact(op)) {
            op->ob_digit[0] = (
                ((sign == -1) ? PyLong_IS_NEGATIVE_MASK : 0) |
                (op->ob_digit[0] & PyLong_MASK)
            );
        } else {
            op->ob_digit[0] = (
                ((sign == -1) ? PyLong_IS_NEGATIVE_MASK : 0) |
                (op->ob_digit[PyLong_LONG_HEADER_DIGITS] & PyLong_MASK)
            );
        }
    } else {
        // assert(op->ob_digit[0] & PyLong_IS_LONG_MASK);
        // assert(_PyLong_DigitCount(op) >= size);
        op->ob_digit[0] = (
            PyLong_IS_LONG_MASK |
            ((sign == -1) ? PyLong_IS_NEGATIVE_MASK : 0) |
            (size >> 32)
        );
        op->ob_digit[1] = (size & 0xffffffff);
    }
}

static inline void
_PyLong_SetDigitCount(PyLongObject *op, Py_ssize_t size)
{
    assert(size >= 0);
    assert(!_PyLong_IsCompact(op));
    if (size == 0) {
        op->ob_digit[0] = 0;
    } else if (size == 1) {
        if (!_PyLong_IsCompact(op)) {
            op->ob_digit[0] = (
                (op->ob_digit[0] & PyLong_IS_NEGATIVE_MASK) |
                (op->ob_digit[PyLong_LONG_HEADER_DIGITS] & PyLong_MASK)
            );
        }
    } else {
        assert(op->ob_digit[0] & PyLong_IS_LONG_MASK);
        assert(_PyLong_DigitCount(op) >= size);
        op->ob_digit[0] = (
            (op->ob_digit[0] & PyLong_FLAGS_MASK) |
            size >> 32
        );
        op->ob_digit[1] = (size & 0xffffffff);
    }
}

static inline void
_PyLong_FlipSign(PyLongObject *op) {
    op->ob_digit[0] ^= PyLong_IS_NEGATIVE_MASK;
}

// MGDTODO: These objects will be 4 bytes larger than needed.

#define _PyLong_DIGIT_INIT(val) \
    { \
        .ob_base = _PyObject_HEAD_INIT(&PyLong_Type), \
        .ob_digit = { \
            (((val) < 0) ? PyLong_IS_NEGATIVE_MASK : 0) | \
            (((val) < 0) ? -(val) : (val)) \
        } \
    }

#ifdef __cplusplus
}
#endif
#endif /* !Py_INTERNAL_LONG_H */
