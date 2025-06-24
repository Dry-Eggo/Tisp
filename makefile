OUT = out
SRCD= src
BIN = $(OUT)/tisp
SRC = $(wildcard $(SRCD)/*.cpp)
OBJ = $(patsubst $(SRCD)/%.cpp, $(OUT)/%.o, $(SRC))
HEADERS = $(wildcard headers/*.hpp)

FLAGS   = -Iheaders/

all: $(BIN)

$(BIN): $(OBJ)
	$(CXX) $(FLAGS) -o $(BIN) $^

$(OUT)/%.o: $(SRCD)/%.cpp
	$(CXX) -c -o $@ $< $(FLAGS)
