## Requirements

- cmake
- g++
- Linux

## How to use

### Compile
```
cd ATLFormulaCheckerC
cmake .
cmake --build ./ --target ATLFormulaChecker
```
### Run
```
./ATLFormulaChecker [dump_file]
```
#### Example
```
./ATLFormulaChecker specs/dumps/simple_voting_synchronous_2v_2c_dump.txt
```