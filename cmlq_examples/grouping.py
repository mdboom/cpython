import numpy as np
values = None

def initialize():
    global values
    if values is not None:
        return values

    print("Initializing values")
    N = 500000
    np.random.seed(0)
    values = np.array(np.random.randint(0,3298,size=N),dtype='u4')
    values.sort()
    return values

def grouping(values):
    import numpy as np
    diff = np.concatenate(([1], np.diff(values)))
    idx = np.concatenate((np.where(diff)[0], [len(values)]))
    return values[idx[:-1]], np.diff(idx)