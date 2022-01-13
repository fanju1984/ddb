import pickle
import time


def test(a):
    t1 = time.time()
    pickle.dumps(a)
    t2 = time.time()
    print(t2 - t1)
    t1 = time.time()
    str(a)
    t2 = time.time()
    print(t2 - t1)
