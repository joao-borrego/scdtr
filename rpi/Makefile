SERVER_EXEC ?= server.bin
CLIENT_EXEC ?= client.bin

BUILD_DIR ?= build
BIN_DIR ?= bin
SRC_DIR ?= src

SERVER_SRC := server.cpp System.cpp TCPServer.cpp TCPSession.cpp request.cpp
SERVER_SRC := $(addprefix $(SRC_DIR)/, $(SERVER_SRC))

CLIENT_SRC := client.cpp
CLIENT_SRC := $(addprefix $(SRC_DIR)/, $(CLIENT_SRC))

SERVER_OBJ := $(SERVER_SRC:%=$(BUILD_DIR)/%.o)
SERVER_DEP := $(SERVER_OBJ:.o=.d)

CLIENT_OBJ := $(CLIENT_SRC:%=$(BUILD_DIR)/%.o)
CLIENT_DEP := $(CLIENT_OBJ:.o=.d)

CPPFLAGS ?= -std=c++11

LDFLAGS ?= -lboost_system -lpthread

MKDIR_P ?= mkdir -p

all: $(BIN_DIR)/$(SERVER_EXEC) $(BIN_DIR)/$(CLIENT_EXEC) clean_build

# Server binary executable
$(BIN_DIR)/$(SERVER_EXEC): $(SERVER_OBJ)
	@$(MKDIR_P) $(dir $@)
	g++ $(SERVER_OBJ) -o $@ $(LDFLAGS)

# Client binary executable
$(BIN_DIR)/$(CLIENT_EXEC): $(CLIENT_OBJ)
	@$(MKDIR_P) $(dir $@)
	g++ $(CLIENT_OBJ) -o $@ $(LDFLAGS)

# C++ source
$(BUILD_DIR)/%.cpp.o: %.cpp
	@$(MKDIR_P) $(dir $@)
	g++ $(CPPFLAGS) $(CXXFLAGS) -c $< -o $@

.PHONY: clean

clean_build:
	$(RM) -r $(BUILD_DIR)

clean:
	$(RM) -r $(BUILD_DIR) $(BIN_DIR)

-include $(DEPS)
