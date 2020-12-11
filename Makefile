# Makefile built from https://hiltmon.com/blog/2013/07/03/a-simple-c-plus-plus-project-structure/ example

CC := g++ 

SRCDIR := src
BUILDDIR := build
TARGET := bin/antColony-CL

SRCEXT := cc
SOURCES := $(shell find $(SRCDIR) -type f -name *.$(SRCEXT))
OBJECTS := $(patsubst $(SRCDIR)/%,$(BUILDDIR)/%,$(SOURCES:.$(SRCEXT)=.o))
CFLAGS := -g -Wall -Wfatal-errors -fopenmp -O3
LIB := 
INC := -I include

$(TARGET): $(OBJECTS)
	@echo " Linking..."
	@echo " $(CC) $^ -o $(TARGET) $(LIB) -fopenmp"; $(CC) $^ -o $(TARGET) $(LIB) -fopenmp

$(BUILDDIR)/%.o: $(SRCDIR)/%.$(SRCEXT)
	@mkdir -p $(BUILDDIR)
	@echo " $(CC) $(CFLAGS) $(INC) -c -o $@ $<"; $(CC) $(CFLAGS) $(INC) -c -o $@ $<

clean:
	@echo " Cleaning..."; 
	@echo " $(RM) -r $(BUILDDIR) $(TARGET)"; $(RM) -r $(BUILDDIR) $(TARGET)

# Tests

# Spikes
ticket:
	$(CC) $(CFLAGS) spikes/ticket.cc $(INC) $(LIB) -o bin/ticket

.PHONY: clean