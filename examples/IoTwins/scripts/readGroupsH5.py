import numpy as np
import h5py
import sys

def read_groups(route):
    with h5py.File(route, 'r') as hdf:
        base_items = list(hdf.items())
        print('Items in the base directory:', base_items)
        G1 = hdf.get('Group1')
        G1_items = list(G1.items())
        print('Items in Group1:', G1_items)
        dataset4 = np.array(G1.get('dataset4'))
        print(dataset4)

        G2 = hdf.get('Group2')
        G2_items = list(G2.items())
        print('Items in Group2:', G2_items)
        G2_1 = G2.get('/Group2/SubGroup1')
        G2_1_items = list(G2_1.items())
        print('Items in Group2_1:', G2_1_items)
        dataset3 = np.array(G2_1.get('dataset3'))
        print(dataset3.shape)


if __name__ == "__main__":
    if len(sys.argv) == 1:
        print('You need to specify the route')
    else:
        read_groups(sys.argv[1])