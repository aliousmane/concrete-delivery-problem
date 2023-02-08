import os
import pandas as pd
import numpy as np
import requests

filepath = "adresses_complete.xlsx"
API_KEY = "AIzaSyAnV5rCE2wBHFyrwoHCD-7kEXGYb3s2g60"
# https://maps.googleapis.com/maps/api/distancematrix/json?origins=Seattle&destinations=San+Francisco&key=YOUR_API_KEY
url = "https://maps.googleapis.com/maps/api/distancematrix/json?"

NOTVALUE = -9
#Use google maps API pour trouver les distances

def get_dist_time(vers, de):
    """
    return distance and time values
    """
    try:
        r = requests.get(f"{url}origins={de}&destinations={vers}&key={API_KEY}")
        # print(r.json())
        dist = r.json()['rows'][0]['elements'][0]['distance']['value']
        time = r.json()['rows'][0]['elements'][0]['duration']['value']
    except (KeyError, TypeError, IndexError):
        return NOTVALUE, NOTVALUE
    return dist, time


def get_matrices(nodes_test):
    """
        Create distance and time matrices
    """
    dim = len(nodes_test)
    matrices_dist = [np.arange(-1, dim) for i in range(dim+1)]
    matrices_time = [np.arange(-1, dim) for i in range(dim+1)]
    for i in range(dim):
        print(f"ligne {i+1}/{dim} ")
        ligne = nodes_test.iloc[i]
        de = f"{ligne['LATITUDE_Y']},{ligne['LONGITUDE_X']}"
        de1 = '+'.join(ligne['ORDER_DESC'].split(" ")) + "+" + ligne['JOBCITY']

        matrices_dist[i+1][0] = i
        matrices_dist[i+1][i+1] = 0
        for j in range(i+1, dim):
            col = nodes_test.iloc[j]
            vers = f"{col['LATITUDE_Y']},{col['LONGITUDE_X']}"
            vers1 = '+'.join(col['ORDER_DESC'].split(" ")
                             ) + "+" + col['JOBCITY']
            dist1, time1 = get_dist_time(de1, vers1)
            dist, time = get_dist_time(de, vers)

            if dist1 == NOTVALUE and dist == NOTVALUE:
                dist2 = NOTVALUE
                time2 = NOTVALUE
            elif dist1 - dist > 1000:
                dist1 = dist
                time1 = time
                dist2, time2 = get_dist_time(vers, de)
            else:
                dist2, time2 = get_dist_time(vers1, de1)

            matrices_dist[i+1][j+1] = dist1
            matrices_dist[j+1][i+1] = dist2
            matrices_time[i+1][j+1] = time1
            matrices_time[j+1][i+1] = time2
    return np.matrix(np.array(matrices_dist)), np.matrix(np.array(matrices_time))


if __name__ == "__main__":
    df = pd.read_excel(filepath)
    print(filepath)

    depots = df[df['Location_type'] != 'customer']
    customers = df[df['Location_type'] == 'customer']
    nodes = df.copy(deep=False)

    depots_loc = depots[['JOB_DESC', 'ORDER_DESC', 'JOBCITY',
                         'Postal_code',	'JOBCITY', 'LATITUDE_Y', 'LONGITUDE_X', 'Location ID']]
    nodes_loc = nodes[['CUST_NBR', 'JOB_DESC', 'ORDER_DESC', 'JOBCITY',
                       'Postal_code',	'LATITUDE_Y', 'LONGITUDE_X', 'Location ID']]
    nodes_loc = nodes_loc.dropna(axis=0, subset=['JOBCITY'])

    dist, time = get_matrices(nodes_loc)

    with open('distances.txt', 'wb') as f:
        for line in dist:
            np.savetxt(f, line, fmt='%2.f')
    with open('times.txt', 'wb') as f:
        for line in time:
            np.savetxt(f, line, fmt='%2.f')
    print("Done")







