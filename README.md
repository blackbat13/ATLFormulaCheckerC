### Compile
```
cd ATLFormulaCheckerC
cmake .
cmake --build ./ --target ATLFormulaChecker
```
### Run
```
./ATLFormulaChecker [dump_file] [num_of_threads] [-p]
-p for perfect information verification
```
#### Example
```
./ATLFormulaChecker dumps/selene_select_vote_1v_1cv_8c_rFalse_dump.txt 10
./ATLFormulaChecker dumps/selene_select_vote_1v_1cv_8c_rFalse_dump.txt 10 -p
```