DOC_TARGET?=html

.PHONY: build
build:
	pip install .

.PHONY: format
format:
	find epiworldpy -path epiworldpy/include -prune -o \( -iname '*.hpp' -o -iname '*.cpp' \) -print | xargs clang-format -i

.PHONY: update
update:
	rsync -avz ../epiworld/include/epiworld include/

README.md: docs/README.qmd scripts/ppquarto.pl
	quarto render docs/README.qmd
	mv docs/README.md .
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
