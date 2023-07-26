cmake_minimum_required(VERSION 3.23)
project(codeCq)

ADD_CUSTOM_TARGET(debug
        COMMAND ${CMAKE_COMMAND} -DCMAKE_BUILD_TYPE=Debug ${CMAKE_SOURCE_DIR}
        COMMAND ${CMAKE_COMMAND} --build ${CMAKE_BINARY_DIR} --target all
        COMMENT "Creating the executable in the debug mode.")

ADD_CUSTOM_TARGET(release
        COMMAND ${CMAKE_COMMAND} -DCMAKE_BUILD_TYPE=Release ${CMAKE_SOURCE_DIR}
        COMMAND ${CMAKE_COMMAND} --build ${CMAKE_BINARY_DIR} --target all
        COMMENT "Creating the executable in the release mode in folder ${CMAKE_SOURCE_DIR}.")

set(CMAKE_CXX_STANDARD 23)
list(APPEND CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR})
add_executable(codeCq heur/main_heur.cpp Data.cpp Data.h Parameters.cpp Parameters.h Prompt.cpp
        Prompt.h Prompt.cpp Parameters.cpp Parameters.cpp Parameters.cpp Node.h Driver.h
        Solution.cpp Solution.h Cost.cpp Cost.h TimeSlot.cpp TimeSlot.h UpdateCost.cpp
        heur/Solver.cpp heur/Solver.h InsertOperator.h heur/CustInsertion.cpp
        heur/CustInsertion.h InsRmvMethod.h ListMove.h Move.h heur/InsRmvMethodFast.cpp
        heur/InsRmvMethodFast.h heur/InsRmvBuilder.h TempsPriorite.cpp TimeSlotPriorite.cpp
        SortRelated.cpp heur/InsRmvBuilder1.h heur/ListMoveVrp.cpp heur/CustInsertionOperator.h
        heur/InsRmvBuilder2.h model/gurobi/gurobiSolver.cpp model/gurobi/gurobiSolver.h
        model/SolverExact.cpp model/SolverExact.h model/cplex/cplexSolver.cpp
        model/cplex/cplexSolver.h model/Graph.cpp model/Graph.h heur/InsRmvBuilder3.h
        heur/RechercheLocale.cpp GRASP.cpp heur/CDPSolver.cpp heur/CDPSolver.h
        heur/PriorityQueueInsertion.h heur/PriorityQueueInsertion.cpp
        heur/PriorityQueueOperator.h heur/DriverInsertion.h heur/DriverInsertion.cpp
        heur/DriverInsertionOperator.h Combinatorial.cpp Combinatorial.h
        heur/CustInsertionBacktrack.cpp heur/CustInsertionBacktrack.h
        heur/AllInsertionOperator.h heur/CustInsertion2.cpp heur/CustInsertion2.h
        model/Arc.h model/cplex/createVariables.cpp model/cplex/createObjective.cpp model/cplex/createConstraints.cpp model/cplex/solveModel.cpp)

if(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
    set(CPLEX_STUDIO_DIR "/Applications/CPLEX_Studio_Community2211/")
    set(SYSTEM  arm64_osx)
    set(GUROBI_DIR "/Library/gurobi952/macos_universal2")
    set(LEMON_DIR  /Users/aliwa/lemon-1.3.1/)
    set(GUROBI_LIB2  libgurobi95.dylib)

elseif(${CMAKE_SYSTEM_NAME} MATCHES "Linux")

    set(CPLEX_STUDIO_DIR /home/alious/CPLEX_Studio1210/)
    set(SYSTEM  x86-64_linux)
    set(GUROBI_DIR "/home/alious/gurobi952/linux64")
    set(GUROBI_LIB2  libgurobi95.so)
    set(LEMON_DIR  /home/alious/lemon-1.3.1/)

endif()

set(CONCERT_INCLUDE_DIRS ${CPLEX_STUDIO_DIR}/concert/include)
set(CONCERT_LIB_DIRS ${CPLEX_STUDIO_DIR}/concert/lib/${SYSTEM}/static_pic)
set(CONCERT_LIBRARIES concert)
set(CPLEX_INCLUDE_DIRS ${CPLEX_STUDIO_DIR}/cplex/include)
set(CPLEX_LIB_DIRS ${CPLEX_STUDIO_DIR}/cplex/lib/${SYSTEM}/static_pic)
set(CPLEX_LIBRARIES cplex)


set(GUROBI_INCLUDE_DIRS ${GUROBI_DIR}/include)
set(GUROBI_LIB_DIRS ${GUROBI_DIR}/lib)

set(LEMON_INCLUDE_DIRS ${LEMON_DIR}/include)
set(LEMON_LIB_DIRS ${LEMON_DIR}/lib)

include_directories(${CONCERT_INCLUDE_DIRS} ${CPLEX_INCLUDE_DIRS}
        ${GUROBI_INCLUDE_DIRS} ${LEMON_INCLUDE_DIRS} )
link_directories(${CONCERT_LIB_DIRS} ${CPLEX_LIB_DIRS}
        ${GUROBI_LIB_DIRS} ${LEMON_LIB_DIRS})
#
target_link_libraries(codeCq
        ${CPLEX_LIB_DIRS}/libilocplex.a ${CPLEX_LIB_DIRS}/libcplex.a
        ${CONCERT_LIB_DIRS}/libconcert.a
        ${GUROBI_LIB_DIRS}/libgurobi_c++.a
        ${GUROBI_LIB_DIRS}/${GUROBI_LIB2}
        ${LEMON_LIB_DIRS}/libemon.a
        )
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -DIL_STD")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DGRB_USE_STD_VECTOR -DGRB_USE_UNORDERED_MAP")

add_compile_options(-Wdeprecated-declaration)