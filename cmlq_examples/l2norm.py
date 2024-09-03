import numpy as np

def l2norm():
  N = 1000
  np.random.seed(10)
  x = np.random.rand(N,N)
  return np.sqrt(np.sum(np.abs(x)**2, 1))