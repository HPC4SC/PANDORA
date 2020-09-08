import numpy as np
import pandas as pd
import h5py
import sys
import matplotlib.pyplot as plt
import seaborn as sns 
from celluloid import Camera
import csv

def read_agents(agent_route):
    with h5py.File(agent_route, 'r') as hdf:
        #base_items = list(hdf.items())
        #print('Items in the base directory:', base_items)
        simulation = hdf.get('Client')
        #print(len(simulation))
        steps_dfs = []
        for i in range(0,len(simulation)):
            G1 = hdf.get('/Client/step' + str(i))
            G1_items = list(G1.items())
            #print(G1_items)
            SICKcombinedBucketsNotifications = np.array(G1_items[0][1])
            SICKfirstBucketNotifications = np.array(G1_items[1][1])
            combinedBucketsNotifications = np.array(G1_items[2][1])
            countInfected = np.array(G1_items[3][1])
            firstBucketNotifications = np.array(G1_items[4][1])
            ids = np.array(G1_items[5][1])
            infected = np.array(G1_items[6][1])
            infectionTime = np.array(G1_items[7][1])
            sick = np.array(G1_items[8][1])
            timeInSimulation = np.array(G1_items[9][1])
            x_coords = np.array(G1_items[10][1])
            y_coords = np.array(G1_items[11][1])
            G2 = hdf.get('/Cashier/step' + str(i))
            G2_items = list(G2.items())
            SICKcombinedBucketsNotifications_cashier = np.array(G2_items[0][1])
            SICKfirstBucketNotifications_cashier = np.array(G2_items[1][1])
            combinedBucketsNotifications_cashier = np.array(G2_items[2][1])
            countInfected_cashier = np.array(G2_items[3][1])
            firstBucketNotifications_cashier = np.array(G2_items[4][1])
            ids_cashiers = np.array(G2_items[5][1])
            infected_cashier = np.array(G2_items[6][1])
            infectionTime_cashier = np.array(G2_items[7][1])
            sick_cashier = np.array(G2_items[8][1])
            timeInSimulation_cashier = np.array(G2_items[9][1])
            x_cashiers = np.array(G2_items[10][1])
            y_cashiers = np.array(G2_items[11][1])

            if ids.size != 0 and ids_cashiers.size != 0:
                SICKcombinedBucketsNotifications = np.append(SICKcombinedBucketsNotifications,SICKcombinedBucketsNotifications_cashier)
                SICKfirstBucketNotifications = np.append(SICKfirstBucketNotifications,SICKfirstBucketNotifications_cashier)
                combinedBucketsNotifications = np.append(combinedBucketsNotifications,combinedBucketsNotifications_cashier)
                countInfected = np.append(countInfected,countInfected_cashier)
                firstBucketNotifications = np.append(firstBucketNotifications,firstBucketNotifications_cashier)
                ids = np.append(ids,ids_cashiers)
                infected = np.append(infected,infected_cashier)
                infectionTime = np.append(infectionTime,infectionTime_cashier)
                sick = np.append(sick,sick_cashier)
                timeInSimulation = np.append(timeInSimulation,timeInSimulation_cashier)
                x_coords = np.append(x_coords,x_cashiers)
                y_coords = np.append(y_coords,y_cashiers)
            
            data = {
            'SICKcombinedBucketsNotifications': SICKcombinedBucketsNotifications,
            'SICKfirstBucketNotifications': SICKfirstBucketNotifications,
            'combinedBucketsNotifications': combinedBucketsNotifications,
            'countInfected': countInfected,
            'firstBucketNotifications': firstBucketNotifications,
            'id': ids,
            'infected': infected,
            'infectionTime': infectionTime,
            'sick': sick,
            'timeInSimulation': timeInSimulation,
            'x': x_coords,
            'y': y_coords,
            }
            step_df = pd.DataFrame(data, columns=['id', 'x', 'y','SICKcombinedBucketsNotifications','SICKfirstBucketNotifications','combinedBucketsNotifications','countInfected','firstBucketNotifications','infected','infectionTime','sick','timeInSimulation'])
            #print(step_df)
            steps_dfs.append(step_df)
        return steps_dfs

def print_agents(steps_dfs):
    result = pd.concat(steps_dfs)
    result.to_csv('./output.csv')

    img = plt.imread("../maps/spar2.png")
    fig, ax = plt.subplots()

    camera = Camera(fig)

    points = []
    x = []
    y = []
    for df in steps_dfs:
        x.append(df['x'])
        y.append(df['y'])
        points.append((x,y))
    for i in range(len(points)):
        ax.imshow(img)
        plt.xlim(0,40)
        plt.ylim(40,0)
        plt.scatter(x[i],y[i],s=4,color='white',marker='o')
        camera.snap()
    anim = camera.animate(blit=True)
    anim.save('animation.htm')

def view_agents(espai_agents):
    steps_dfs = read_agents(espai_agents)
    print_agents(steps_dfs)

if __name__ == "__main__":
    if len(sys.argv) < 2:
        print('You need to specify the route of the agent data file')
    else:
        view_agents(sys.argv[1])