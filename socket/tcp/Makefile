all:tcp_srv tcp_process_srv tcp_thread_srv tcp_cli

tcp_srv:tcp_srv.cc
	g++ $< -o $@

tcp_process_srv:tcp_process_srv.cc
	g++ $< -o $@

tcp_thread_srv:tcp_thread_srv.cc
	g++ $< -o $@ -lpthread

tcp_cli:tcp_cli.cc
	g++ $< -o $@

.PHONY:clean
clean:
	rm -rf tcp_srv tcp_process_srv tcp_thread_srv tcp_cli
