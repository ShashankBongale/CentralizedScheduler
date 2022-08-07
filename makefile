cslib:CentralizedScheduler.o Master.o Worker.o
	g++ CentralizedScheduler.o Master.o Worker.o -o cslib -ljsoncpp -lzmq -lpthread
CentralizedScheduler.o:CentralizedScheduler.h CentralizedScheduler.cpp
	g++ -c CentralizedScheduler.h CentralizedScheduler.cpp
Master.o:Master.h Master.cpp
	g++ -c Master.h Master.cpp
Worker.o:Worker.h Worker.cpp
	g++ -c Worker.h Worker.cpp
