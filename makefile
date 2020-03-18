#Alice Giandjian, CS302 - 1004, Assignment #11
#Note to self, never use Ed's makefiles again too complicated
CC=g++ -g -Wall -pedantic -std=c++11
DEPS=linkedQueue.h ganttUtils.h
SOURCES=projectInfo.cpp ganttUtils.cpp

all: $(SOURCES) $(DEPS)
	$(CC) -o projectInfo $(SOURCES)

copy: all
	cp projectInfo test 

clean:
	rm projectInfo
