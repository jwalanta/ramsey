build: gtools.h naurng.h nautaux.h naututil.h solver.h naugroup.h nausparse.h nautinv.h nauty.h utils.h ramsey.cpp solver.cpp utils.cpp
	g++ ramsey.cpp solver.cpp utils.cpp nauty64.a libgmp.a -o ramsey -O3 -I . 
32: gtools.h naurng.h nautaux.h naututil.h solver.h naugroup.h nausparse.h nautinv.h nauty.h utils.h ramsey.cpp solver.cpp utils.cpp
	g++ ramsey.cpp solver.cpp utils.cpp nauty32.a -o ramsey32 -O3
clean:
	rm ramsey ramsey64

