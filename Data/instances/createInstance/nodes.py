

class Node:
    def __init__(self, id=-1, type=""):
        self.id = id
        self.nbr = -1
        self.coord = (0, 0)
        self.adresse = ""
        self.code_postal = ""
        self.demand = 0
        self.ltw = 0
        self.utw = 0
        self.location_type = type
        self.location_id = -1
        self.name = ""
        self.depot_loc = 0

    def __str__(self) -> str:
        return f"id: {self.id} {self.location_type} locId {self.location_id} x:{self.coord[0]:.2f},y:{self.coord[1]:.2f} depot {self.depot_loc}\n"
