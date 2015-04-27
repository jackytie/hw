target = 3_4_1.out 
CPPFLAGS = -O2 -std=c++11 -Wall

all: $(target)

%: %.cpp
	g++ $(CPPFLAGS) $< -o $@

3_4_1.out: int_calculator.cpp 
	g++ $(CPPFLAGS) $^ -o $@

%.o: %.cpp %.h
	g++ $(CPPFLAGS) -c -o $@ $<

clean:
	rm -rf $(target)

run:
	./3_4_1.out
