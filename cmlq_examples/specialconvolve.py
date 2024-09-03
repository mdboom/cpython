
def specialconvolve():
    import numpy as np
    a = np.arange(100*10000, dtype=np.uint32).reshape(1000,1000)
    rowconvol = a[1:-1,:] + a[:-2,:] + a[2:,:]
    colconvol = rowconvol[:,1:-1] + rowconvol[:,:-2] + rowconvol[:,2:] - 9*a[1:-1,1:-1]
    return colconvol
