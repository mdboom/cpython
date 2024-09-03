import numpy as np
def l1norm(x, y):
    return np.sum(np.abs(x[:, None, :] - y), axis=-1)