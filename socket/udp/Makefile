all:udp_srv udp_cli

udp_srv:udp_srv.cc
	g++ $< -o $@

udp_cli:udp_cli.cc
	g++ $< -o $@

.PHONY:clean
clean:
	rm -rf udp_srv udp_cli
