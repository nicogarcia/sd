DIRS = project_tester exercise_1 exercise_2 exercise_3 \
	exercise_4 exercise_6 exercise_7
# the sets of directories to do various things in
BUILDDIRS = $(DIRS:%=build-%)
INSTALLDIRS = $(DIRS:%=install-%)
CLEANDIRS = $(DIRS:%=clean-%)
TESTDIRS = $(DIRS:%=test-%)
     
all: $(BUILDDIRS)
$(DIRS): $(BUILDDIRS)
$(BUILDDIRS):
	mkdir -p build/fedora
	mkdir -p build/fedora/project1_src
	cp -ru -t build/fedora/project1_src $(DIRS) lib/ Makefile
	tar cfa build/fedora/project1_src.tar.gz -C build/fedora project1_src/
	$(MAKE) -C $(@:build-%=%)

# the utils need the libraries in dev built first
build-utils: build-dev

install: $(INSTALLDIRS) all
$(INSTALLDIRS):
	$(MAKE) -C $(@:install-%=%) install

test: $(TESTDIRS) all
$(TESTDIRS): 
	$(MAKE) -C $(@:test-%=%) test

clean: $(CLEANDIRS)
$(CLEANDIRS):
	rm -rf build/ 
#	$(MAKE) -C $(@:clean-%=%) clean
	
.PHONY: subdirs $(DIRS)
.PHONY: subdirs $(BUILDDIRS)
.PHONY: subdirs $(INSTALLDIRS)
.PHONY: subdirs $(TESTDIRS)
.PHONY: subdirs $(CLEANDIRS)
.PHONY: all install clean test