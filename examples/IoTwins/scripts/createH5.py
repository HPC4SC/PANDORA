import numpy as np
import h5py

def create():
    matrix1 = np.random.random(size=(1000,1000))
    matrix2 = np.random.random(size=(10000,100))
    with h5py.File('C:/Users/Quim/Documents/BSC/Pandora/PANDORA/examples/IoTwins/scripts/H5files/hdf5_data.h5', 'w') as hdf:
        hdf.create_dataset('dataset1', data=matrix1)
        hdf.create_dataset('dataset2', data=matrix2)

if __name__ == "__main__":
        create()