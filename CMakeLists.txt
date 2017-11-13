cmake_minimum_required(VERSION 3.6)
project(ATLFormulaChecker)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_FLAGS "-std=c++0x -Wall")

set(SOURCE_FILES cppitertools/itertools.hpp main.cpp AtlModel.cpp AtlModel.h DisjointUnion.cpp DisjointUnion.h BridgeModel.cpp BridgeModel.h BridgeModelTestSuite.cpp BridgeModelTestSuite.h SeleneModel.cpp SeleneModel.h BridgeModelLowerAbstr.cpp BridgeModelLowerAbstr.h BridgeModelUpperAbstr.cpp BridgeModelUpperAbstr.h BridgeModelAbstrTestSuite.cpp BridgeModelAbstrTestSuite.h)
add_executable(ATLFormulaChecker ${SOURCE_FILES})