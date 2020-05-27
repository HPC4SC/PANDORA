import pandas as pd
import sys

def read_pandas(route):
    hdf = pd.HDFStore(route, mode='r')
    print(hdf.keys())
    df1 = hdf.get('DF1')
    print(type(df1))
    print(df1.head())
    hdf.close()

if __name__ == "__main__":
    if len(sys.argv) < 2:
        print('You need to specify the route and the source file')
    else:
        read_pandas(sys.argv[1])