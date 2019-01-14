compiler:
	
	mkdir -p generated
	mkdir -p build
	flex -o generated/scanner.cpp src/microLexer.ll
	bison -d -o generated/parser.cpp src/microParser.yy
	g++ -std=c++0x -c -o build/symbols.o src/symbols.cpp
	g++ -std=c++0x -c -o build/parser.o generated/parser.cpp
	g++ -std=c++0x -c -o build/scanner.o generated/scanner.cpp
	g++ -std=c++0x -c -o build/main.o src/compiler.cpp
	g++ -std=c++0x -o Micro build/scanner.o build/parser.o build/main.o build/symbols.o


clean:
	@rm -rf build/ generated/
	@rm -f *.out
	@rm -f Micro

team : 
	@echo "Team: Hegewald_Park"
	@echo ""
	@echo "Andrew Hegewald"
	@echo "ahegewald"
	@echo ""
	@echo "Chul Woo Park"
	@echo "ChulWPark"

help:
	@echo "This makefile helps you quickly run the compiler."
	@echo "make clean: removes all temporary files"
	@echo "make team: displays team information"
	@echo "make compiler: compiles the file passed as <arg>"

test1:
	./runme input/fibonacci.micro fibonacci.out
	diff output/fibonacci.out fibonacci.out
	@rm fibonacci.out
	./runme input/loop.micro loop.out
	diff output/loop.out loop.out
	@rm loop.out
	./runme input/nested.micro nested.out
	diff output/nested.out nested.out
	@rm nested.out
	./runme input/sqrt.micro sqrt.out
	diff output/sqrt.out sqrt.out
	@rm sqrt.out
	./runme input/test1.micro test1.out
	diff output/test1.out test1.out
	@rm test1.out
	./runme input/test2.micro test2.out
	diff output/test2.out test2.out
	@rm test2.out

test2:
	./runme input_step2/test1.micro output.out
	diff output.out output_step2/test1.out
	./runme input_step2/test2.micro output.out
	diff output.out output_step2/test2.out
	./runme input_step2/test3.micro output.out
	diff output.out output_step2/test3.out
	./runme input_step2/test4.micro output.out
	diff output.out output_step2/test4.out
	./runme input_step2/test5.micro output.out
	diff output.out output_step2/test5.out
	./runme input_step2/test6.micro output.out
	diff output.out output_step2/test6.out
	./runme input_step2/test7.micro output.out
	diff output.out output_step2/test7.out
	./runme input_step2/test8.micro output.out
	diff output.out output_step2/test8.out
	./runme input_step2/test9.micro output.out
	diff output.out output_step2/test9.out
	./runme input_step2/test10.micro output.out
	diff output.out output_step2/test10.out
	./runme input_step2/test11.micro output.out
	diff output.out output_step2/test11.out
	./runme input_step2/test12.micro output.out
	diff output.out output_step2/test12.out
	./runme input_step2/test13.micro output.out
	diff output.out output_step2/test13.out
	./runme input_step2/test14.micro output.out
	diff output.out output_step2/test14.out
	./runme input_step2/test15.micro output.out
	diff output.out output_step2/test15.out
	./runme input_step2/test16.micro output.out
	diff output.out output_step2/test16.out
	./runme input_step2/test17.micro output.out
	diff output.out output_step2/test17.out
	./runme input_step2/test18.micro output.out
	diff output.out output_step2/test18.out
	./runme input_step2/test19.micro output.out
	diff output.out output_step2/test19.out
	./runme input_step2/test20.micro output.out
	diff output.out output_step2/test20.out
	./runme input_step2/test21.micro output.out
	diff output.out output_step2/test21.out
	@rm -f output.out

test3:
	./runme input_step3/test1.micro output.out
	diff output.out output_step3/test1.out
	./runme input_step3/test5.micro output.out
	diff output.out output_step3/test5.out
	./runme input_step3/test6.micro output.out
	diff output.out output_step3/test6.out
	./runme input_step3/test7.micro output.out
	diff output.out output_step3/test7.out
	./runme input_step3/test8.micro output.out
	diff output.out output_step3/test8.out
	./runme input_step3/test11.micro output.out
	diff output.out output_step3/test11.out
	./runme input_step3/test13.micro output.out
	diff output.out output_step3/test13.out
	./runme input_step3/test14.micro output.out
	diff output.out output_step3/test14.out
	./runme input_step3/test16.micro output.out
	diff output.out output_step3/test16.out
	./runme input_step3/test18.micro output.out
	diff output.out output_step3/test18.out
	./runme input_step3/test19.micro output.out
	diff output.out output_step3/test19.out
	./runme input_step3/test20.micro output.out
	diff output.out output_step3/test20.out
	./runme input_step3/test21.micro output.out
	diff output.out output_step3/test21.out
	./runme input_step3/test22.micro output.out
	diff output.out output_step3/test22.out
	@rm -f output.out

