
all:
	@echo ""
	@cd src ; $(MAKE) $(MFLAGS)
	@echo ""

clean:
	@echo ""
	@cd src ; $(MAKE) clean
	@echo ""
