CFLAGS=-O0 -g -std=c++11
LDFLAGS=-lwt -lwthttp

squidlights: squidlights.cpp squidlightswidget.cpp lights.cpp
	g++ -o $@ $(CFLAGS) $^ $(LDFLAGS)

.PHONY: clean
clean:
	rm -f squidlights