test_tiny:
	./tiny output_step4/test_complex.out | head -n 5 > test_out.out
	./runme input_step4/test_complex.micro output.out 
	./tiny output.out | head -n 5 > my_out.out
	diff my_out.out test_out.out
	./tiny output_step4/test_combination.out | head -n 1 > test_out.out
	./runme input_step4/test_combination.micro output.out 
	./tiny output.out | head -n 1 > my_out.out
	diff my_out.out test_out.out
	./tiny output_step4/test_expr.out | head -n 21 > test_out.out
	./runme input_step4/test_expr.micro output.out 
	./tiny output.out | head -n 21 > my_out.out
	diff my_out.out test_out.out
	./tiny output_step4/test_mult.out < input_step4/test_mult.input | head -n 5 > test_out.out
	./runme input_step4/test_mult.micro output.out 
	./tiny output.out < input_step4/test_mult.input | head -n 5 > my_out.out
	diff my_out.out test_out.out
	./tiny output_step5/step5_test1.out < input_step5/step5_test1.input | head -n 1 > test_out.out
	./runme input_step5/step5_test1.micro output.out 
	./tiny output.out < input_step5/step5_test1.input | head -n 1 > my_out.out
	diff my_out.out test_out.out
	./tiny output_step5/step5_test2.out | head -n 1 > test_out.out
	./runme input_step5/step5_test2.micro output.out 
	./tiny output.out | head -n 1 > my_out.out
	diff my_out.out test_out.out
	./tiny output_step5/step5_test3.out < input_step5/step5_test3.input | head -n 2 > test_out.out
	./runme input_step5/step5_test3.micro output.out 
	./tiny output.out < input_step5/step5_test3.input | head -n 2 > my_out.out
	diff my_out.out test_out.out
	./tiny output_step5/step5_test4.out < input_step5/step5_test4.input | head -n 2 > test_out.out
	./runme input_step5/step5_test4.micro output.out 
	./tiny output.out < input_step5/step5_test4.input | head -n 2 > my_out.out
	diff my_out.out test_out.out
	./tiny output_step5/step5_test5.out | head -n 1 > test_out.out
	./runme input_step5/step5_test5.micro output.out 
	./tiny output.out | head -n 1 > my_out.out
	diff my_out.out test_out.out
	./tiny output_step6/fma.out < input_step6/fma.input | head -n 1 > test_out.out
	./runme input_step6/fma.micro output.out 
	./tiny output.out < input_step6/fma.input | head -n 1 > my_out.out
	diff my_out.out test_out.out
	./tiny output_step6/fibonacci2.out < input_step6/fibonacci2.input | head -n 14 > test_out.out
	./runme input_step6/fibonacci2.micro output.out 
	./tiny output.out < input_step6/fibonacci2.input | head -n 14 > my_out.out
	diff my_out.out test_out.out
	./tiny output_step6/factorial2.out < input_step6/factorial2.input | head -n 1 > test_out.out
	./runme input_step6/factorial2.micro output.out 
	./tiny output.out < input_step6/factorial2.input | head -n 1 > my_out.out
	diff my_out.out test_out.out
	@rm -f output.out my_out.out test_out.out

