
import pandas as pd
import os
import math
from driver import Driver
import datetime
from collections import Counter

INSTANCE_FOLDER = "/".join([os.path.dirname(os.getcwd()), "createInstance/instances/"])
INSTANCE_FOLDER_CQ = "/".join([os.path.dirname(os.getcwd()), "createInstance/cq/"])
DATA_FOLDER = "/".join([os.path.dirname(os.getcwd()), "createInstance/data/"])

Instances_with_extern_fleet   = set()  
Instances_with_intern_fleet = set ()


def create_instance_CQ(Object,jour)->None:
    day = jour.day
    if day < 10:
        day = f"0{day}"
    print(f"Instance {jour}")
    list_ordres = list(set( Object.df_orders[Object.df_orders['DATE']==jour][Object.df_orders['JOB_DESC']!='PICKUP']['ORDER_ID']))
    tickets = Object.df_tickets[Object.df_tickets['ORDER_ID'].isin(list_ordres)]
    ordres = Object.df_orders[Object.df_orders['ORDER_ID'].isin(list_ordres)]
    sortedMap = dict()
    totalDemand = 0
    for ord in list_ordres:
        ordered_prod =  list(set(ordres[ordres['ORDER_ID']==ord]['PROD_NBR']))
        delivered_prod =  list(set(tickets[tickets['ORDER_ID']==ord]['PROD_NBR']) & set(ordered_prod))
        if len(delivered_prod) == 0:
            continue
        cur =  tickets[tickets['PROD_NBR'].isin(delivered_prod)][tickets['ORDER_ID']==ord]
        # print(ord,' ',ordered_prod, ' ',delivered_prod,' ',cur['QUANTITY'].sum())
        totalDemand = totalDemand + cur['QUANTITY'].sum()
        sortedMap[ord] = (cur['QUANTITY'].sum(), ordres[ordres['ORDER_ID']==ord]['SHIPTIME (min)'].min(),len(delivered_prod))

    sortedMap = sorted(sortedMap.items(), key=lambda x:(x[1][0],x[1][1],x[1][2]))

    drivers = list(dict.fromkeys(tickets['DRIVER_NBR']))
    dict_driver = {}
    usedLoc = set()
    for driver in drivers:
        trucks = list(dict.fromkeys(tickets['TRUCK_NBR'][tickets['DRIVER_NBR']==driver]))    
        
        if int(driver) == 1:
            for t in trucks:
                d = Driver()
                df_drv = tickets[tickets['TRUCK_NBR']==t]
                _data = df_drv[["BEGIN_LOAD", "ARRIVE_PLANT"]].copy()
                _data.sort_values(["BEGIN_LOAD"], inplace=True)
                begin, end = _data.iloc[0, 0], _data.iloc[-1, 1]
                a = (begin.hour * 60 + begin.minute - 10)
                a =  a - a%5
                d.start_shift = a
                d.factory_nbr = Counter(df_drv['SHIP_LOC']).most_common(1)[0][0]
                d.t_nbr = t
                d.d_nbr = int(driver)
                _cap = max(df_drv['QUANTITY'])
                if _cap <= 8:
                    d.capacity = 8
                else:
                    d.capacity = 12
                dict_driver[d.t_nbr] = d

        elif len(trucks) == 1:
            d  = Driver()
            df_drv = tickets[tickets['TRUCK_NBR'].isin(trucks)]
            _data = df_drv[["BEGIN_LOAD", "ARRIVE_PLANT"]].copy()
            _data.sort_values(["BEGIN_LOAD"], inplace=True)
            begin, end = _data.iloc[0, 0], _data.iloc[-1, 1]
            a = (begin.hour * 60 + begin.minute - 10)
            a =  a - a%5
            d = Driver()
            d.factory_nbr = Counter(df_drv['SHIP_LOC']).most_common(1)[0][0]
            d.start_shift = a
            _cap = max(df_drv['QUANTITY'])
            if _cap <= 8:
                d.capacity = 8
            else:
                d.capacity = 12

            d.t_nbr = trucks[0]
            d.d_nbr = int(driver)
            dict_driver[d.t_nbr] = d       
        else:
            d = Driver()
            df_drv = tickets[tickets['TRUCK_NBR'].isin(trucks)]
            _data = df_drv[["BEGIN_LOAD", "ARRIVE_PLANT"]].copy()
            _data.sort_values(["BEGIN_LOAD"], inplace=True)
            begin, end = _data.iloc[0, 0], _data.iloc[-1, 1]
            a = (begin.hour * 60 + begin.minute - 10)
            a =  a - a%5
            d = Driver()
            d.factory_nbr = Counter(df_drv['SHIP_LOC']).most_common(1)[0][0]
            d.start_shift = a
            _cap = max(df_drv['QUANTITY'])
            if _cap <= 8:
                d.capacity = 8
            else:
                d.capacity = 12
            d.t_nbr = '|'.join(trucks)
            d.d_nbr = int(driver)
            dict_driver[d.t_nbr] = d
        usedLoc.add(d.factory_nbr)             
                
    contents = []
    count = 0
    for dep in usedLoc:    
        if dep not in Object.df_depots.index:
            continue
        Object.depot_loc_id[dep] = count
        count = count + 1

    #  Ajouter les chauffeurs
    # Lines of drivers: driver id, driver number, driver rank, depot assigned driver max capacity
    driver_cap = set()
    dict_driver = dict(sorted(dict_driver.items(), key=lambda x:(x[1].capacity,x[1].factory_nbr,x[1].start_shift)))

    contents.append(f"Drivers:  {len(dict_driver):8}\n")

    for i, id in enumerate(dict_driver):
        
        dr = dict_driver[id]
        contents.append(
            f"{i:3} {dr.d_nbr:8} {'_'.join(dr.t_nbr.split(' ')):8} {dr.rank:6} {Object.depot_loc_id[dr.factory_nbr]:2} {dr.capacity:2.0f}"
            f" { dr.start_shift :4}\n")
        driver_cap.add(dr.capacity)

        if dr.d_nbr == 1 or dr.d_nbr == 999:
            Instances_with_extern_fleet.add(str(jour))
    if str(jour) not in Instances_with_extern_fleet:
                Instances_with_intern_fleet.add(str(jour))
    dmd_total = 0
    clientCount = 0
    new_orders = list()
    for i, values in enumerate(sortedMap):
        ord = values[0]
        ordered_prod =  list(set(ordres[ordres['ORDER_ID']==ord]['PROD_NBR']))
        delivered_prod =  list(set(tickets[tickets['ORDER_ID']==ord]['PROD_NBR']) & set(ordered_prod))
        if len(delivered_prod) == 0:
            continue
        ticket_ordres = tickets[tickets['PROD_NBR'].isin(delivered_prod)][tickets['ORDER_ID'].isin([ord])]
        
        dmd_total = dmd_total + ticket_ordres['QUANTITY'].sum()
        ordre = ordres[ordres['ORDER_ID']==ord].iloc[0]
        
        cur_order_count = 0
        for val in delivered_prod:
            if ticket_ordres[ticket_ordres['PROD_NBR'] == val]['QUANTITY'].sum() > 0:
                cur_order_count = cur_order_count + 1
        dep = Counter(ticket_ordres['SCHED_LOC']).most_common(1)[0][0]
        string = f"{clientCount:3} {ord} {ordre['CUST_NBR']:6} {ordre['LOCATION ID']:4} {Object.depot_loc_id[dep]:2} {ordre['SHIPTIME (min)']:4.0f} {ticket_ordres['QUANTITY'].sum():5.1f} {cur_order_count:2} \n"
        clientCount = clientCount + 1
        contents.append(string)
        new_orders.append(ord)
    contents.insert(-clientCount, f"Clients:  {clientCount} {dmd_total}\n")
            
    print('Client Demand ',dmd_total,end=' ')

    order_count = 0    
    sum_order_dmd = 0
    for clientID, ord in enumerate(new_orders):

        ord_seq = tickets[tickets['ORDER_ID'] == ord]
        prod_nbrs = list(dict.fromkeys(ordres['PROD_NBR']))
        
            
        for prod in prod_nbrs:
            ticket_ordres = tickets[tickets['ORDER_ID'] == ord]
            
            ticket_ordres =  ord_seq[ord_seq['PROD_NBR']==prod]
            if ticket_ordres['QUANTITY'].sum() < 1:
                continue
            sum_order_dmd = sum_order_dmd + ticket_ordres['QUANTITY'].sum()
