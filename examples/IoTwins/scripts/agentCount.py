import numpy as np
import pandas as pd
import h5py
import sys
import csv

def read_sim_output(file_route):
    with h5py.File(file_route, 'r') as hdf:
        simulation = hdf.get('Counter')
        steps_dfs = []
        results = []
        agent_ids = []
        unique_agent_ids = []
        agent_count = []
        agent_ids_count = []
        for i in range(0,len(simulation)):
            G1 = hdf.get('/Counter/step' + str(i))
            G1_items = list(G1.items())
            #print(G1_items)
            ids = np.array(G1_items[1][1])
            agentCount = np.array(G1_items[0][1])
            data = {
            'id': ids,
            'agentCount': agentCount
            }
            step_df = pd.DataFrame(data, columns=['id', 'agentCount'])
            #print(step_df)
            for id in step_df['id']:
                agent_ids.append(id)
            for count in step_df['agentCount']:
                agent_count.append(count)
        for i in range(len(agent_ids)):
                agent_ids_count.append((agent_ids[i],agent_count[i]))
        for id in agent_ids: 
            if id not in unique_agent_ids: 
                unique_agent_ids.append(id) 
        for i in range(len(unique_agent_ids)):
            agentCounts = []
            for j in range(len(agent_ids_count)):
                if unique_agent_ids[i] == agent_ids_count[j][0]:
                   agentCounts.append(agent_ids_count[j][1])
            results.append((unique_agent_ids[i],agentCounts))
        with open('./outputCount.csv','w') as out:
            csv_out=csv.writer(out)
            csv_out.writerow(['id','agentCount'])
            for row in results:
                csv_out.writerow(row)

if __name__ == "__main__":
    if len(sys.argv) < 2:
        print('You need to specify the route of the agent data file')
    else:
        read_sim_output(sys.argv[1])