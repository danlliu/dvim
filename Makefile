
CXX = g++ -std=c++17
CXXFLAGS = -O3 -I./src -Wall -Werror -Wpedantic -Wconversion # -DASCIIONLY
DBGFLAGS = -g3 -I./src -Wall -Werror -Wpedantic -Wconversion

SRC = src

DCURSES_CPP = $(wildcard $(SRC)/dcurses/*.cpp)
DCURSES_O = $(DCURSES_CPP:%.cpp=%.o)

DVIM_CPP = $(wildcard $(SRC)/dvim/*.cpp)
DVIM_O = $(DVIM_CPP:%.cpp=%.o)

UTIL_CPP = $(wildcard $(SRC)/Utilities.cpp)
UTIL_O = $(UTIL_CPP:%.cpp=%.o)

# No default rules.
.SUFFIXES:

dvim: $(SRC)/main.cpp $(DCURSES_O) $(DVIM_O) $(UTIL_O)
	$(CXX) $(CXXFLAGS) $(MODE) $^ -o $@

dvim_dbg: $(SRC)/main.cpp $(DCURSES_O:%.o=%.o.dbg) $(DVIM_O:%.o=%.o.dbg) $(UTIL_O:%.o=%.o.dbg)
	$(CXX) $(DBGFLAGS) $(MODE) $^ -o $@

.PHONY: debug
debug: dvim_dbg

$(SRC)/%.o: $(SRC)/%.cpp $(SRC)/%.hpp
	$(CXX) $(CXXFLAGS) $(MODE) -c $< -o $@

$(SRC)/%.o.dbg: $(SRC)/%.cpp
	$(CXX) $(DBGFLAGS) $(MODE) -c $^ -o $@

.PHONY: clean
clean: 
	find . -name "*.o" -delete
	find . -name "*.o.dbg" -delete
	rm -f dvim
	rm -f dvim_dbg
	rm -rf *.dSYM
