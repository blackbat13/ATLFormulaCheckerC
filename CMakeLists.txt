cmake_minimum_required(VERSION 3.6)
project(ATLFormulaChecker)

set(CMAKE_CXX_STANDARD 14)

set(SOURCE_FILES cppitertools/itertools.hpp main.cpp AtlModel.cpp AtlModel.h DisjointUnion.cpp DisjointUnion.h BridgeModel.cpp BridgeModel.h BridgeModelTestSuite.cpp BridgeModelTestSuite.h)
add_executable(ATLFormulaChecker ${SOURCE_FILES})