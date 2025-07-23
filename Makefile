DOC_TARGET?=html

.PHONY: build
build:
	pip install .

.PHONY: format
format:
	find src -iname '*.hpp' -o -iname '*.cpp' | xargs clang-format -i

.PHONY: update
update:
	rsync -avz ../epiworld/include/epiworld include/

README.md: README.qmd
	quarto render README.qmd

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
