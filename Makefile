BASEDIR = .

INCLUDES = -I$(BASEDIR)

CXX = g++
CXXFLAGS = $(INCLUDES)

.SUFFIXES: .cc

.cc.o:
	$(CXX) $(CXXFLAGS) -c $<

.cc :
	$(CXX) $(CXXFLAGS) $< -o $@

SRC = OrderBook.cpp \
      OrderBookMain.cpp

OBJ = $(addsuffix .o, $(basename $(SRC)))

orderBook : $(OBJ)
		$(CXX) $(CXXFLAGS) -o $@ $(OBJ)

clean:
	rm -f $(OBJ) orderBook
