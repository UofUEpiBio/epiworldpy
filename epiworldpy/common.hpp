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
auto vector_to_pylist(std::vector<T> &vec) -> pybind11::list {
	pybind11::list out;
	for (auto &v : *vec) {
		out.append(v);
	}

	pybind11::capsule free_when_done(
		vec, [](void *v) { delete reinterpret_cast<std::vector<T> *>(v); });

	out.attr("_capsule") = free_when_done;

	return out;
}

template <typename T>
auto vector_to_pyarray(std::vector<T> &vec) -> pybind11::array {
	pybind11::capsule free_when_done(
		&vec, [](void *v) { delete reinterpret_cast<std::vector<T> *>(v); });
	return pybind11::array(vec.size(), vec.data(), free_when_done);
}

static auto strings_to_pydict(std::vector<std::string> strings)
	-> pybind11::dict {
	pybind11::list values;
	std::unordered_map<std::string, int> map;
	std::vector<int> idx(strings.size());

	int next_id = 0;
	for (size_t i = 0; i < strings.size(); ++i) {
		auto &s = strings[i];
		auto it = map.find(s);
		if (it == map.end()) {
			int id = next_id++;
			map.emplace(s, id);
			values.append(s);
			idx[i] = id;
		} else {
			idx[i] = it->second;
		}
	}

	pybind11::array indexes(idx.size(), idx.data());

	pybind11::dict d;
	d["values"] = std::move(pybind11::array(values));
	d["indexes"] = std::move(indexes);
	return d;
}

template <typename T> auto to_python(std::vector<T> &vec) -> pybind11::object {
	if constexpr (std::is_same_v<T, int> || std::is_same_v<T, float> ||
				  std::is_same_v<T, double>) {
		return pybind11::object(vector_to_pyarray(vec));
	} else if constexpr (std::is_same_v<T, std::string>) {
		return strings_to_pydict(vec);
	} else {
		return vector_to_pylist(vec);
	}
}

template <typename T>
auto make_dict_entry(const char *key, std::vector<T> &vec)
	-> std::pair<const char *, pybind11::object> {
	return {key, to_python(vec)};
}

template <typename... Pairs>
auto make_dict(Pairs &&...pairs) -> pybind11::dict {
	pybind11::dict d;
	(void)std::initializer_list<int>{(d[pairs.first] = pairs.second, 0)...};
	return d;
}
} // namespace epiworldpy

#endif /* ~EPIWORLDPY_COMMON_HPP */
