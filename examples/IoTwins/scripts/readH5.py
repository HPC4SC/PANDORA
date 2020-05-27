import numpy as np
import h5py
import sys

def read(route):
    with h5py.File(route, 'r') as hdf:
        ls = list(hdf.keys())
        print('List of datasets in this file: ', ls)
        data = hdf.get('dataset1')
        datset1 = np.array(data)
        print('Shape of dataset1: ', datset1.shape)

if __name__ == "__main__":
    if len(sys.argv) == 1:
        print('You need to specify the route')
    else:
        read(sys.argv[1])