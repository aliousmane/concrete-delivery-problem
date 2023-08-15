"""Create instances."""

import pandas as pd
import os
from driver import Driver
import datetime
from collections import Counter
from create_instance import DATA_FOLDER


Instances_with_extern_fleet = set()
Instances_with_intern_fleet = set()

class Instance:
    def __init__(self) -> None:
        self.df_orders = None
        self.df_depots = None
        self.df_tickets = None
        self.df_employee = None
        self.df_depots_loc = None
        self.df_driver = None
        self.list_driver_nbr = []
        self.instance_size = dict()
        self.dict_driver = {}
        self.depot_loc_id = {}
        #self.clean_depot()
        #self.clean_employee()
        #self.clean_tickets()
        #self.get_driver_from_ticket()
        
    
    def init_order(self)->None:
        self.df_orders = pd.read_excel(f"{DATA_FOLDER}/DORDERSTAB_BETON.xlsx", engine='openpyxl', sheet_name="data")
        self.df_orders.sort_values(by='QUANTITY',inplace=True)
        
    def init_depot(self):
        self.df_depots = pd.read_excel(f"{DATA_FOLDER}/adresses_complete.xlsx")
           
    def init_tickets(self):
        self.df_tickets = pd.read_excel(f"{DATA_FOLDER}/DTICKETHISTAB.xlsx")
    
    def init_employee(self):
        self.df_employee = pd.read_excel(f"{DATA_FOLDER}/EMPLOYETAB.xlsx", engine='openpyxl')
        
    def init_depot_loc(self):
         self.df_depots_loc = pd.read_excel(f"{DATA_FOLDER}/LOCATIONTAB.xlsx", engine='openpyxl')
    
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
        print("Clean employee")
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
        
        self.df_tickets['CUST'] = self.df_tickets['ORDER_ID'].apply(lambda x: self.df_orders[self.df_orders['ORDER_ID']==x]['CUST_NBR'].max())
        self.df_tickets['JOB_DESC'] = self.df_tickets['ORDER_ID'].apply(lambda x: self.df_orders[self.df_orders['ORDER_ID']==x]['JOB_DESC'].max())
                              

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
            
    def get_driver_from_day(self, day):
        # Liste des livreurs disponibles dans le fichier Ticket.
        df_driver = self.df_tickets.loc[:, ("DTICKETHIS_TICKET_ID", "SHIP_LOC", "ORDER_ID", "DRIVER_NBR",
                                                 "TRUCK_NBR", "SCHED_LOC", "LOAD", "LOADNR", "QUANTITY","DATE")]
        # Enlever les ordres où le numero du chauffeur/camion est manquant
        df_driver.dropna(axis=0, inplace=True)
        # Enlever TRUCK_NBR = ACHATEXT
        # ! self.df_driver = self.df_driver[self.df_driver['TRUCK_NBR'] != 'ACHATEXT']
        # Enlever livraison dont cap > 12
        df_driver = df_driver[df_driver['DATE']==day]
        df_driver = df_driver[df_driver['QUANTITY'] <= 12]

        df_copy = df_driver[['TRUCK_NBR', 'DRIVER_NBR']].copy()
        df_copy.reset_index(inplace=True)
        df_copy.set_index(['TRUCK_NBR', 'DRIVER_NBR'], inplace=True)
        list_driver_nbr = list(dict.fromkeys(df_copy.index))
        employee_nbr = self.df_employee.index
        dummy_driver = []
        dict_driver = {}
        for (t_nbr, d_nbr) in list_driver_nbr:
            if d_nbr not in employee_nbr:
                dummy_driver.append(d_nbr)
            d = Driver()
            d.t_nbr = t_nbr
            d.d_nbr = int(d_nbr)
            _df = df_driver[df_driver['TRUCK_NBR'] == t_nbr]
            _cap = max(_df['QUANTITY'])
            if _cap <= 8:
                d.capacity = 8
            else:
                d.capacity = 12
            d.factory_nbr = Counter(_df['SHIP_LOC']).most_common(1)[0][0]
            dict_driver[d.t_nbr] = d
            
        return dict_driver
            
    def show(self):
        print(f" {len(self.df_orders)} ordres")
        print(f" {len(self.df_driver)} chauffeurs")
        print(f" {len(self.df_depots)} depots")