#                     if len(list(dict.fromkeys(ticket_ordres['SCHED_LOC']))) > 1:
#                         print(ord,' ',prod,' ', list(dict.fromkeys(ticket_ordres['SCHED_LOC'])),' ',Counter(ticket_ordres['SCHED_LOC']).most_common(1)[0][0])
#                     contents.append(f"{order_count:3} {clientID:3} {ticket_ordres['QUANTITY'].sum():5.1f} {Counter(ordres['SCHED_LOC']).most_common(1)[0][0]} {Counter(ticket_ordres['SCHED_LOC']).most_common(1)[0][0]}  \n")
            dep = Counter(ticket_ordres['SCHED_LOC']).most_common(1)[0][0]
            contents.append(f"{order_count:3} {clientID:3} {ticket_ordres['QUANTITY'].sum():5.1f}  {Object.depot_loc_id[dep]}  \n")
            
            order_count = order_count + 1
            
    contents.insert(-order_count, f"Orders:  {order_count}  \n")
    
    print('order_demand ',sum_order_dmd)
    contents.append(f"Plants:   {len(usedLoc)}\n")

    # 4 Liste des depots
             
    for i, dep in enumerate(usedLoc):
        contents.append(
            f"{i:2} {Object.df_depots.loc[dep]['Location ID']} {dep:2} {int(Object.df_depots_loc.loc[dep]['CAPACITY']):3} \n")
#                 Object.depot_loc_id[dep] = i


    with open(f"{INSTANCE_FOLDER_CQ}/{jour.year}{jour.month}{day}.txt", 'w') as file:
        contents[-1] = contents[-1].rstrip()
        contents = "".join(contents)
        file.write(contents)
    Object.instance_size[f"{jour.year}{jour.month}{day}"] = [len(usedLoc), clientCount,
                                                                   order_count,
                                                                   len(dict_driver), dmd_total, len(driver_cap)]
    pd.DataFrame.from_dict(Object.instance_size, orient='index').to_csv(f'{INSTANCE_FOLDER_CQ}/instance_size.csv',
                                                                            sep=';',decimal='.',header=0)


def create_all_instances_CQ(Object) -> None:
    print("Create all CQ instances")
    Instances_with_extern_fleet   = set()  
    Instances_with_intern_fleet = set ()
    Object.instance_size = dict()
    Object.instance_size[f"date"]=['depot','client','order','driver','demand','cap']
    ticket_date = list(dict.fromkeys(Object.df_tickets['DATE']))
    # ticket_date = [datetime.date(2020, 10, 10),datetime.date(2020, 10, 1)]
    for jour in ticket_date:
        create_instance_CQ(Object,jour)         