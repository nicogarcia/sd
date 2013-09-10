all: project_tester exercise_1 exercise_2_image exercise_2 exercise_4
	
project_tester: project_tester/project_tester.c
	gcc -g -o build/$@ $^
	
exercise_1: exercise_1/exercise_1.c
	gcc -g -o build/$@ $^
	
exercise_2_image: exercise_2/exercise_2_image.c
	gcc -g -lrt -o build/$@ $^
	
exercise_2: exercise_2/exercise_2.c
	gcc -g -lrt -o build/$@ $^

exercise_4: exercise_4/server.c exercise_4/client.c 
	gcc -g -o build/$@ $^
	
clean:
	rm -rf build/*