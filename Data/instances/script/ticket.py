"""Ticket."""
#%%
import pandas as pd
import os
import datetime
from collections import Counter
#%%
from driver import Driver
#%%
def time_to_minute(stamps):

    return round(stamps.hour*60 + stamps.minute + stamps.second/60,2)

#%%
TICKET_FOLDER = "/".join([os.path.dirname(__file__), "tickets/"])
INSTANCE_FOLDER = "/".join([os.path.dirname(__file__), "instances/"])
#%%
""" Load dataframes """
df_tickets = pd.read_excel(f"{os.path.dirname(__file__)}/DTICKETHISTAB_BETON.xlsx")
#%%
df_orders = pd.read_excel(f"{os.path.dirname(__file__)}/DORDERSTAB_BETON.xlsx")
#%%
df_employee = pd.read_excel(f"{os.path.dirname(__file__)}/EMPLOYETAB.xlsx")
df_employee.drop(['COMP_NBR', 'emp_sch_specificite'], axis=1, inplace=True)
df_employee.set_index('EMPL_NBR',inplace=True)
#%%
df_depots = pd.read_excel(
    f"{os.path.dirname(__file__)}/adresses_complete.xlsx")
df_depots = df_depots[df_depots['Location_type'] != 'customer']
df_depots.dropna(axis=1,  inplace=True)
df_depots['NBR'] = df_depots['JOB_DESC'].apply(
    lambda x: x if len(x.split(' ')) == 1 else int(x.split(' ')[1]))
df_depots.set_index('NBR',inplace=True)
#%%
# Depot location
df_depots_loc = pd.read_excel(
    f"{os.path.dirname(__file__)}/LOCATIONTAB.xlsx")
df_depots_loc['LOC_NBR'] = df_depots_loc['LOC_NBR'].apply(lambda x: int(x))
df_depots_loc.set_index('LOC_NBR',inplace=True)
#%%
# Les tickets contiennent les séquences des opérations de distribution
# Un ticket est la livraison de béton sur un chantier par un livreur

# Je dois déterminer la séquence de livraison par jour.
df_tickets['DATE'] = df_tickets['TICKET_DATE'].apply(lambda x:datetime.date(x.year,x.month,x.day))
df_orders['DATE'] = df_orders['SHIPDATE'].apply(lambda x:datetime.date(x.year,x.month,x.day))
#%% Clean data
df_tickets.dropna(axis=0,inplace=True)
# Enlever TRUCK_NBR = ACHATEXT
df_tickets = df_tickets[df_tickets['TRUCK_NBR'] != 'ACHATEXT']
# Enlever livraison dont cap > 12
df_tickets = df_tickets[df_tickets['QUANTITY']<=12]
# SHIP_LOC = 099
df_tickets = df_tickets[df_tickets['SHIP_LOC']!=99]
#%%
# df_tickets[df_tickets['SHIP_LOC']=='99']
# list(dict.fromkeys(df_tickets['SHIP_LOC']))
#%%
"""
   Dans mes fichiers d'instance je dois mettre les chauffeurs réellement disponibles dans la journée.
   Mais dans le fichier ticket les numeros des chauffeurs ne sont pas tous présents.
   Mais si j'arrive à trouver la capacité de chaque véhicule utilisé ça aidera 

   Il y a des véhicules de capacité > 8 dans les données!!! 

   Il y a des véhicules qui n'existent pas dans le fichier des véhicules

   Il y a des employés qui n'existent pas dans le fichier des employés 
"""
#%%
# Liste des livreurs disponibles dans le fichier Ticket.
df_driver = df_tickets[["DTICKETHIS_TICKET_ID","SHIP_LOC","ORDER_ID","DRIVER_NBR","TRUCK_NBR","SCHED_LOC","LOAD","LOADNR","QUANTITY"]]
# Enlever les ordres où le numero du chauffeur/camion est manquant
df_driver.dropna(axis=0,inplace=True)
 # Enlever TRUCK_NBR = ACHATEXT
df_driver = df_driver[df_driver['TRUCK_NBR'] != 'ACHATEXT']
# Enlever livraison dont cap > 12
df_driver = df_driver[df_driver['QUANTITY']<=12]
# SHIP_LOC = 099
df_driver = df_driver[df_driver['SHIP_LOC']!=99]
print(len(df_driver))
list_driver_nbr = list(dict.fromkeys(df_driver['DRIVER_NBR']))
#%%
"""
 Recuperer capacité des livreurs disponibles. Numero des usines d'où ces livreurs partent le plus.
"""
list_driver = []
dict_driver = {}
list_cap = []
for nbr in list_driver_nbr:
    d = Driver() 
    d.nbr = int(nbr)
    _df =  df_driver[df_driver['DRIVER_NBR']==nbr]
    d.capacity = max(_df['QUANTITY']) 
    d.factory_nbr =  Counter(_df['SHIP_LOC']).most_common(1)[0][0]
    list_cap.append(d.capacity)
    list_driver.append(d)
    dict_driver[d.nbr] = d
