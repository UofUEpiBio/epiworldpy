#ifndef DEFM_COMMON_H
#define DEFM_COMMON_H

#include <pybind11/pybind11.h>
#include <random>

#ifdef _WIN32
#include <io.h>
#include <windows.h>
#define unlink _unlink
#else
#include <dirent.h>
#include <unistd.h>
#endif

#if !defined(_WIN32) && (defined(__unix__) || defined(__unix) ||               \
						 (defined(__APPLE__) && defined(__MACH__)))
#define EPIWORLD_PLATFORM_UNIX
#elif defined(__CYGWIN__) && !defined(_WIN32)
#define EPIWORLD_PLATFORM_UNIXISH
#elif defined(_WIN32) || defined(_WIN64)
#define EPIWORLD_PLATFORM_WINDOWS
#else
#define EPIWORLD_PLATFORM_UNKNOWN
#endif

#ifdef EPIWORLD_PLATFORM_WINDOWS
const char EPIWORLD_OS_PATHSEP = '\\';
#else
const char EPIWORLD_OS_PATHSEP = '/';
#endif

namespace epiworld {
inline void pyprinter(const char *fmt, ...) {
	char buffer[1024];

	va_list args;
	va_start(args, fmt);
	vsnprintf(buffer, sizeof(buffer), fmt, args);
	va_end(args);

	pybind11::print(std::string(buffer), pybind11::arg("end") = "");
}
} // namespace epiworld

#define printf_epiworld epiworld::pyprinter
#include "epiworld.hpp"

namespace epiworldpy {
static std::string parse_kwarg_string(const pybind11::kwargs &kwargs,
									  const char *key,
									  const std::string &_default) {
	PyObject *item = PyDict_GetItemString(kwargs.ptr(), key);

	if (item != nullptr) {
		return std::string(PyBytes_AS_STRING(
			PyUnicode_AsEncodedString(PyObject_Str(item), "utf-8", "?")));
	}

	return _default;
}

static int parse_kwarg_int(const pybind11::kwargs &kwargs, const char *key,
						   int _default) {
	PyObject *item = PyDict_GetItemString(kwargs.ptr(), key);

	if (item != nullptr) {
		return PyLong_AsLong(item);
	}

	return _default;
}

static bool parse_kwarg_bool(const pybind11::kwargs &kwargs, const char *key,
							 bool _default) {
	PyObject *item = PyDict_GetItemString(kwargs.ptr(), key);

	if (item != nullptr) {
		return item == Py_True;
	}

	return _default;
}

static std::string dirname(const std::string &filepath) {
	struct stat s;

#if EPIWORLD_PLATFORM_WINDOWS
	if (_stat(filepath.c_str(), &s) == 0) {
#else
	if (stat(filepath.c_str(), &s) == 0) {
#endif
		if (s.st_mode & S_IFREG) {
			goto treat_as_file;
		} else {
			return filepath;
		}
	} else {
		goto treat_as_file;
	}

treat_as_file:
	std::string directory;
	const size_t last_slash_idx = filepath.rfind(EPIWORLD_OS_PATHSEP);

	if (std::string::npos != last_slash_idx) {
		directory = filepath.substr(0, last_slash_idx);
	}

	return directory;
}

static std::vector<std::string> get_files_in_dir(const std::string &directory) {
	std::vector<std::string> found;

#ifdef _WIN32
	WIN32_FIND_DATA find_file_data;
	HANDLE hFind = FindFirstFile((directory + "\\*").c_str(), &find_file_data);

	if (hFind == INVALID_HANDLE_VALUE) {
		std::cerr << "Error opening directory: " << GetLastError() << std::endl;
		return found;
	}

	do {
		std::string file_name = find_file_data.cFileName;
		if (!(find_file_data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) {
			found.push_back(directory + "\\" + file_name);
		}
	} while (FindNextFile(hFind, &find_file_data) != 0);

	FindClose(hFind);
#else
	struct dirent *entry;
	DIR *dir = opendir(directory.c_str());

	if (dir == nullptr) {
		throw std::runtime_error(directory + ": " + strerror(errno));
	}

	while ((entry = readdir(dir)) != nullptr) {
		std::string file_name = entry->d_name;
		if (entry->d_type != DT_DIR) {
			found.push_back(directory + "/" + file_name);
		}
	}

	closedir(dir);
#endif

	return found;
}

inline std::string temp_id(size_t len) {
	const char alphanum[] =
		"0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_int_distribution<> dis(0, sizeof(alphanum) - 2);
	std::string id;

	id.reserve(len);

	for (size_t i = 0; i < len; i++) {
		id += alphanum[dis(gen)];
	}

	return id;
}

inline std::string temp_directory_path() {
	std::string env_to_check[]{"TMPDIR", "TMP", "TEMP", "TEMPDIR"};

	for (auto env : env_to_check) {
		char const *result = getenv(env.c_str());

		if (result != nullptr) {
			return result;
		}
	}

	/* Otherwise, default to a value hardcoded per platform. */
#if defined(EPIWORLD_PLATFORM_UNIX) || defined(EPIWORLD_PLATFORM_UNIXISH)
	return "/tmp/";
#elif defined(EPIWORLD_PLATFORM_WINDOWS)
	/* I can't see us ever getting here, Windows isn't at heterogeneous as UNIX.
	 */
	throw std::runtime_error("TEMP not defined on Windows, are you nuts!?");
#elif EPIWORLD_PLATFORM_UNKNOWN
	return ""; /* Current directory. */
#endif
}
} // namespace epiworldpy

#endif
