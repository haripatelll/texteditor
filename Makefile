OBJ=out
SRC=source


CXX=gcc
CXXFLAGS=-Wall -Wextra
ONE= source/one/main.c
SOURCES=${wildcard ${SRC}/*.c}
OBJECTS=${patsubst ${SRC}/%.c, ${OBJ}/%.o, ${SOURCES}}
EXEC=texteditor

${EXEC}:
	${CXX} ${ONE} -o ${EXEC} ${CXXFLAGS}

${EXECC}: ${OBJECTS}
	${CXX} ${CXXFLAGS} ${OBJECTS} -o ${EXEC}

${OBJ}:
	mkdir -p $@

${OBJ}/%.o : $(addprefix ${SRC}/, %.c) | ${OBJ}
	${CXX} ${CXXFLAGS} -c -o $@ $<

.PHONY : clean

clean:
	rm -r ${wildcard ${OBJ}} ${wildcard ${EXEC}}

#gcc -Wall -W -pedantic  -c main.c -o main.o