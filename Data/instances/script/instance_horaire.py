
#%%
import pandas as pd
import os
import datetime
from collections import Counter
#%%
from driver import Driver

#%%
TICKET_FOLDER = "/".join([os.path.dirname(__file__), "tickets/"])
INSTANCE_FOLDER = "/".join([os.path.dirname(__file__), "instances/"])
#%%
""" Load dataframes """
df_tickets = pd.read_excel(f"{os.path.dirname(__file__)}/DTICKETHISTAB_BETON.xlsx")
#%%
df_orders = pd.read_excel(f"{os.path.dirname(__file__)}/DORDERSTAB_BETON.xlsx",sheet_name='data')
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
df_tickets['WEEK'] = df_tickets['DATE'].apply(lambda x:datetime.date(x.year,x.month,x.day).isocalendar()[1])
df_tickets['DAY'] = df_tickets['DATE'].apply(lambda x:datetime.date(x.year,x.month,x.day).toordinal())
df_orders['DATE'] = df_orders['SHIPDATE'].apply(lambda x:datetime.date(x.year,x.month,x.day))
df_orders['WEEK'] = df_orders['DATE'].apply(lambda x:datetime.date(x.year,x.month,x.day).isocalendar()[1])
df_orders['DAY'] = df_orders['DATE'].apply(lambda x:datetime.date(x.year,x.month,x.day).toordinal())
#%% Clean data
df_tickets.dropna(axis=0,inplace=True)
# # Enlever TRUCK_NBR = ACHATEXT
df_tickets = df_tickets[df_tickets['TRUCK_NBR'] != 'ACHATEXT']
# # Enlever livraison dont cap > 12
df_tickets = df_tickets[df_tickets['QUANTITY']<=12]
# # SHIP_LOC = 099
df_tickets = df_tickets[df_tickets['SHIP_LOC']!=99]
# len(df_tickets)
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
df_driver = df_tickets.loc[:,("DTICKETHIS_TICKET_ID","SHIP_LOC","ORDER_ID","DRIVER_NBR","TRUCK_NBR","SCHED_LOC","LOAD","LOADNR","QUANTITY")]
# Enlever les ordres où le numero du chauffeur/camion est manquant
df_driver.dropna(axis=0,inplace=True)
 # Enlever TRUCK_NBR = ACHATEXT
