all: server tests_exec

server: app/runner.cpp conf/config.cpp strings/resolve.cpp
	g++ -Iconf -Iapp -Istrings -pthread -o server app/runner.cpp conf/config.cpp strings/resolve.cpp

tests_exec: tests/tests.cpp strings/resolve.cpp
	g++ -Iapp -Istrings -pthread -o tests_exec tests/tests.cpp strings/resolve.cpp

clean:
	rm -f server tests_exec
