import pandas as pd
import sys

def create(route,source):
    hdf = pd.HDFStore(route)
    df1 = pd.read_csv(source)
    hdf.put('DF1', df1, format='table', data_columns=True)

    data = {
        'city': ['Tripoli', 'Sydney', 'Tripoli', 'Rome', 'Rome', 'Tripoli', 'Rome', 'Sydney', 'Sydeny'],
        'rank': ['1st', '2nd', '1st', '2nd', '1st', '2nd', '1st', '2nd', '1st'],
        'score1': [44, 48, 39, 41, 38, 44, 34, 54, 61],
        'score2': [67, 63, 55, 70, 64, 77, 45, 66, 72]
        }
    df2 = pd.DataFrame(data, columns=['city', 'rank', 'score1', 'score2'])
    print(df2)
    hdf.put('DF2', df2, format='table', data_columns=True)

    hdf.close()

if __name__ == "__main__":
    if len(sys.argv) < 3:
        print('You need to specify the route and the source file')
    else:
        create(sys.argv[1],sys.argv[2])
