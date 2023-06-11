# convenient makefile to run some static analyzers:
#	$ make -f etc/analyze.mk

nproc != nproc
MAKEFLAGS := -j$(nproc)

analyze: analyze-gcc analyze-cppcheck analyze-clang-tidy analyze-clang-weverything
analyze-gcc:
	gcc sx4.c -o /dev/null -c -std=c99 -Wall -Wextra -Wpedantic -fanalyzer \
		-Ofast -fwhole-program
analyze-cppcheck:
	cppcheck sx4.c --std=c99 --quiet --inline-suppr --force \
		--enable=performance,portability,style --max-ctu-depth=16 \
		--suppress=constVariable --suppress=variableScope
analyze-clang-tidy:
	clang-tidy --config-file=./etc/.clang-tidy sx4.c --quiet -- -std=c99
analyze-clang-weverything:
	clang sx4.c -o /dev/null -c -std=c99 -Ofast -Weverything \
		-Wno-unreachable-code-break -Wno-comma -Wno-padded \
		-Wno-disabled-macro-expansion -Wno-declaration-after-statement \
		-Wno-shorten-64-to-32 -Wno-implicit-int-conversion \
		-Wno-sign-conversion -Wno-string-conversion -Wno-switch-enum \
		-Wno-tautological-type-limit-compare -Wno-missing-noreturn

.PHONY: analyze
