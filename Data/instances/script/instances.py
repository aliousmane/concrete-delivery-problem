
"""Create instances."""

#%%
import pandas as pd
import numpy as np
import os
from driver import Driver
from nodes import Node

INSTANCE_FOLDER = "/".join([os.path.dirname(__file__), "instances/"])

#%%
# Load dataframes
df_nodes = pd.read_excel(f"{os.path.dirname(__file__)}/DORDERSTAB_BETON.xlsx")
df_depots = pd.read_excel(
    f"{os.path.dirname(__file__)}/adresses_complete.xlsx")
df_depots = df_depots[df_depots['Location_type'] != 'customer']
df_driver = pd.read_excel(f"{os.path.dirname(__file__)}/EMPLOYETAB.xlsx")
df_driver.drop(['COMP_NBR', 'emp_sch_specificite'], axis=1, inplace=True)
df_tickets = pd.read_excel(
    f"{os.path.dirname(__file__)}/DTICKETHISTAB.xlsx")
#%%

df_nodes.columns

#%%
print(f" {len(df_nodes)} ordres")
print(f" {len(df_driver)} chauffeurs")
print(f" {len(df_depots)} depots")
# df_nodes.head()
#%%
# Load depots nodes
depots = df_depots.copy()
depots.dropna(axis=1, how='all', inplace=True)
depots['JOB_DESC'] = depots['JOB_DESC'].apply(
     lambda x: x if len(x.split(' ')) == 1 else int(x.split(' ')[1]))
list_depots = []
depots_loc = []
for i in range(len(depots)):
    d = Node(id=i)
    d.adresse = depots.loc[i:,('ORDER_DESC')]
    d.location_id = depots.iloc[i]['Location ID']
    d.location_type = depots.iloc[i]['Location_type']
    d.coord = (depots.iloc[i]['LONGITUDE_X'], depots.iloc[i]['LATITUDE_Y'])
    d.depot_loc = depots.iloc[i]['JOB_DESC']

    depots_loc.append(d.depot_loc)
    list_depots.append(d)

#%%
df_driver_subset = pd.merge(
    df_driver, depots, how='inner', left_on='emp_usi_numero', right_on='JOB_DESC')
print(f" {len(df_driver_subset)} Vs {len(df_driver)} au total")

#%% Load Drivers
# take only drivers with ORDER_LOC in depots
df_driver_subset.sort_values(["emp_rang"], inplace=True)
list_drivers = []
for i in range(min(5, len(df_driver_subset))):
    cur = df_driver_subset.iloc[i]
    d = Driver()
    d.id = i
    d.nbr = cur['EMPL_NBR']
    d.rank = cur['emp_rang']
    d.description = cur['sch_description']
    if 'betoniere' in d.description:
        d.capacity = 8
    else:
        d.capacity = 12
    d.factory_nbr = cur['emp_usi_numero']
    list_drivers.append(d)
#%%
df_nodes.columns
#%%
list_order = df_nodes[:]
list_order.drop(['ORDER_ID', 'ORDER_LOC', 'ORDER_NBR', 'ORDER_STATUS', 'ORDER_DATE', 'ORDER_TIME (min)', 'Day of the week ID', 'SHIPTIME (hour)', 'ZONE_NBR', 'ZONE_NBR', 'WORK_TYPE', 'SCHED_LOC', 'ORDER_DESC ID', 'TRUCK_REQUEST', 'PROD_NBR_ID', 'CATEGORY_CODE', 'Unnamed: 20', 'Unnamed: 23', 'Product nbr', 'Unnamed: 25', 'Location ', 'ID', 'Unnamed: 28', 'Day', 'id.1'], axis=1, inplace=True)
list_order.dropna(axis=1, how='all', inplace=True)
list_order.columns
ship_date = list(set(list_order['SHIPDATE']))
## On dispose de 51 jours de livraison
# Chaque instance correspondra à une journée
#%%
# Trier les instances par journée de travail
for jour in ship_date:
    ordres = list_order[list_order['SHIPDATE'] == jour]
    instance_name = str(jour).split(' ')[0].replace('-', '')
    with open(f"{INSTANCE_FOLDER}/{instance_name}.txt", 'w') as file:
        # 1 Nombre de dépôts
        file.write(f"{len(list_depots)}\n")
        # 2 Nombre de clients
        file.write(f"{len(ordres)}\n")
        # 3 Nombre de chauffeurs
        file.write(f"{len(list_drivers)}\n")
        # 4 Liste des depots
        for d in list_depots:
            file.write(
                f"{d.id} {d.location_id} {d.depot_loc} \n")  # {d.coord[0]:.3f}  {d.coord[1]:.3f}   \n")
        # 5 Liste des clients de la journée
        print(f'{instance_name}: {len(ordres)}')
        for i in range(len(ordres)):
            #Index, Customer number, Location ID Ship time, "UNLOAD_MINUTES" QUANTITY, "Coordonnées"
            ordre = ordres.iloc[i]
            # {ordre['LONGITUDE_X']:.3f} {ordre['LATITUDE_Y']:.3f} \n")
            file.write(
                f"{i} {ordre['CUST_NBR']} {ordre['LOCATION ID']:.0f} {ordre['SHIPTIME (min)']:.0f} {ordre['QUANTITY']} \n")

        # 6 Ajouter les chauffeurs
        # Lines of drivers: drive id, driver number, driver rank, depot assigned driver max capacity
        for dr in list_drivers:
            file.write(
                f"{dr.id} {dr.nbr} {dr.rank} {dr.factory_nbr} {dr.capacity} \n")