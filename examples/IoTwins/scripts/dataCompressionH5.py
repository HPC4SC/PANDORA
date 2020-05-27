import numpy as np
import h5py
import sys

def compress_groups(route):
    matrix1 = np.random.random(size=(1000,1000))
    matrix2 = np.random.random(size=(1000,1000))
    matrix3 = np.random.random(size=(1000,1000))
    matrix4 = np.random.random(size=(1000,1000))
    with h5py.File(route,'w') as hdf:
        G1 = hdf.create_group('Group1')
        G1.create_dataset('dataset1', data=matrix1, compression='gzip', compression_opts=9)
        G1.create_dataset('dataset4', data=matrix4, compression='gzip', compression_opts=9)

        G2_1 = hdf.create_group('Group2/SubGroup1')
        G2_1.create_dataset('dataset3', data=matrix3, compression='gzip', compression_opts=9)

        G2_2 = hdf.create_group('Group2/SubGroup2')
        G2_2.create_dataset('dataset2', data=matrix2, compression='gzip', compression_opts=9)

if __name__ == "__main__":
    if len(sys.argv) == 1:
        print('You need to specify the route')
    else:
        compress_groups(sys.argv[1])