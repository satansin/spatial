# DEBUG = y
TRIMESHDIR = ../../trimesh2
MAKERULESDIR = $(TRIMESHDIR)
INCLUDES += -I$(TRIMESHDIR)/include
LIBDIR += -L$(TRIMESHDIR)/lib.$(UNAME)

include $(MAKERULESDIR)/Makerules

SOURCES = icpconvergence.cc icprobustness.cc results2svg.cc

OFILES = $(addprefix $(OBJDIR)/,$(SOURCES:.cc=.o))

PROGS = $(SOURCES:.cc=$(EXE))
default : $(PROGS)

LIBS += -ltrimesh

$(PROGS) : %$(EXE) : $(OBJDIR)/%.o $(OTHEROFILES)
	$(LINK)

clean :
	-rm -f $(OFILES) $(OBJDIR)/Makedepend $(OBJDIR)/*.d
	-rm -rf $(OBJDIR)/ii_files
	-rmdir $(OBJDIR)

spotless : clean
	-rm -f $(PROGS)