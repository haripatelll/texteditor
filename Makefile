OBJ=out
SRC=source
INC=include

CXX = g++
CXXFLAGS = -std=c++14 -Wall -Werror=vla -g -MMD
SOURCES=${wildcard ${SRC}/*.cc}
OBJECTS=${patsubst ${SRC}/%.cc, ${OBJ}/%.o, ${SOURCES}}
EXEC=texteditor

-include ${DEPENDS}

${EXEC}: ${OBJECTS}
	 ${CXX} ${CXXFLAGS} ${OBJECTS} -o ${EXEC}

${OBJ}:
	mkdir -p $@

${OBJ}/%.o : $(addprefix ${SRC}/, %.cc) | ${OBJ}
	${CXX} ${CXXFLAGS} -c -o $@ $<

.PHONY : clean

clean:
	rm -r ${wildcard ${OBJ}} ${wildcard ${EXEC}}