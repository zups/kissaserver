cmake_minimum_required (VERSION 3.1)
project (
	kissaserver 
	LANGUAGES C
	)

find_package(Threads REQUIRED)

add_executable(client src/client.c)
add_executable(server src/server.c)
target_link_libraries(server PRIVATE Threads::Threads)
