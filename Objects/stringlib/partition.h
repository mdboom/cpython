/* stringlib: partition implementation */

#ifndef STRINGLIB_FASTSEARCH_H
#  error must include "stringlib/fastsearch.h" before including this module
#endif

#if !STRINGLIB_MUTABLE && !defined(STRINGLIB_GET_EMPTY)
#  error "STRINGLIB_GET_EMPTY must be defined if STRINGLIB_MUTABLE is zero"
#endif


Py_LOCAL_INLINE(PyObject*)
STRINGLIB(partition)(PyObject* str_obj,
                    const STRINGLIB_CHAR* str, Py_ssize_t str_len,
                    PyObject* sep_obj,
                    const STRINGLIB_CHAR* sep, Py_ssize_t sep_len)
{
    PyObject* out;
    Py_ssize_t pos;

    if (sep_len == 0) {
        PyErr_SetString(PyExc_ValueError, "empty separator");
        return NULL;
    }

    pos = FASTSEARCH(str, str_len, sep, sep_len, -1, FAST_SEARCH);

    if (pos < 0) {
#if STRINGLIB_MUTABLE
        out = _PyTuple_BorrowPack3(
            STRINGLIB_NEW(str, str_len),
            STRINGLIB_NEW(NULL, 0),
            STRINGLIB_NEW(NULL, 0)
            );
        if (out == NULL) {
            return NULL;
        }

        if (PyErr_Occurred()) {
            Py_DECREF(out);
            return NULL;
        }
#else
        Py_INCREF(str_obj);
        PyObject *empty = (PyObject*)STRINGLIB_GET_EMPTY();
        assert(empty != NULL);
        Py_INCREF(empty);
        Py_INCREF(empty);

        out = _PyTuple_BorrowPack3(str_obj, empty, empty);
        if (out == NULL) {
            Py_DECREF(str_obj);
            Py_DECREF(empty);
            Py_DECREF(empty);
            return NULL;
        }
#endif
        return out;
    }

    PyObject *a = STRINGLIB_NEW(str, pos);
    Py_INCREF(sep_obj);
    pos += sep_len;
    PyObject *b = STRINGLIB_NEW(str + pos, str_len - pos);

    if (PyErr_Occurred()) {
        return NULL;
    }

    out = _PyTuple_BorrowPack3(a, sep_obj, b);
    if (out == NULL) {
        Py_DECREF(a);
        Py_DECREF(sep_len);
        Py_DECREF(b);
        return NULL;
    }

    return out;
}

Py_LOCAL_INLINE(PyObject*)
STRINGLIB(rpartition)(PyObject* str_obj,
                     const STRINGLIB_CHAR* str, Py_ssize_t str_len,
                     PyObject* sep_obj,
                     const STRINGLIB_CHAR* sep, Py_ssize_t sep_len)
{
    PyObject* out;
    Py_ssize_t pos;

    if (sep_len == 0) {
        PyErr_SetString(PyExc_ValueError, "empty separator");
        return NULL;
    }

    out = PyTuple_New(3);
    if (!out)
        return NULL;

    pos = FASTSEARCH(str, str_len, sep, sep_len, -1, FAST_RSEARCH);

    if (pos < 0) {
#if STRINGLIB_MUTABLE
        PyTuple_SET_ITEM(out, 0, STRINGLIB_NEW(NULL, 0));
        PyTuple_SET_ITEM(out, 1, STRINGLIB_NEW(NULL, 0));
        PyTuple_SET_ITEM(out, 2, STRINGLIB_NEW(str, str_len));

        if (PyErr_Occurred()) {
            Py_DECREF(out);
            return NULL;
        }
#else
        PyObject *empty = (PyObject*)STRINGLIB_GET_EMPTY();
        assert(empty != NULL);
        Py_INCREF(empty);
        PyTuple_SET_ITEM(out, 0, empty);
        Py_INCREF(empty);
        PyTuple_SET_ITEM(out, 1, empty);
        Py_INCREF(str_obj);
        PyTuple_SET_ITEM(out, 2, (PyObject*) str_obj);
#endif
        return out;
    }

    PyTuple_SET_ITEM(out, 0, STRINGLIB_NEW(str, pos));
    Py_INCREF(sep_obj);
    PyTuple_SET_ITEM(out, 1, sep_obj);
    pos += sep_len;
    PyTuple_SET_ITEM(out, 2, STRINGLIB_NEW(str + pos, str_len - pos));

    if (PyErr_Occurred()) {
        Py_DECREF(out);
        return NULL;
    }

    return out;
}
