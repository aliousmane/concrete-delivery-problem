
# Dataset Description

The name of an instance has the format C_n_o_k_p, where n is the number of customers, o is the total number of orders, k is the number of available drivers, and p is the number of plants.

The instance values are ordered as follows:

## Drivers: k
- ID: Driver ID
- PLANT_ID: ID of the plant the driver is assigned to
- CAPACITY: Capacity of the driver
- SHIFT_START: Start time of the driver's shift

## Clients: n
- ID: Client ID
- MATRIX_ID: ID of the client in the time matrix (companion file Time_Matrix.csv)
- DUE_DATE: Due date for the client
- DEMAND: Total demand from the client
- #ORDER: Number of orders from the client

## Orders: o
- ID: Order ID
- CLIENT_ID: ID of the client associated with the order.
- DEMAND: Demand for the order
- PLANT_ID: ID of the plant from which the order will be scheduled

## Plants: p
- ID: Plant ID
- MATRIX_ID: ID of the client in the time matrix (companion file Time_Matrix.csv)
- CAPACITY: Capacity of the plant. 