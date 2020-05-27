import numpy as np
import h5py
import sys

def set_attributes(route):
    matrix1 = np.random.random(size=(1000,1000))
    matrix2 = np.random.random(size=(10000,100))

    hdf = h5py.File(route,'w')
    dataset1 = hdf.create_dataset('dataset1',data=matrix1)
    hdf.create_dataset('dataset2',data=matrix2)
    dataset1.attrs['CLASS'] = 'DATA MATRIX'
    dataset1.attrs['VERSION'] = '1.1'
    hdf.close()


def read_attributes(route):
    hdf = h5py.File(route,'r')
    ls = list(hdf.keys())
    print('List of datasets in this file:',ls)
    data = hdf.get('dataset1')
    dataset1 = np.array(data)
    print('Shape of datset1:', dataset1.shape)

    keys = list(data.attrs.keys())
    values = list(data.attrs.values())
    print(keys[0])
    print(values[0])
    print(data.attrs[keys[0]])

    hdf.close()

if __name__ == "__main__":
    if len(sys.argv) == 1:
        print('You need to specify the route')
    else:
        set_attributes(sys.argv[1])
        read_attributes(sys.argv[1])