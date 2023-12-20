# import os
# os.environ["OMP_NUM_THREADS"] = '1'

import time

import numpy as np

N = 2048

A = np.random.randn(N, N)
B = np.random.randn(N, N)

start = time.monotonic()
C = A @ B
print("%.2f" % (time.monotonic() - start))
