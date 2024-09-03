import numpy as np

from util import check_mul_result


def pairwise():
    # for i in range(10):
    #     test = np.array(0, dtype=np.int64)
    #     sum = test * 1.0
    #     check_mul_result(test, 1.0, sum)
    #
    # return sum

    pts = np.linspace(0,10,20000).reshape(200,100)
    return np.sum((pts[None,:] - pts[:, None])**2, -1)**0.5