test_tiny4:
	./tiny output_step4/test_complex.out | head -n 5 > test_out.out
	./runme input_step4/test_complex.micro output.out 
	./tiny4 output.out | head -n 5 > my_out.out
	diff my_out.out test_out.out
	./tiny output_step4/test_combination.out | head -n 1 > test_out.out
	./runme input_step4/test_combination.micro output.out 
	./tiny4 output.out | head -n 1 > my_out.out
	diff my_out.out test_out.out
	./tiny output_step4/test_expr.out | head -n 21 > test_out.out
	./runme input_step4/test_expr.micro output.out 
	./tiny4 output.out | head -n 21 > my_out.out
	diff my_out.out test_out.out
	./tiny output_step4/test_mult.out < input_step4/test_mult.input | head -n 5 > test_out.out
	./runme input_step4/test_mult.micro output.out 
	./tiny4 output.out < input_step4/test_mult.input | head -n 5 > my_out.out
	diff my_out.out test_out.out
	./tiny output_step5/step5_test1.out < input_step5/step5_test1.input | head -n 1 > test_out.out
	./runme input_step5/step5_test1.micro output.out 
	./tiny4 output.out < input_step5/step5_test1.input | head -n 1 > my_out.out
	diff my_out.out test_out.out
	./tiny output_step5/step5_test2.out | head -n 1 > test_out.out
	./runme input_step5/step5_test2.micro output.out 
	./tiny4 output.out | head -n 1 > my_out.out
	diff my_out.out test_out.out
	./tiny output_step5/step5_test3.out < input_step5/step5_test3.input | head -n 2 > test_out.out
	./runme input_step5/step5_test3.micro output.out 
	./tiny4 output.out < input_step5/step5_test3.input | head -n 2 > my_out.out
	diff my_out.out test_out.out
	./tiny output_step5/step5_test4.out < input_step5/step5_test4.input | head -n 2 > test_out.out
	./runme input_step5/step5_test4.micro output.out 
	./tiny4 output.out < input_step5/step5_test4.input | head -n 2 > my_out.out
	diff my_out.out test_out.out
	./tiny output_step5/step5_test5.out | head -n 1 > test_out.out
	./runme input_step5/step5_test5.micro output.out 
	./tiny4 output.out | head -n 1 > my_out.out
	diff my_out.out test_out.out
	./tiny output_step6/fma.out < input_step6/fma.input | head -n 1 > test_out.out
	./runme input_step6/fma.micro output.out 
	./tiny4 output.out < input_step6/fma.input | head -n 1 > my_out.out
	diff my_out.out test_out.out
	./tiny output_step6/fibonacci2.out < input_step6/fibonacci2.input | head -n 14 > test_out.out
	./runme input_step6/fibonacci2.micro output.out 
	./tiny4 output.out < input_step6/fibonacci2.input | head -n 14 > my_out.out
	diff my_out.out test_out.out
	./tiny output_step6/factorial2.out < input_step6/factorial2.input | head -n 1 > test_out.out
	./runme input_step6/factorial2.micro output.out 
	./tiny4 output.out < input_step6/factorial2.input | head -n 1 > my_out.out
	diff my_out.out test_out.out
	@rm -f output.out my_out.out test_out.out

test7:
	./runme input_step7/step7_test17.micro new.out
	./tiny old.out < input_step7/step7_test17.input
	./tiny4 new.out < input_step7/step7_test17.input
	#./runme input_step8/func_call_test.micro new.out
	#./tiny old.out
	#./tiny4 new.out
	#./runme input_step8/local_string.micro new.out
	#./tiny old.out
	#./tiny4 new.out
	#./runme input_step8/logic_if_test.txt new.out
	#./tiny old.out
	#./tiny4 new.out
	#./runme input_step8/str_test.micro new.out
	#./tiny old.out
	#./tiny4 new.out

test13:
	./runme input_step7/step7_test13.micro output.out > tiny_out.out
	cat tiny_out.out
	./tiny tiny_out.out < input_step7/step7_test13.input > output_log.out
	cat output_log.out
	./tiny4 output.out < input_step7/step7_test13.input > output_log.out
	cat output_log.out
	
test14:
	./runme input_step7/step7_test14.micro output.out > tiny_out.out
	cat tiny_out.out
	./tiny tiny_out.out < input_step7/step7_test14.input > output_log.out
	cat output_log.out
	./tiny4 output.out < input_step7/step7_test14.input > output_log.out
	cat output_log.out

test16:
	./runme input_step7/step7_test16.micro output.out > tiny_out.out
	cat tiny_out.out
	./tiny tiny_out.out < input_step7/step7_test16.input > output_log.out
	cat output_log.out
	./tiny4 output.out < input_step7/step7_test16.input > output_log.out
	cat output_log.out

test17:
	./runme input_step7/step7_test17.micro output.out > tiny_out.out
	cat tiny_out.out
	./tiny tiny_out.out < input_step7/step7_test17.input > output_log.out
	cat output_log.out
	./tiny4 output.out < input_step7/step7_test17.input > output_log.out
	cat output_log.out
