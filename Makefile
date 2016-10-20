all:
	g++ -lpthread main.cpp -o Proxy

clean:
	rm -f a.out Proxy tmp/*.txt *.txt