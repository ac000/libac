TARGETS = libac test

.PHONY: all $(TARGETS)
all: $(TARGETS)

MAKE_OPTS = --no-print-directory V=$V

.PHONY: libac
libac:
	@echo -e "Building: libac"
	@$(MAKE) $(MAKE_OPTS) -C src/

.PHONY: test
test:
	@echo -e "Building: test"
	@$(MAKE) $(MAKE_OPTS) -C src/ test

.PHONY: clean
clean:
	@echo -e "Cleaning: libac test"
	@$(MAKE) $(MAKE_OPTS) -C src/ clean
