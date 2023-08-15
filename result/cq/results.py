import pandas as pd
import numpy as np
import subprocess

destination_server = 'alious@beluga.calculquebec.ca'
remote_path = "/home/alious/works/CimentQuebec/result/cq/"
local_path = "/Users/aliwa/Documents/Ulaval/thesis/works/cimentQuebec/result/cq/"
path = "/Users/aliwa/Documents/Ulaval/thesis/works/cimentQuebec/"


def downloadFile(file="results.csv"):
    local_file = f"{local_path}/{file}"
    remote_file = f"{remote_path}/{file}"
    command = f'scp {destination_server}:{remote_file} {local_file}'
    try:
        subprocess.check_output(command, shell=True)
        print('File successfully transferred.')
    except subprocess.CalledProcessError as e:
        print(f'Error occurred: {e}')

    return local_file


def getStats(result_file, df_kinable, methods,statFile,instanceName):

    my_df = pd.read_csv(f"{result_file}",sep=";",header=0)
    my_df = my_df[["Name","grasp","runtime","Heure"]]
    my_df["grasp"] = my_df["grasp"].apply(lambda x:int(x))
    table = pd.pivot_table(my_df, values=['grasp','runtime'], index=['Name'],
                           aggfunc={'grasp': np.max,
                                    'runtime': np.mean})
    table.reset_index(inplace=True)
    df_merge = pd.merge(df_kinable,table,how='inner')

    with open(f"{statFile}",'w') as f:
        f.write(f";;{instanceName};;;\n")
        f.write("Method;better;equal;worse;Gap worse;Gap;Time_Kinable;Time_GRASP;Total\n")
        for val in methods:

            df_merge["Gap_heur"] = df_merge.apply(lambda x: 100*(x.grasp-x[f"{val}"])/x[f"{val}"],axis=1)
            df_worse = df_merge[df_merge["Gap_heur"]<0]
            df_better = df_merge[df_merge["Gap_heur"]>0]
            df_equal = df_merge[df_merge["Gap_heur"]==0]
            count = len(df_better) + len(df_equal) + len(df_worse)

            f.write(f"{val};{len(df_better)};{len(df_equal)};{len(df_worse)};"
                    f"{-np.mean(df_worse['Gap_heur']):.2f};{-np.mean(df_merge['Gap_heur']):.2f};")
            if val != 'UB':
                f.write(f"{np.mean(df_merge[f'{val}_Time'])/1000:.2f};")
            else:
                f.write("-;")
            f.write(f"{np.mean(df_merge['runtime'])/1000:.2f};{count}\n")
            print(val,' ',list(df_better['Name']))
        df_merge["Gap_heur"] = df_merge.apply(lambda x: 100*(x.grasp-x["Heuristic"])/x["Heuristic"],axis=1)
        df_worse = df_merge[df_merge["Gap_heur"]<0]
        df_worse.sort_values(by="Name",ascending=True, inplace=True)
        worse_instances = df_worse["Name"]
        with open(f"{path}/script/cdp/runWorse{instanceName}.sh",'w') as f:
            f.write("#!/bin/sh\n")
            # f.write("cp exec_heur exec_heurA\n\n")

            for name in worse_instances:
                f.write(f'sbatch {name}.sb \n')
                # f.write(f'./exec_heurA ../instances/cdp/{name}.rmc 1000 "test_A.csv" >> OutputA.txt\n')

            df_better = df_merge[df_merge["Gap_heur"]>0]
            df_equal = df_merge[df_merge["Gap_heur"]==0]

            print(f"{len(df_better)} better solutions -- {len(df_equal)} equals solutions {len(df_worse)} worse solutions = {len(df_merge)}")
            print(np.mean(df_worse['Gap_heur']))
            print(list(df_worse['Name']))
            # print(df_better)

methodA= ["UB","CP","MIP","Heuristic","Hybrid"]
# fileA=f"{local_path}/test_A.csv"
fileA=downloadFile("results.csv")
# getStats(fileA,df_kinableA,methodA,"./statsA.csv","A")
