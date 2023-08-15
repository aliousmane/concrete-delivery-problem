import pandas as pd
import numpy as np
import plotly.express as px

# Transform minutes to format"hh:mm:ss:"
def time_to_date(date,t):
    date=str(date)
    if len(date.split('-'))==1:
        date="2022-05-16"
    else:
        date=date[:4]+"-"+date[4:6]+"-"+date[6:]
    min=int(t%60)
    hour=int(t//60)
    if min<10:
        min=f"0{min}"
    new_date=f"{date} {hour}:{min}:00"
    if hour>23: 
        print(new_date,' ',t)
    return new_date


# Creer des diagrammes de Gantt
# Par ordre: afficher les heures d'arrivée et de départ des différents noeuds
# J'ai besoin de la date, des heures d'arrivée et de départ.
def showGanttDel(df):
    df['task'] = df.apply(lambda x: f" del {x.Del}_{x.Order}_{x.Cust}",axis=1)
    fig = px.timeline(df, x_start="startUnload", x_end="endUnload", y="task",title="All deliveries",color="Cust")
    # fig.update_yaxes(autorange="reversed") # otherwise tasks are listed from the bottom up
    fig.show()

def showGanttDock(df):
    df['task'] = df.apply(lambda x: f" dock {x.Del}_{x.Order}_{x.Cust}",axis=1)
    fig = px.timeline(df, x_start="startLoad", x_end="endLoad", y="task",title="All Loading",color="Cust")
    # fig.update_yaxes(autorange="reversed") # otherwise tasks are listed from the bottom up
    fig.show()

# Creer des diagrammes de Gantt
# Par ordre: afficher les heures d'arrivée et de départ des différents noeuds
# J'ai besoin de la date, des heures d'arrivée et de départ.
def showGanttOrder(df):
    orderList=np.unique(df['ordreId'])
    df['task'] = df[['delId','ordreId']].apply(lambda x: f"del {x.delId}_{x.ordreId}",axis=1)
    
    for ord in orderList:
        df_ordre=df.set_index('ordreId').xs(ord)
        if type(df_ordre)==pd.core.series.Series:
            df_ordre = df_ordre.to_frame().T
        df_ordre.sort_values(by=['ArrivalTimeDel'],inplace=True)
        fig = px.timeline(df_ordre, x_start="start", x_end="end", y="task",title=f'order {ord}',color="driverId")
        fig.update_yaxes(autorange="reversed") # otherwise tasks are listed from the bottom up
        fig.show()

# Creer des diagrammes de Gantt
# Par ordre: afficher les heures d'arrivée et de départ des différents noeuds
# J'ai besoin de la date, des heures d'arrivée et de départ.
def showGanttDriver(df):
    driverList=np.unique(df['Driver'])
    df['task'] = df.apply(lambda x: f"del {x.Del}_{x.Order}_{x.Cust}",axis=1)
    
    for ord in driverList:
        df_ordre=df.set_index('Driver').xs(ord)
        if type(df_ordre)==pd.core.series.Series:
            df_ordre = df_ordre.to_frame().T
        df_ordre.sort_values(by=['startLoad'],inplace=True)
        fig = px.timeline(df_ordre, x_start="startLoad", x_end="endUnload", y="task",title=f"Driver {ord}",color="Order")
        fig.update_yaxes(autorange="reversed") # otherwise tasks are listed from the bottom up
        fig.show()

# Creer des diagrammes de Gantt
# Par ordre: afficher les heures d'arrivée et de départ des différents noeuds
# J'ai besoin de la date, des heures d'arrivée et de départ.
def showGanttDepot(df):
    depotList=np.unique(df['Depot'])
    df['task'] = df.apply(lambda x: f"del {x.Del}_{x.Order}_{x.Cust}",axis=1)
    
    for ord in depotList:
        df_ordre=df.set_index('Depot').xs(ord)
        if type(df_ordre)==pd.core.series.Series:
            df_ordre = df_ordre.to_frame().T
        df_ordre.sort_values(by=['startLoad'],inplace=True)
        fig = px.timeline(df_ordre, x_start="startLoad", x_end="endLoad", y="task",title=f'Depot {ord}',color="Driver")
        fig.update_yaxes(autorange="reversed") # otherwise tasks are listed from the bottom up
        fig.show()