all:
	cd build && cmake .. && make

run:
	cd build && ./vtk

leak-check:
	cd build && valgrind --leak-check=full ./vtk