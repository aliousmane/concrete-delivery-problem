

class Node:
    def __init__(self):
        self.id = -1
        self.nbr = -1
        self.coord = (0, 0)
        self.adresse = ""
        self.code_postal = ""
        self.demand = 0
        self.ltw = 0
        self.utw = 0
        self.type = -1


class Driver:
    def __init__(self):
        self.id = -1
        self.rank = -1
        self.capacity = 0
        self.driver_type = -1
        self.truck_capacity = 0
        self.factory_nbr = -1
        self.location_id = -1
        self.ltw = 0
        self.utw = 0

    def __str__(self):
        return f"Driver {self.id} {self.rank}"
