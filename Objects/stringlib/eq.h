/* Fast unicode equal function optimized for dictobject.c and setobject.c */

/* Return 1 if two unicode objects are equal, 0 if not.
 * unicode_eq() is called when the hash of two unicode objects is equal.
 */
Py_LOCAL_INLINE(int)
unicode_eq(PyObject *str1, PyObject *str2)
{
    Py_ssize_t len = PyUnicode_GET_LENGTH(str1);
    if (PyUnicode_GET_LENGTH(str2) != len) {
        return 0;
    }

    Py_hash_t hash1 = ((PyASCIIObject*)str1)->hash;
    Py_hash_t hash2 = ((PyASCIIObject*)str2)->hash;
    if (hash1 != -1 && hash2 != -1 && hash1 != hash2) {
        return 0;
    }

    int kind = PyUnicode_KIND(str1);
    if (PyUnicode_KIND(str2) != kind) {
        return 0;
    }

    const void *data1 = PyUnicode_DATA(str1);
    const void *data2 = PyUnicode_DATA(str2);
    return (memcmp(data1, data2, len * kind) == 0);
}
