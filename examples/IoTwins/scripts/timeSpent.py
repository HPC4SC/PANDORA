import numpy as np
import pandas as pd
import h5py
import sys
import csv

def read_sim_output(file_route):
    with h5py.File(file_route, 'r') as hdf:
        #base_items = list(hdf.items())
        #print('Items in the base directory:', base_items)
        simulation = hdf.get('Person')
        #print(len(simulation))
        steps_dfs = []
        results = []
        agent_ids = []
        unique_agent_ids = []
        agent_times = []
        agent_ids_times = []
        for i in range(0,len(simulation)):
            G1 = hdf.get('/Person/step' + str(i))
            G1_items = list(G1.items())
            #print(G1_items)
            ids = np.array(G1_items[0][1])
            timeSpent = np.array(G1_items[1][1])
            data = {
            'id': ids,
            'timeSpent': timeSpent
            }
            step_df = pd.DataFrame(data, columns=['id', 'timeSpent'])
            for id in step_df['id']:
                agent_ids.append(id)
            for time in step_df['timeSpent']:
                agent_times.append(time)
        for i in range(len(agent_ids)):
                agent_ids_times.append((agent_ids[i],agent_times[i]))
        for id in agent_ids: 
            if id not in unique_agent_ids: 
                unique_agent_ids.append(id) 
        for i in range(len(unique_agent_ids)):
            maxTimeSpent = 0
            for j in range(len(agent_ids_times)):
                if unique_agent_ids[i] == agent_ids_times[j][0] and maxTimeSpent < agent_ids_times[j][1]:
                    maxTimeSpent = agent_ids_times[j][1]                
            results.append((unique_agent_ids[i],maxTimeSpent))  
        with open('./outputTime.csv','w') as out:
            csv_out=csv.writer(out)
            csv_out.writerow(['id','timeSpent(s)'])
            for row in results:
                csv_out.writerow(row)

if __name__ == "__main__":
    if len(sys.argv) < 2:
        print('You need to specify the route of the agent data file')
    else:
        read_sim_output(sys.argv[1])