CXX      := g++
CXXFLAGS := -O0 -g -std=c++11
INCLUDES := -I include
LDFLAGS  := -lwt -lwthttp -lboost_system -lboost_filesystem -lola -lolacommon

SRCS := $(wildcard src/*.cpp)
OBJS := $(patsubst src/%.cpp,build/%.o,$(SRCS))
DEPS := $(patsubst src/%.cpp,build/%.d,$(SRCS))

build/squidlights: $(OBJS)
	@echo "LINK $^"
	@$(CXX) -o $@ $(LDFLAGS) $^

# pull in dependency info for existing .o files
-include $(DEPS)

build/%.o: src/%.cpp
	@echo "CXX $<"
	@$(CXX) -c -o $@ -MMD $(CXXFLAGS) $(INCLUDES) $<

.PHONY: clean
clean:
	rm -f build/*
