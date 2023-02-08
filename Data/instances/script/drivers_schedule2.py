
#%%
import pandas as pd
import os
import datetime
from collections import Counter
#%%
from driver import Driver

#%%
TICKET_FOLDER = "/".join([os.path.dirname(__file__), "tickets/"])
INSTANCE_FOLDER = "/".join([os.path.dirname(__file__), "instance2/"])
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

df_tickets = df_tickets[df_tickets['DRIVER_NBR']!=1]
# len(df_tickets)
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
# Find dummy driver (occasional drivers)
employee_nbr=df_employee.index
dummy_driver=[]
for nbr in list_driver:
    if nbr not in employee_nbr:
       dummy_driver.append(nbr)
#%%

"""
Matcher les livreurs de list_driver avec les employés de CQ
"""
for i,emp_nbr in enumerate(df_employee.index):
    emp = df_employee.loc[emp_nbr]
    d = Driver()
    if emp_nbr in dict_driver:
        d = dict_driver[emp_nbr]
        d.id = i
        d.rank = emp['emp_rang']
        d.description = emp['sch_description']
        d.depot_nbr = emp['emp_usi_numero']
    else:
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
# ticket_date = [datetime.date(2020,10,5)]
instance_size = dict()
count = 0
for week in ticket_date:
    week_op = df_tickets[df_tickets['WEEK']==week]

    list_days = list(dict.fromkeys(week_op['DATE']))
    week_op = week_op[["DTICKETHIS_TICKET_ID","SHIP_LOC","ORDER_ID","DRIVER_NBR","TRUCK_NBR","SCHED_LOC","LOAD","LOADNR","QUANTITY","BEGIN_LOAD","FINISH_LOAD","ARRIVE_PLANT","DAY","DATE"]]
    print(f"Week {week}")
    contents = []
    for jour in list_days:
        day_op = week_op[week_op['DATE']==jour]

        nbDays = len(list(dict.fromkeys(day_op['DAY'])))
        orders_of_day = list(dict.fromkeys(day_op['ORDER_ID']))
        list_driver = list(dict.fromkeys(day_op['DRIVER_NBR']))
        list_trucks = list(dict.fromkeys(day_op['TRUCK_NBR']))
        check=any(item in list_driver for item in dummy_driver)
        if  not check:
           continue

        for i,id in enumerate(list_driver):
            dr = dict_driver[id]
            _data = day_op[day_op['DRIVER_NBR']==dr.nbr][["BEGIN_LOAD","ARRIVE_PLANT"]]
            _data.sort_values(["BEGIN_LOAD"],inplace=True)
            begin, end = _data.iloc[0,0],_data.iloc[-1,1]
            instance_size[count] = [ f"{week}",f"{jour}",f"{dr.nbr}",  
                f"{begin.time()}".split('.')[0],f"{end.time()}".split('.')[0]]
            count = count + 1
    
# Create file with instances size
pd.DataFrame.from_dict(instance_size, orient='index',columns = ['week','jour','driver','begin','end']).to_csv('drivers_schedule.csv')
# %%
