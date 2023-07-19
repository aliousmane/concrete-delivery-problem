"""Create instances."""

import pandas as pd
import os
from driver import Driver
import datetime
from collections import Counter

INSTANCE_FOLDER = "/".join([os.path.dirname(__file__), "instances/"])
DATA_FOLDER = "/".join([os.path.dirname(__file__), "data/"])


class Instance:
    def __init__(self) -> None:
        self.df_orders = pd.read_excel(f"{DATA_FOLDER}/DORDERSTAB_BETON.xlsx", engine='openpyxl', sheet_name="data")
        self.df_depots = pd.read_excel(f"{DATA_FOLDER}/adresses_complete.xlsx")
        self.df_tickets = pd.read_excel(f"{DATA_FOLDER}/DTICKETHISTAB.xlsx")
        self.df_employee = pd.read_excel(f"{DATA_FOLDER}/EMPLOYETAB.xlsx", engine='openpyxl')
        self.df_depots_loc = pd.read_excel(f"{DATA_FOLDER}/LOCATIONTAB.xlsx", engine='openpyxl')
        self.df_driver = None
        self.list_driver_nbr = []
        self.instance_size = dict()
        self.dict_driver = {}
        self.depot_loc_id = {}
        self.clean_depot()
        self.clean_employee()
        self.clean_tickets()
        self.get_driver_from_ticket()

    def clean_depot(self) -> None:
        print("Clean Depot")
        self.df_depots = self.df_depots[self.df_depots['Location_type'] != 'customer']
        self.df_depots.dropna(axis=1, inplace=True)
        self.df_depots['NBR'] = self.df_depots['JOB_DESC'].apply(
            lambda x: x if len(x.split(' ')) == 1 else int(x.split(' ')[1]))
        self.df_depots.set_index('NBR', inplace=True)

        self.df_depots_loc['LOC_NBR'] = self.df_depots_loc['LOC_NBR'].apply(lambda x: int(x))
        self.df_depots_loc.set_index('LOC_NBR', inplace=True)

    def clean_employee(self) -> None:
        self.df_employee.drop(['COMP_NBR', 'emp_sch_specificite'], axis=1, inplace=True)
        self.df_employee.set_index('EMPL_NBR', inplace=True)

    def clean_tickets(self) -> None:
        # Les tickets contiennent les séquences des opérations de distribution
        # Un ticket est la livraison de béton sur un chantier par un livreur

        # Je dois déterminer la séquence de livraison par jour.
        self.df_tickets['DATE'] = self.df_tickets['TICKET_DATE'].apply(lambda x: datetime.date(x.year, x.month, x.day))
        self.df_orders['DATE'] = self.df_orders['SHIPDATE'].apply(lambda x: datetime.date(x.year, x.month, x.day))

        self.df_tickets.dropna(axis=0, inplace=True)
        # # Enlever TRUCK_NBR = ACHATEXT
        # ! self.df_tickets = self.df_tickets[self.df_tickets['TRUCK_NBR'] != 'ACHATEXT']
        # # Enlever livraison dont cap > 12
        self.df_tickets = self.df_tickets[self.df_tickets['QUANTITY'] <= 12]
        # # SHIP_LOC = 099
        self.df_tickets = self.df_tickets[self.df_tickets['SHIP_LOC'] != 99]

        # ! self.df_tickets = self.df_tickets[self.df_tickets['DRIVER_NBR'] != 1]
        # ! self.df_tickets = self.df_tickets[self.df_tickets['DRIVER_NBR'] != 999]

    def get_driver_from_ticket(self) -> None:
        # Liste des livreurs disponibles dans le fichier Ticket.
        self.df_driver = self.df_tickets.loc[:, ("DTICKETHIS_TICKET_ID", "SHIP_LOC", "ORDER_ID", "DRIVER_NBR",
                                                 "TRUCK_NBR", "SCHED_LOC", "LOAD", "LOADNR", "QUANTITY")]
        # Enlever les ordres où le numero du chauffeur/camion est manquant
        self.df_driver.dropna(axis=0, inplace=True)
        # Enlever TRUCK_NBR = ACHATEXT
        # ! self.df_driver = self.df_driver[self.df_driver['TRUCK_NBR'] != 'ACHATEXT']
        # Enlever livraison dont cap > 12
        self.df_driver = self.df_driver[self.df_driver['QUANTITY'] <= 12]

        df_copy = self.df_driver[['TRUCK_NBR', 'DRIVER_NBR']].copy()
        df_copy.reset_index(inplace=True)
        df_copy.set_index(['TRUCK_NBR', 'DRIVER_NBR'], inplace=True)
        self.list_driver_nbr = list(dict.fromkeys(df_copy.index))
        employee_nbr = self.df_employee.index
        dummy_driver = []
        for (t_nbr, d_nbr) in self.list_driver_nbr:
            if d_nbr not in employee_nbr:
                dummy_driver.append(d_nbr)
            d = Driver()
            d.t_nbr = t_nbr
            d.d_nbr = int(d_nbr)
            _df = self.df_driver[self.df_driver['TRUCK_NBR'] == t_nbr]
            _cap = max(_df['QUANTITY'])
            if _cap <= 8:
                d.capacity = 8
            else:
                d.capacity = 12
            d.factory_nbr = Counter(_df['SHIP_LOC']).most_common(1)[0][0]
            self.dict_driver[d.t_nbr] = d

    def show(self):
        print(f" {len(self.df_orders)} ordres")
        print(f" {len(self.df_driver)} chauffeurs")
        print(f" {len(self.df_depots)} depots")

    def create_instance(self, jour) -> None:
        day_op = self.df_tickets[self.df_tickets['DATE'] == jour]
        day_op = day_op[
            ["DTICKETHIS_TICKET_ID", "SHIP_LOC", "ORDER_ID", "DRIVER_NBR", "TRUCK_NBR", "SCHED_LOC", "LOAD", "LOADNR",
             "QUANTITY", "BEGIN_LOAD", "FINISH_LOAD", "ARRIVE_PLANT"]]
        day = jour.day
        if day < 10:
            day = f"0{day}"
        with open(f"{INSTANCE_FOLDER}/{jour.year}{jour.month}{day}.txt", 'w') as file:
            contents = []
            order_count = 0
            dmd_total = 0
            contents.append(f"Depots:   {len(self.df_depots):8}\n")
            orders_of_day = list(dict.fromkeys(day_op['ORDER_ID']))
            all_orders_of_day = list(dict.fromkeys(self.df_orders['ORDER_ID'][self.df_orders['DATE'] == jour]))
            real_orders_of_day = list(set(orders_of_day) & set(all_orders_of_day))
            print(f"{len(real_orders_of_day)} ordres servis")

            # 2 Nombre de clients
            contents.append(f"Clients:   {len(real_orders_of_day):7}\n")

            list_driver = list(dict.fromkeys(day_op['TRUCK_NBR']))
            print(jour)

            contents.append(f"Drivers:  {len(list_driver):8}\n")

            # 4 Liste des depots
            # for i,d in enumerate(list_depot):
            #     contents.append(f"{i} {df_depots.loc[d]['Location ID']} {d} {df_depots_loc.loc[d]['CAPACITY']}\n")  
            for i, dep in enumerate(self.df_depots.index):
                contents.append(
                    f"{i:2} {self.df_depots.loc[dep]['Location ID']} {dep:2} {int(self.df_depots_loc.loc[dep]['CAPACITY']):3} \n")
                self.depot_loc_id[dep] = i
            # 5 Liste des clients de la journée  
            for i, ord in enumerate(real_orders_of_day):
                ordres = self.df_orders[self.df_orders['ORDER_ID'] == ord]
                if len(ordres) == 0:
                    continue

                ordre = ordres.iloc[0]
                string = f"{i:3} {ord} {ordre['CUST_NBR']} {ordre['LOCATION ID']:4} {self.depot_loc_id[ordre['SCHED_LOC']]:2} {ordre['SHIPTIME (min)']:4.0f} {ordres['QUANTITY'].sum():5.1f} {ordres['QUANTITY'].count():2} \n"
                contents.append(string)
                dmd_total = dmd_total + ordres['QUANTITY'].sum()
            # 6 Liste des ordres de la journée  
            for clientID, ord in enumerate(real_orders_of_day):

                ord_seq = day_op[day_op['ORDER_ID'] == ord]
                ordres = self.df_orders[self.df_orders['ORDER_ID'] == ord]
                if len(ordres) == 0:
                    continue
                for j in range(len(ordres)):
                    ordre = ordres.iloc[j]
                    contents.append(
                        f"{order_count:3} {clientID:3} {ordre['QUANTITY']:5.1f} \n")
                    order_count = order_count + 1

            # 7 Ajouter les chauffeurs
            # Lines of drivers: driver id, driver number, driver rank, depot assigned driver max capacity
            driver_cap = set()
            for i, id in enumerate(list_driver):
                dr = self.dict_driver[id]
                df_drv = day_op[day_op['TRUCK_NBR'] == dr.t_nbr]
                _data = df_drv[["BEGIN_LOAD", "ARRIVE_PLANT"]]
                _data.sort_values(["BEGIN_LOAD"], inplace=True)
                # print(dr)
                # _data = test[]
                begin, end = _data.iloc[0, 0], _data.iloc[-1, 1]
                factory_nbr = Counter(df_drv['SHIP_LOC']).most_common(1)[0][0]
                contents.append(
                    f"{i:3} {dr.d_nbr:8} {'_'.join(dr.t_nbr.split(' ')):8} {dr.rank:6} {self.depot_loc_id[factory_nbr]:2} {dr.capacity:2.0f}"
                    f" {int((begin.hour * 60 + begin.minute) / 60) * 60:4}\n")
                driver_cap.add(dr.capacity)
            contents.insert(2, f"Orders:  {order_count:9}  \n")
            contents[-1] = contents[-1].rstrip()
            contents = "".join(contents)
            file.write(contents)
            self.instance_size[f"{jour.year}{jour.month}{day}"] = [len(self.df_depots), len(real_orders_of_day),
                                                                   order_count,
                                                                   len(list_driver), dmd_total, len(driver_cap)]

    def create_all_instances(self) -> None:
        ticket_date = list(dict.fromkeys(self.df_tickets['DATE']))
        # ticket_date = [datetime.date(2020, 10, 10)]
        for jour in ticket_date:
            self.create_instance(jour)

        pd.DataFrame.from_dict(self.instance_size, orient='index').to_csv(f'{INSTANCE_FOLDER}/instance_size.csv',
                                                                          sep=';')


newInstance = Instance()
newInstance.create_all_instances()
