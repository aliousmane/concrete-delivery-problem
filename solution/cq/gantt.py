import pandas as pd
import numpy as np
import plotly.express as px
from distinctipy import distinctipy

# Transform minutes to format"hh:mm:ss:"
def time_to_date(date,t):
    date=str(date)
    # print(date)
    date=date[:4]+"-"+date[4:6]+"-"+date[6:]
    min=int(t%60)
    hour=int(t//60)
    if min<10:
        min=f"0{min}"
    new_date=f"{date} {hour}:{min}:00"
    if hour>23: 
        print(new_date,' ',t)
    return new_date


def show(df):
    df['task'] = df.apply(lambda x: f" del {x.Del}",axis=1)
    color = [f"rgb{distinctipy.get_rgb256(x)}" for x in distinctipy.get_colors(29)]
    fig = px.timeline(df, x_start="start", x_end="end", y="Driver",title="All drivers",
                      color="Driver",template='ggplot2',
                      color_continuous_scale= color)
    # fig.update_yaxes(autorange="reversed") # otherwise tasks are listed from the bottom up
    # fig.update_xaxes(fixedrange=True)
    # fig.update_xaxes(
    # tickformatstops=[
    #     dict(dtickrange=[None, 86400000], value=" %h %m %s \n")
    # ]
    # )
    
    # fig.update_xaxes(dtick=86400000)
    fig.update_layout(
             xaxis=dict(
        # type="date",           # Set x-axis type to date-based
        tickmode="auto",       # Automatically choose tick positions
        # tickformat="%b %d",    # Format of tick labels (Jan 01)
        nticks=15,  
    ),
    )
    fig.show()

# Creer des diagrammes de Gantt
# Par ordre: afficher les heures d'arrivée et de départ des différents noeuds
# J'ai besoin de la date, des heures d'arrivée et de départ.
def showGanttDel(df):
    df['task'] = df.apply(lambda x: f" del {x.Order}_{x.Cust}",axis=1)
    # df['task'] = df.apply(lambda x: f" del {x.Del}",axis=1)
    fig = px.timeline(df, x_start="start", x_end="end", y="Del",title="All deliveries",color="task")
    fig.update_yaxes(autorange="reversed") # otherwise tasks are listed from the bottom up
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
    Prism = [
   'rgb(0, 255, 0)', 'rgb(255, 0, 255)', 'rgb(0, 128, 255)',
    'rgb(255, 128, 0)', 'rgb(128, 191, 128)']# 'rgb(74, 17, 157)',
    # 'rgb(209, 2, 37)', 'rgb(74, 17, 157)', 'rgb(176, 114, 254)',
    # 'rgb(3, 255, 226)', 'rgb(52, 125, 6)', 'rgb(0, 128, 128)',
    # 'rgb(250, 81, 134)', 'rgb(255, 128, 0)'
    # ]
    depotList=np.unique(df['Depot'])
    df['Task'] = df.apply(lambda x: f"Driver {x.Driver}",axis=1)
    # color = [f"rgb{distinctipy.get_rgb256(x)}" for x in distinctipy.get_colors(14)]
    # print(color)
    for dep in depotList:
        df_ordre=df.set_index('Depot').xs(dep)
        # df_ordre=df[df['Depot']==dep]
        if type(df_ordre)==pd.core.series.Series:
            df_ordre = df_ordre.to_frame().T
        df_ordre.sort_values(by=['start'],inplace=True)
        fig = px.timeline(df_ordre, x_start="start", x_end="end", y="Task"
        ,text='Order',width=1000, height=600,
        template='ggplot2',
        # color_discrete_sequence=["red", "green", "blue", "goldenrod", "magenta"],
        color="Order",      
        color_continuous_scale= Prism
        ,opacity=1
        )
        # ,color_continuous_scale=["cyan", "green", "blue", "goldenrod", "magenta","magenta"]
        # )
        fig.update_yaxes(autorange="reversed",
                        #  showgrid=True,
                        #  showticklabels=False,
                        #  ticks="", 
                         ) # otherwise tasks are listed from the bottom up
        
        # fig.update_layout(
            # yaxis=dict(visible=False),  # Hide the y-axis
            # xaxis=dict(type="date"),     # Use a date-based x-axis
            # title="Timeline",
            # font=dict(
            # family="Courier New, monospace",
            # size=5,  # Set the font size here
            # color="RebeccaPurple"
            # )
            # )
        # fig.update_traces(textposition='inside')
        fig.update_layout(uniformtext_minsize=6)#, uniformtext_mode='hide')
        fig.update_coloraxes(showscale=False)
        # fig.update_traces(showlegend=False)
        fig.update_xaxes(
            tickformat="%H:%M",
            nticks = 25,
            tickformatstops=[
            dict(dtickrange=[3600000, 86400000], value="%H:%M")]  # range is 1 hour to 24 hours
        )
        # for template in ["plotly", "plotly_white", "plotly_dark", "ggplot2", "seaborn", "simple_white", "none"]:
        #     fig.update_layout(template=template, title="Mt Bruno Elevation: '%s' theme" % template)
        #     fig.show()
        
        fig.write_image(f"depot_{dep}.pdf")
        fig.write_image(f"depot_{dep}.png")
        fig.show()