#%%
"""
Matcher les livreurs de list_driver avec les employés de CQ
"""
for i,emp_nbr in enumerate(df_employee.index):
    emp = df_employee.loc[emp_nbr]
    if emp_nbr in dict_driver:
        d = dict_driver[emp_nbr]
        d.id = i
        d.rank = emp['emp_rang']
        d.description = emp['sch_description']
        d.depot_nbr = emp['emp_usi_numero']
    else:
        d = Driver()
        d.id = i
        d.nbr = emp_nbr
        d.rank = emp['emp_rang']
        d.description = emp['sch_description']
        d.capacity = 8
        if 'semi' in d.description:
            d.capacity = 12   
        d.factory_nbr = d.depot_nbr = emp['emp_usi_numero']
        dict_driver[d.nbr] = d
        list_driver.append(d)
#%%
ticket_date = list(dict.fromkeys(df_tickets['DATE']))
#%%
## Dans le fichier des tickets, récuperer la liste des chauffeurs qui ont effectué les livraisons
# creer un fichier où enregistrer les dépôts, ordres et chauffeurs.
instance_size = dict()
# ticket_date=[datetime.date(2020, 11, 30)]
for jour in ticket_date:
    day_op = df_tickets[df_tickets['DATE']==jour]
    day_op = day_op[["DTICKETHIS_TICKET_ID","SHIP_LOC","ORDER_ID","DRIVER_NBR","TRUCK_NBR","SCHED_LOC","LOAD","LOADNR","QUANTITY","BEGIN_LOAD","FINISH_LOAD","TO_JOB","ON_JOB","BEGIN_POUR", "FINISH_POUR","TO_PLANT", "ARRIVE_PLANT"]]
    with open(f"{TICKET_FOLDER}/{jour}.txt", 'w') as file:
        print(jour)
        list_depot = list(dict.fromkeys(day_op['SHIP_LOC']))
        orders_of_day = list(dict.fromkeys(day_op['ORDER_ID']))
        all_orders_of_day = list(dict.fromkeys(df_orders['ORDER_ID'][df_orders['DATE'] == jour ]))
        print(f"Nombre ordres dans Ticket table: {len(orders_of_day)} ")
        print(f"Nombre ordres dans Order table: {len(all_orders_of_day)} ")
        real_orders_of_day = list(set(orders_of_day) & set(all_orders_of_day))
        print(f"{len(real_orders_of_day)} ordres servis")
        list_driver = list(dict.fromkeys(day_op['DRIVER_NBR']))
        list_trucks = list(dict.fromkeys(day_op['TRUCK_NBR']))
         
        # 5 Liste des clients de la journée  
        for i,ord in enumerate(orders_of_day):
            ord_seq = day_op[day_op['ORDER_ID']==ord]
            ordre = df_orders[df_orders['ORDER_ID']==ord]
            if len(ordre) == 0: 
                continue
            print(f"Operations for order {ord}")
            file.write(f"Operations for order {ord}\n")
            ordre = ordre.iloc[0]
            ord_seq_ticket_id = list(dict.fromkeys(ord_seq['DTICKETHIS_TICKET_ID'] ))
            pouring_duration = []
            pouring_minutes = 3
            for j,id in enumerate(ord_seq_ticket_id):
                ticket_sched =  ord_seq[ord_seq['DTICKETHIS_TICKET_ID'] == id]
                ticket_sched =ticket_sched[["ORDER_ID", "BEGIN_LOAD","FINISH_LOAD","TO_JOB","ON_JOB", "BEGIN_POUR","FINISH_POUR","TO_PLANT","ARRIVE_PLANT", "DRIVER_NBR","TRUCK_NBR","SCHED_LOC","QUANTITY"]]
                # print (" j, BEGIN_LOAD","FINISH_LOAD","TO_JOB","ON_JOB")
                #    "BEGIN_POUR","FINISH_POUR","TO_PLANT",  "ARRIVE_PLANT", DRIVER_NBR",
                #  "TRUCK_NBR","SCHED_LOC","QUANTITY"
               
                file.write(f"{j} { time_to_minute( max(ticket_sched['BEGIN_LOAD']) )} {time_to_minute(max(ticket_sched['FINISH_LOAD']))}  {time_to_minute(max(ticket_sched['FINISH_LOAD']))}  {time_to_minute(max(ticket_sched['FINISH_LOAD']))}  {time_to_minute(max(ticket_sched['TO_JOB']))}  {time_to_minute(max(ticket_sched['ON_JOB']))}  {time_to_minute(max(ticket_sched['BEGIN_POUR']))}  {time_to_minute(max(ticket_sched['FINISH_POUR']))}  {time_to_minute(max(ticket_sched['TO_PLANT']))}  {time_to_minute(max(ticket_sched['ARRIVE_PLANT']))}  {max(ticket_sched['DRIVER_NBR']):.0f}  {max(ticket_sched['TRUCK_NBR'])} {max(ticket_sched['SCHED_LOC'])} {max(ticket_sched['QUANTITY'])} \n")
            
            
       
        # instance_size[jour] = [len(list_depot), len(real_orders_of_day), len(list_driver)]
# %%
df =pd.DataFrame.from_dict(instance_size, orient='index')
df.columns = ['depot','order','driver']
df.to_csv('instance_size.csv')
#%%

# %%
