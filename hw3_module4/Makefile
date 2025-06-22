all:	malloc test testcpp test_improvements test_free_list

malloc:
	gcc -Wall -g -shared -fpic -o libmalloc.so malloc.c
test:
	gcc -g test.c -o test
testcpp:
	g++ -g test.cpp -o testcpp
test_improvements:
	gcc -g test_improvements.c -o test_improvements
test_free_list:
	gcc -g test_free_list.c -o test_free_list
run:	malloc test
	LD_PRELOAD="./libmalloc.so" ./test
runcpp:	malloc testcpp
	LD_PRELOAD="./libmalloc.so" ./testcpp
run_improvements: malloc test_improvements
	LD_PRELOAD="./libmalloc.so" ./test_improvements
run_free_list: malloc test_free_list
	LD_PRELOAD="./libmalloc.so" ./test_free_list
debug:
	gdb -iex "set exec-wrapper env LD_PRELOAD=./libmalloc.so" ./test
clean:
	rm -f test testcpp test_improvements test_free_list *.so *.o 