df_driver = df_driver[df_driver['TRUCK_NBR'] != 'ACHATEXT']
# Enlever livraison dont cap > 12
df_driver = df_driver[df_driver['QUANTITY']<=12]
# print(len(df_driver))
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
    _cap = max(_df['QUANTITY']) 
    if _cap <= 8:
        d.capacity = 8
    else:
        d.capacity = 12 
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
ticket_date = list(dict.fromkeys(df_tickets['WEEK']))
#%%
## Dans le fichier des tickets, récuperer la liste des chauffeurs qui ont effectué les livraisons
# creer un fichier où enregistrer les dépôts, ordres et chauffeurs.
instance_size = dict()
# ticket_date = [datetime.date(2020,10,5)]
for week in ticket_date:
    day_op = df_tickets[df_tickets['WEEK']==week]
    day_op = day_op[["DTICKETHIS_TICKET_ID","SHIP_LOC","ORDER_ID","DRIVER_NBR","TRUCK_NBR","SCHED_LOC","LOAD","LOADNR","QUANTITY","BEGIN_LOAD","FINISH_LOAD","ARRIVE_PLANT","DAY"]]
    nbDays = 0
    with open(f"{INSTANCE_FOLDER}/week{week}.txt", 'w') as file:
        print(f"Week {week}")
        contents = []
        order_count = 0
        # Nombre de jours dans week
        contents.append(f"{len(df_depots)}\n")
        nbDays = len(list(dict.fromkeys(day_op['DAY'])))
        contents.append(f"{nbDays}\n")
        orders_of_day = list(dict.fromkeys(day_op['ORDER_ID']))
        all_orders_of_day = list(dict.fromkeys(df_orders['ORDER_ID'][df_orders['WEEK'] == week ]))
        # print(f"Nombre ordres dans Ticket table: {len(orders_of_day)} ")
        # print(f"Nombre ordres dans Order table: {len(all_orders_of_day)} ")
        real_orders_of_day = list(set(orders_of_day) & set(all_orders_of_day))
        # print(f"{len(real_orders_of_day)} ordres servis")
        # 2 Nombre de clients
        # file.write(f"{len(real_orders_of_day)}\n")
        list_driver = list(dict.fromkeys(day_op['DRIVER_NBR']))
        # print(f"Nombre livreurs utilisés: {len(list_driver)}")
        list_trucks = list(dict.fromkeys(day_op['TRUCK_NBR']))
        # print(f"Nombre trucks utilisés: {len(list_trucks)}")
        # 3 Nombre de chauffeurs
        # file.write(f"{len(list_driver)}\n")
        contents.append(f"{len(list_driver)}\n")

        # 4 Liste des depots
        # for i,d in enumerate(list_depot):
        #     contents.append(f"{i} {df_depots.loc[d]['Location ID']} {d} {df_depots_loc.loc[d]['CAPACITY']}\n")  
        for i, dep in enumerate(df_depots.index):
            contents.append(
                f"{i} {df_depots.loc[dep]['Location ID']} {dep} {df_depots_loc.loc[dep]['CAPACITY']} \n")
        
        # 5 Liste des clients de la journée  
        for i,ord in enumerate(orders_of_day):
            # print(f"Operations for order {ord}")
            ord_seq = day_op[day_op['ORDER_ID']==ord]
            ordres = df_orders[df_orders['ORDER_ID']==ord]
            if len(ordres) == 0: 
                continue          
            # Pouring minutes
            ord_seq_ticket_id = list(dict.fromkeys(ord_seq['DTICKETHIS_TICKET_ID'] ))
            # print(ord_seq_ticket_id)
            # filter le ticket selon ord
            # Get ticket for ord_seq
            pouring_duration = []
            pouring_minutes = 3
            for id in ord_seq_ticket_id:
                ticket_schedule =  ord_seq[ord_seq['DTICKETHIS_TICKET_ID'] == id]
                temp = ticket_schedule["FINISH_LOAD"]-ticket_schedule["BEGIN_LOAD"]
                _max = max(temp)
                if _max.days >= 0:
                    pouring_duration.append(_max)
            if len(pouring_duration) > 0:
                pouring_minutes = round(max(pouring_duration).seconds/60,2)
            if pouring_minutes < 0.5:
                pouring_minutes = 3
            for j in range(len(ordres)):
                ordre = ordres.iloc[j]
                contents.append(f"{order_count} {ordre['DAY']} {ord} {ordre['CUST_NBR']} {ordre['LOCATION ID']} {ordre['SCHED_LOC']} {ordre['SHIPTIME (min)']:.0f} {ordre['QUANTITY']:.2f} \n")

                order_count = order_count + 1
            
        # 6 Ajouter les chauffeurs
        # Lines of drivers: driver id, driver number, driver rank, depot assigned driver max capacity
        for i,id in enumerate(list_driver):
            dr = dict_driver[id]
            _data = day_op[day_op['DRIVER_NBR']==dr.nbr][["BEGIN_LOAD","ARRIVE_PLANT"]]
            _data.sort_values(["BEGIN_LOAD"],inplace=True)
            # print(dr)
            # _data = test[]
            begin, end = _data.iloc[0,0],_data.iloc[-1,1]
            contents.append(f"{i} {dr.nbr} {dr.rank} {dr.depot_nbr} {dr.factory_nbr} {dr.capacity:.0f} \n")
            

        # print(order_count)
        contents.insert(1,str(order_count)+'\n')
        contents[-1] = contents[-1].rstrip()
        contents = "".join(contents)
        file.write(contents)
        instance_size[week] = [nbDays, len(df_depots), order_count, len(list_driver)]
        
# Create file with instances size
df =pd.DataFrame.from_dict(instance_size, orient='index')
df.columns = ['nbDays','depot','order','driver']
df.to_csv('instance_size.csv')
# %%
