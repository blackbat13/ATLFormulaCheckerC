cmake_minimum_required(VERSION 3.6)
project(ATLFormulaChecker)
set(CMAKE_CXX_STANDARD 17)

#SET(GCC_NO_FORK "-D__NOFORK__")
#SET(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} ${GCC_NO_FORK}")

find_package(Threads)
set(SOURCE_FILES cppitertools/itertools.hpp main.cpp Logics/AtlModel.cpp Logics/AtlModel.h Tools/DisjointUnion.cpp Tools/DisjointUnion.h Models/BridgeModel.cpp Models/BridgeModel.h Models/SeleneModel.cpp Models/SeleneModel.h Models/ModelGenerator.cpp Models/ModelGenerator.h Models/State.cpp Models/State.h Models/SimpleModel.cpp Models/SimpleModel.h Models/Transition.cpp Models/Transition.h Models/ActionTransition.cpp Models/ActionTransition.h Models/ParallelModel.hpp Models/asynchronous/LocalTransition.cpp Models/asynchronous/LocalTransition.h Models/asynchronous/SharedTransition.cpp Models/asynchronous/SharedTransition.h Models/asynchronous/LocalModel.cpp Models/asynchronous/LocalModel.h Models/asynchronous/GlobalState.cpp Models/asynchronous/GlobalState.h Models/asynchronous/parser/GlobalModelParser.cpp Models/asynchronous/parser/GlobalModelParser.h Models/asynchronous/parser/LocalModelParser.cpp Models/asynchronous/parser/LocalModelParser.h Models/asynchronous/parser/LocalTransitionParser.cpp Models/asynchronous/parser/LocalTransitionParser.h Models/asynchronous/GlobalModel.cpp Models/asynchronous/GlobalModel.h Tools/StringTools.cpp Tools/StringTools.h Tools/SetTools.cpp Tools/SetTools.h Models/ParallelModel.cpp)
add_executable(ATLFormulaChecker ${SOURCE_FILES})
target_link_libraries( ATLFormulaChecker Threads::Threads)