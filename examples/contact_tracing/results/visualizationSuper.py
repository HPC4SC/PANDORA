import numpy as np
import pandas as pd
import h5py
import sys
import matplotlib.pyplot as plt
import seaborn as sns 
from celluloid import Camera
import csv

def read_agents(agent_route,scenario):
    with h5py.File(agent_route, 'r') as hdf:
        #base_items = list(hdf.items())
        #print('Items in the base directory:', base_items)
        if scenario == '0':
            agentType = 'Client'
        elif scenario == '1':
            agentType = 'Walker'
        elif scenario == '2':
            agentType = 'Passenger'

        simulation = hdf.get(agentType)
        #print(len(simulation))
        steps_dfs = []
        for i in range(0,len(simulation)):
            G1 = hdf.get('/' + agentType + '/step' + str(i))
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
            if scenario == '0':
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

def generateOutput(steps_dfs,concat):
    total_people = concat['id'].nunique()
    steps = len(steps_dfs) - 1
    aggregation_functions = {'SICKcombinedBucketsNotifications': 'last', 'SICKfirstBucketNotifications': 'last', 'combinedBucketsNotifications': 'last', 'countInfected' : 'last', 'firstBucketNotifications' : 'last', 'infected' : 'last', 'infectionTime' : 'last', 'sick' : 'last', 'timeInSimulation' : 'last'}
    agent_records = concat.groupby(concat['id']).aggregate(aggregation_functions)

    agent_records['infected'] = agent_records.apply(lambda x: x['infected'] > 0, axis=1)
    agent_records['SICKcombinedBucketsNotifications'] = agent_records.apply(lambda x: x['SICKcombinedBucketsNotifications'] > 0, axis=1)

    agent_records['truePositives'] = agent_records['SICKcombinedBucketsNotifications'] & agent_records['infected']
    agent_records['falseNegatives'] = ~agent_records['SICKcombinedBucketsNotifications'] & agent_records['infected']
    agent_records['falsePositives'] = agent_records['SICKcombinedBucketsNotifications'] & ~agent_records['infected']
    agent_records['trueNegatives'] = ~agent_records['SICKcombinedBucketsNotifications'] & ~agent_records['infected']

    infected = len(agent_records[agent_records['infected']>0])

    return {
        'meanTimeInSimulation': agent_records['timeInSimulation'].mean(),
        'medianTimeInSimulation': agent_records['timeInSimulation'].median(),
        'avgPeoplePerMinute': total_people/(steps/60),
        'totalPeople': total_people,
        'previouslySick': sum(agent_records['sick']),
        'infected': infected,
        'infectionRate': infected/(total_people-sum(agent_records['sick'])),
        'firstBucketNotifications': sum(agent_records['firstBucketNotifications']),
        'combinedBucketsNotifications': sum(agent_records['combinedBucketsNotifications']),
        'SICKfirstBucketNotifications': sum(agent_records['SICKfirstBucketNotifications']),
        'SICKcombinedBucketsNotifications': sum(agent_records['SICKcombinedBucketsNotifications']),
        'truePositives': sum(agent_records['truePositives'])/total_people,
        'trueNegatives': sum(agent_records['trueNegatives'])/total_people,
        'falseNegatives': sum(agent_records['falseNegatives'])/total_people,
        'falsePositives': sum(agent_records['falsePositives'])/total_people
    }

def print_agents(steps_dfs,scenario):
    concat = pd.concat(steps_dfs)
    result = []
    result.append(generateOutput(steps_dfs,concat))
    if scenario == '0':
        x1 = 0
        y1 = 40
        x2 = 40
        y2 = 0
        type = 'Supermarket'
        img = plt.imread("../maps/spar2.png")
        agentColor = 'white'
    elif scenario == '1':
        x1 = 0
        y1 = 100
        x2 = 10
        y2 = 0
        type = 'Street'
        img = plt.imread("../maps/street_layout.png")
        agentColor = 'red'
    elif scenario == '2':
        x1 = 0
        y1 = 258
        x2 = 9
        y2 = 0
        type = 'Train' 
        img = plt.imread("../maps/train_layout.png")
        agentColor = 'white'
    pd.DataFrame(result).to_csv('./' + type + 'Output.csv')

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
        plt.xlim(x1,x2)
        plt.ylim(y1,y2)
        plt.scatter(x[i],y[i],s=4,color=agentColor,marker='o')
        camera.snap()
    anim = camera.animate(blit=True)
    anim.save('animation' + type + '.htm')

def view_agents(espai_agents,scenario):
    steps_dfs = read_agents(espai_agents,scenario)
    print_agents(steps_dfs,scenario)

if __name__ == "__main__":
    if len(sys.argv) < 2:
        print('You need to specify the route of the agent data file')
    else:
        view_agents(sys.argv[1],sys.argv[2])