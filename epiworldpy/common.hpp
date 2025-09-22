#ifndef EPIWORLDPY_COMMON_HPP
#define EPIWORLDPY_COMMON_HPP

#include <pybind11/numpy.h>
#include <pybind11/pybind11.h>
#include <vector>

namespace epiworld {
static const unsigned int PYPRINTER_BUFFER_SIZE = 1024;

inline void pyprinter(const char *fmt, ...) {
	auto buffer = std::array<char, PYPRINTER_BUFFER_SIZE>{0};

	va_list args;
	va_start(args, fmt);
	vsnprintf(buffer.data(), sizeof(buffer), fmt, args);
	va_end(args);

	pybind11::print(std::string(buffer.data()), pybind11::arg("end") = "");
}
} // namespace epiworld

#define printf_epiworld epiworld::pyprinter
#include "epiworld.hpp"

/* Data translation. */

namespace epiworldpy {
template <typename T>
auto vector_to_pyarray(std::vector<T> *vec) -> pybind11::array {
	pybind11::capsule free_when_done(
		vec, [](void *v) { delete reinterpret_cast<std::vector<T> *>(v); });
	return pybind11::array(vec->size(), vec->data(), free_when_done);
}

template <typename T>
auto make_entry(const char *key, std::vector<T> *vec)
	-> std::pair<const char *, pybind11::array> {
	return {key, vector_to_pyarray(vec)};
}

template <typename... Pairs>
auto make_dict(Pairs &&...pairs) -> pybind11::dict {
	pybind11::dict d;
	(void)std::initializer_list<int>{
		((d[pybind11::str(pairs.first)] = pairs.second), 0)...};
	return d;
}
} // namespace epiworldpy

#endif /* ~EPIWORLDPY_COMMON_HPP */
