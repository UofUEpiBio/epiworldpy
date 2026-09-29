DOC_TARGET?=html

.PHONY: build
build:
	pip install .

.PHONY: format
format:
	find epiworldpy -path epiworldpy/include -prune -o \( -iname '*.hpp' -o -iname '*.cpp' \) -print | xargs clang-format -i

# Branch (or tag) of epiworld to pull the headers from
EPIWORLD_BRANCH ?= master
EPIWORLD_REPO ?= https://github.com/UofUEpiBio/epiworld

# Update the vendored C++ headers from a local epiworld checkout
.PHONY: local-update
local-update:
	rsync -avz --delete ../epiworld/include/epiworld/ epiworldpy/include/epiworld/

# Update the vendored C++ headers from GitHub
.PHONY: update
update: clean-update
	git clone --depth=1 -b $(EPIWORLD_BRANCH) $(EPIWORLD_REPO) tmp_epiworld && \
		rsync -avz --delete tmp_epiworld/include/epiworld/ epiworldpy/include/epiworld/ && \
		rm -rf tmp_epiworld

.PHONY: clean-update
clean-update:
	rm -rf tmp_epiworld

docs/intro.md: docs/intro.qmd scripts/ppquarto.pl
	quarto render docs/intro.qmd
	perl scripts/ppquarto.pl docs/intro.qmd docs/intro.qmd

README.md: docs/README.qmd scripts/ppquarto.pl
	quarto render docs/README.qmd
	mv docs/README.md README.md
	perl scripts/ppquarto.pl README.md README.md

.PHONY: compile_commands.json
compile_commands.json:
	cmake -S . -B build -DCMAKE_EXPORT_COMPILE_COMMANDS=ON
	cp compile_commands.json ..
	rm -rf build

.PHONY: cpp_only
configure:
	cmake -S . -B build -DCMAKE_EXPORT_COMPILE_COMMANDS=ON

.PHONY: docs
docs:
	$(MAKE) -C docs $(DOC_TARGET)
