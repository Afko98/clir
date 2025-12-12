CXX = g++
CXXFLAGS = -std=c++17 -Wall -O2
LDFLAGS = -lsqlite3

SRC = src
BUILD = build
OBJS = $(SRC)/main.o $(SRC)/dbController.o
TARGET = $(BUILD)/clir

all: $(BUILD) $(TARGET)

# Create build folder if it doesn't exist
$(BUILD):
	mkdir -p $(BUILD)

$(TARGET): $(OBJS) | $(BUILD)
	$(CXX) $(CXXFLAGS) -o $@ $(OBJS) $(LDFLAGS)

$(SRC)/%.o: $(SRC)/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)
