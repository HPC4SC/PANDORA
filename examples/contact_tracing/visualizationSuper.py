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
        simulation = hdf.get('Client')
        #print(len(simulation))
        steps_dfs = []
        for i in range(0,len(simulation)):
            G1 = hdf.get('/Client/step' + str(i))
            G1_items = list(G1.items())
            #print(G1_items)
            ids = np.array(G1_items[1][1])
            x_coords = np.array(G1_items[5][1])
            y_coords = np.array(G1_items[6][1])
            G2 = hdf.get('/Cashier/step' + str(i))
            G2_items = list(G2.items())
            ids_cashiers = np.array(G2_items[1][1])
            x_cashiers = np.array(G2_items[5][1])
            y_cashiers = np.array(G2_items[6][1])

            if ids.size != 0 and ids_cashiers.size != 0:
                ids = np.append(ids,ids_cashiers)
                x_coords = np.append(x_coords,x_cashiers)
                y_coords = np.append(y_coords,y_cashiers)
            
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
    img = plt.imread("./maps/spar2.png")
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
        plt.scatter(x[i],y[i],s=2,marker='o')
        camera.snap()
    anim = camera.animate(blit=True)
    anim.save('scatter.htm')

def view_agents(espai_agents):
    steps_dfs = read_agents(espai_agents)
    print_agents(steps_dfs)

if __name__ == "__main__":
    if len(sys.argv) < 2:
        print('You need to specify the route of the agent data file')
    else:
        view_agents(sys.argv[1])