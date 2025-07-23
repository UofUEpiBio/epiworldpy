#include "saver.hpp"

#include <sstream>

using namespace epiworld;
using namespace epiworldpy;
namespace py = pybind11;

void Saver::set_cvsloc(std::string cvsloc) { this->csvloc = cvsloc; }

const std::string_view Saver::get_cvsloc() const { return csvloc; }

std::ostream &operator<<(std::ostream &stream, const Saver &data) {
	data.out(stream);
	return stream;
}

Saver::Saver(std::vector<std::string> what, std::string fn, std::string id,
			 bool file_output)
	: fun(epiworld::make_save_run<int>(
		  fn, std::find(what.begin(), what.end(), "total_hist") != what.end(),
		  std::find(what.begin(), what.end(), "virus_info") != what.end(),
		  std::find(what.begin(), what.end(), "virus_hist") != what.end(),
		  std::find(what.begin(), what.end(), "tool_info") != what.end(),
		  std::find(what.begin(), what.end(), "tool_hist") != what.end(),
		  std::find(what.begin(), what.end(), "transmission") != what.end(),
		  std::find(what.begin(), what.end(), "transition") != what.end(),
		  std::find(what.begin(), what.end(), "reproductive") != what.end(),
		  std::find(what.begin(), what.end(), "generation") != what.end())),
	  what(what), fn(fn), id(id), file_output(file_output) {}

void Saver::unlink_siblings() const {
	auto dir = dirname(fn);
	auto contestants = get_files_in_dir(dir);

	for (auto contestant : contestants) {
		if (unlink(contestant.c_str()) != 0 && errno != ENOENT) {
			throw std::runtime_error("Failed to remove file " + contestant +
									 ": " + strerror(errno));
		}
	}
}

const std::ostream &Saver::out(std::ostream &stream) const {
	stream << "A saver for -run_multiple-" << std::endl;
	stream << "Saves the following: ";

	for (const auto whatum : what) {
		stream << whatum;

		if (what.back() != whatum) {
			stream << ", ";
		} else {
			stream << std::endl;
		}
	}

	stream << "To file            : " << (file_output ? "yes" : "no")
		   << std::endl;
	if (file_output) {
		stream << "Saver pattern      : " << fn << std::endl;
	}

	return stream;
}

std::function<void(size_t, epiworld::Model<int> *)> Saver::operator*() {
	return fun;
}

void epiworldpy::export_saver(
	pybind11::class_<Saver, std::shared_ptr<Saver>> &c) {
	c.def(py::init([](py::args args, const py::kwargs &kwargs) {
		/* TODO: Verify that this has the same effect as `make_saver` in:
		 *  https://github.com/UofUEpiBio/epiworldR/blob/main/R/make_saver.R
		 */

		bool file_output = true;
		struct stat sinfo;
		std::string fn = parse_kwarg_string(kwargs, "fn", "");
		std::string id = temp_id(5);
		std::vector<std::string> whats;
		std::vector<std::string> valid_whats = {
			"total_hist", "virus_info",	  "virus_hist",
			"tool_info",  "tool_hist",	  "transmission",
			"transition", "reproductive", "generation"};

		/* Make sure valid arguments are passed into this constructor, and
		 * marshall things out all the same. */
		for (auto arg : args) {
			std::string whatum = arg.cast<std::string>();

			if (std::find(valid_whats.begin(), valid_whats.end(), whatum) ==
				valid_whats.end()) {
				throw std::invalid_argument("What '" + whatum +
											"' is not supported.");
			}

			whats.push_back(whatum);
		}

		/* Handle the filename. If only we have C++17's std::filesystem... */
		if (fn.empty()) {
			int error = 0;
			std::string norm = temp_directory_path() + EPIWORLD_OS_PATHSEP +
							   "epiworldpy-" + id;

#ifdef EPIWORLD_PLATFORM_WINDOWS
			error = _mkdir(norm.c_str());
#else
			error = mkdir(norm.c_str(), 0733);
#endif

			if (error != 0) {
				throw std::runtime_error(strerror(error));
			}

			fn = norm + EPIWORLD_OS_PATHSEP + "%05lu-episimulation.csv";
			file_output = false;
#if EPIWORLD_PLATFORM_WINDOWS
		} else if (_stat(fn.c_str(), &sinfo) != 0) {
#else
		} else if (stat(fn.c_str(), &sinfo) != 0) {
#endif
			throw std::runtime_error("The directory \"" + fn +
									 "\" does not exist.");
		}

		return epiworldpy::Saver(whats, fn, id, file_output);
	}));

	c.def("run_multiple", [](Saver &self, Model<int> &model, int ndays,
							 int nsims, const py::kwargs &kwargs) {
		int seed = parse_kwarg_int(kwargs, "seed", std::time(0));
		int nthreads = parse_kwarg_int(kwargs, "nthreads", 1);
		bool reset = parse_kwarg_bool(kwargs, "reset", true);
		bool verbose = parse_kwarg_bool(kwargs, "verbose", true);

		/* Do we have previously saved files? */
		self.unlink_siblings();

		/* Dispatch! */
		model.run_multiple(ndays, nsims, seed, *self, reset, verbose, nthreads);

		/* EpiworldR does this so we do too. */
		return model;
	});

	c.def("run_multiple_get_results", [](Saver &self, const py::args &args) {
		auto csvloc = self.get_cvsloc();
		std::vector<std::vector<std::vector<int>>> results;
		std::vector<std::string> whats;
		std::vector<std::string> valid_whats = {
			"total_hist", "virus_info",	  "virus_hist",
			"tool_info",  "tool_hist",	  "transmission",
			"transition", "reproductive", "generation"};

		/* Make sure valid arguments are passed into this constructor, and
		 * marshall things out all the same. */
		for (auto arg : args) {
			std::string whatum = arg.cast<std::string>();

			if (std::find(valid_whats.begin(), valid_whats.end(), whatum) ==
				valid_whats.end()) {
				throw std::invalid_argument("What '" + whatum +
											"' is not supported.");
			}

			whats.push_back(whatum);
		}

		for (auto what : whats) {
			std::ifstream t(std::string(csvloc) + what);
			std::stringstream buffer;
			buffer << t.rdbuf();

			std::vector<std::vector<int>> result;
			std::string line;

			while (std::getline(buffer, line)) {
				std::vector<std::string> row;
				std::stringstream line_stream(line);
				std::string cell;

				while (std::getline(line_stream, cell, ',')) {
					row.push_back(cell);
				}

				result.push_back(std::vector<int>(std::atoi(row[0].c_str())));
			}

			results.push_back(result);
		}

		return results;
	});
}
