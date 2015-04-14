target = demo 
CPPFLAGS = -O2 -std=c++11 -Wall
DataPath = /tmp2/KDDCup2012/track2/kddcup2012track2.txt

all: $(target)

%: %.c
	gcc $(CFLAGS) $< -o $@

%: %.cpp
	g++ $(CPPFLAGS) $< -o $@

demo: data_list.o data_list_main.cpp
	g++ $(CPPFLAGS) $^ -o $@

%.o: %.cpp %.h
	g++ $(CPPFLAGS) -c -o $@ $<

clean:
	rm -rf $(target)

run:
	./demo $(DataPath)
