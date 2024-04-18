cmake_minimum_required(VERSION 3.15)
project(LibraryManagementSystem)

link_libraries(mysqlclient)
set(SRC 
	${PROJECT_SOURCE_DIR}/main/main.c
	${PROJECT_SOURCE_DIR}/libManSys/libManSys.c
	${PROJECT_SOURCE_DIR}/sql/sql.c
)

include_directories(
	${PROJECT_SOURCE_DIR}/libManSys/libManSys.h 
	${PROJECT_SOURCE_DIR}/sql/sql.h
)

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
add_executable(${PROJECT_NAME} ${SRC})