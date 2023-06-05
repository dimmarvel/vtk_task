all:
	g++ -g -Wall main.cpp tlv_parser.cpp tlv.cpp vtk_parser.cpp vtk.cpp app.cpp -o out
