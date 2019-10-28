cmake_minimum_required(VERSION 3.6)
project(ATLFormulaChecker)

set(CMAKE_CXX_STANDARD 14)

set(SOURCE_FILES cppitertools/itertools.hpp main.cpp Logics/AtlModel.cpp Logics/AtlModel.h Tools/DisjointUnion.cpp Tools/DisjointUnion.h Models/BridgeModel.cpp Models/BridgeModel.h BridgeModelTestSuite.cpp BridgeModelTestSuite.h Models/SeleneModel.cpp Models/SeleneModel.h Models/ModelGenerator.cpp Models/ModelGenerator.h Models/State.cpp Models/State.h Models/SimpleModel.cpp Models/SimpleModel.h Models/Transition.cpp Models/Transition.h Models/ActionTransition.cpp Models/ActionTransition.h)
add_executable(ATLFormulaChecker ${SOURCE_FILES})