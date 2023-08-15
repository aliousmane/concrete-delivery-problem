#Ensembles:
set T; # tâche
set LINK dimen 2;

#Paramètres:
#param P {i in T, j in T};
param dur {T}>=0;
param LB {T};
param UB {T};

#Variables:
var t {i in T} integer;
var end integer;
#var r{i in T} binary;
#Fonction objectif:
minimize z: end;

#Contraintes:
subject to Greater_than {i in T}: end >= t[i] + dur[i];
subject to LimitLB {i in T}: t[i] >= LB[i]; 
subject to LimitUB {i in T}: t[i] <= UB[i]; 
subject to Relation {(i,j) in LINK}:  t[i] >= t[j] + dur[j];
subject to Limit2 : end>=0;
#subject to bound : end<=28;
#subject to bound2: sum{i in T} r[i] <=0;
#min 10000t + sum_{i in T} ti
#sà  t >= ti+di  pour i = A à L
#    tj >= ti + dj  pour (i,j) dans P
#    ti >= 0     pour i = A à L
#    t >= 0