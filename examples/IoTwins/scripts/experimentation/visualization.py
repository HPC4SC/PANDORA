import numpy as np
import pandas as pd
import h5py
import sys
import matplotlib.pyplot as plt
import seaborn as sns 
from celluloid import Camera

def read_agents(agent_route):
    with h5py.File(agent_route, 'r') as hdf:
        #base_items = list(hdf.items())
        #print('Items in the base directory:', base_items)
        simulation = hdf.get('Person')
        #print(len(simulation))
        steps_dfs = []
        for i in range(0,len(simulation)):
            G1 = hdf.get('/Person/step' + str(i))
            G1_items = list(G1.items())
            #print(G1_items)
            ids = np.array(G1_items[0][1])
            x_coords = np.array(G1_items[2][1])
            y_coords = np.array(G1_items[3][1])
            data = {
            'id': ids,
            'x': x_coords,
            'y': y_coords
            }
            step_df = pd.DataFrame(data, columns=['id', 'x', 'y'])
            #print(step_df)
            steps_dfs.append(step_df)
        #print(steps_dfs)
        return steps_dfs

def print_agents(steps_dfs):
    camera = Camera(plt.figure())
    sns.set()
    points = []
    x = []
    y = []
    for df in steps_dfs:
        x.append(df['x'])
        y.append(df['y'])
        points.append((x,y))
    for i in range(len(points)):
        plt.xlim(0,970)
        plt.ylim(970,0)
        plt.scatter(x[i],y[i],s=0.15)
        camera.snap()
    anim = camera.animate(blit=True)
    anim.save('scatter.htm')

def view_agents(espai__agents):
    steps_dfs = read_agents(espai__agents)
    print_agents(steps_dfs)

if __name__ == "__main__":
    if len(sys.argv) < 2:
        print('You need to specify the route of the agent data file')
    else:
        view_agents(sys.argv[1])