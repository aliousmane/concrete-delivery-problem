"""Class driver."""
class Driver:
    def __init__(self):
        self.id = -1
        self.rank = -1
        self.capacity = 0
        self.driver_type = -1
        self.truck_capacity = 0
        self.factory_nbr = -1
        self.depot_nbr = -1
        self.location_id = -1
        self.ltw = 0
        self.utw = 0
        self.nbr = -1
        self.description = ""

    def __str__(self):
        return f"Driver Nbr {self.nbr} Rank {self.rank} {self.description} depot n {self.depot_nbr} fac {self.factory_nbr} de capacité {self.capacity}\n"
