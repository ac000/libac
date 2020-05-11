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

.PHONY: rpm
rpm:
	@echo -e "Building: rpm"
ifeq ($(wildcard ~/rpmbuild/),)
	@echo "***"
	@echo "*** ~/rpmbuild not found, create with"
	@echo "***"
	@echo "***    mkdir -p ~/rpmbuild/{BUILD,RPMS,SOURCES,SPECS,SRPMS}"
	@echo "***"
	@false
else
	@version=$$(git describe | tail -c +2); echo "Building $${version}"; \
		git archive --prefix=libac-$${version%%-*}/ -o ~/rpmbuild/SOURCES/libac-$${version%%-*}.tar HEAD;
	@rpmbuild -bb libac.spec
endif

.PHONY: clean
clean:
	@echo -e "Cleaning: libac test"
	@$(MAKE) $(MAKE_OPTS) -C src/ clean
