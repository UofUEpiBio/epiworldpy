#include "database.hpp"
#include "config.hpp"

#include <pybind11/numpy.h>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

using namespace epiworld;
using namespace epiworldpy;
using namespace pybind11::literals;
namespace py = pybind11;

static py::dict get_hist_total(DataBase<int> &self) {
	/* Lo, one of the times in modern C++ where the 'new' keyword
	 * isn't out of place. */
	std::vector<std::string> states;
	std::vector<int> *dates = new std::vector<int>();
	std::vector<int> *counts = new std::vector<int>();

	self.get_hist_total(dates, &states, counts);

	/* Return to Python. */
	py::capsule pyc_dates(
		dates, [](void *x) { delete reinterpret_cast<std::vector<int> *>(x); });
	py::capsule pyc_counts(counts, [](void *x) {
		delete reinterpret_cast<std::vector<int> *>(x);
	});

	/* TODO: Find a way to do a no-copy of a string vector. */
	py::array py_states = py::array(py::cast(states));
	py::array_t<int> py_dates(dates->size(), dates->data(), pyc_dates);
	py::array_t<int> py_counts(counts->size(), counts->data(), pyc_counts);

	py::dict ret("dates"_a = py_dates, "states"_a = py_states,
				 "counts"_a = py_counts);

	return ret;
}

static std::vector<std::vector<std::map<int, int>>>
get_reproductive_number(DataBase<int> &self) {
	MapVec_type<int, int> raw_rt = self.get_reproductive_number();
	// viruses | dates     | pairs
	// C       | C         | V
	std::vector<std::vector<std::map<int, int>>> viruses;

	/* Reserve our spaces for our elements so we don't have to
	 * worry about it later. */
	for (int virus_id = 0; virus_id < self.get_n_viruses(); virus_id++) {
		std::vector<std::map<int, int>> dates(self.get_model()->today() + 1);
		viruses.push_back(dates);
	}

	/* Load into pre-return. */
	for (const auto &keyValue : raw_rt) {
		const std::vector<int> &key = keyValue.first;
		const int virus_id = key.at(0);
		const int source = key.at(1);
		const int exposure_date = key.at(2);
		const int effective_rn = keyValue.second;

		viruses[virus_id][exposure_date].insert({source, effective_rn});
	}

	/* TODO: There's lots room for optimization here, namely
	 * returning an array instead of a bunch of lists. */
	return viruses;
}

static py::dict get_transmissions(DataBase<int> &self) {
	/* Lo, one of the times in modern C++ where the 'new' keyword
	 * isn't out of place. */
	std::vector<int> *dates = new std::vector<int>();
	std::vector<int> *sources = new std::vector<int>();
	std::vector<int> *targets = new std::vector<int>();
	std::vector<int> *viruses = new std::vector<int>();
	std::vector<int> *source_exposure_dates = new std::vector<int>();

	self.get_transmissions(*dates, *sources, *targets, *viruses,
						   *source_exposure_dates);

	/* Return to Python. */
	py::capsule pyc_dates(
		dates, [](void *x) { delete reinterpret_cast<std::vector<int> *>(x); });
	py::capsule pyc_sources(sources, [](void *x) {
		delete reinterpret_cast<std::vector<int> *>(x);
	});
	py::capsule pyc_targets(targets, [](void *x) {
		delete reinterpret_cast<std::vector<int> *>(x);
	});
	py::capsule pyc_viruses(viruses, [](void *x) {
		delete reinterpret_cast<std::vector<int> *>(x);
	});
	py::capsule pyc_source_exposure_dates(source_exposure_dates, [](void *x) {
		delete reinterpret_cast<std::vector<int> *>(x);
	});

	py::array py_dates(dates->size(), dates->data(), pyc_dates);
	py::array py_sources(sources->size(), sources->data(), pyc_sources);
	py::array py_targets(targets->size(), targets->data(), pyc_targets);
	py::array py_viruses(viruses->size(), viruses->data(), pyc_viruses);
	py::array py_source_exposure_dates(source_exposure_dates->size(),
									   source_exposure_dates->data(),
									   pyc_source_exposure_dates);

	py::dict ret("dates"_a = py_dates, "sources"_a = py_sources,
				 "targets"_a = py_targets, "viruses"_a = py_viruses,
				 "source_exposure_dates"_a = py_source_exposure_dates);

	return ret;
}

static py::dict get_generation_time(DataBase<int> &self) {
	std::vector<int> *agents = new std::vector<int>();
	std::vector<int> *viruses = new std::vector<int>();
	std::vector<int> *times = new std::vector<int>();
	std::vector<int> *gentimes = new std::vector<int>();

	self.get_generation_time(*agents, *viruses, *times, *gentimes);

	/* Return to Python. */
	py::capsule pyc_agents(agents, [](void *x) {
		delete reinterpret_cast<std::vector<int> *>(x);
	});
	py::capsule pyc_viruses(viruses, [](void *x) {
		delete reinterpret_cast<std::vector<int> *>(x);
	});
	py::capsule pyc_times(
		times, [](void *x) { delete reinterpret_cast<std::vector<int> *>(x); });
	py::capsule pyc_gentimes(gentimes, [](void *x) {
		delete reinterpret_cast<std::vector<int> *>(x);
	});

	py::array py_agents(agents->size(), agents->data(), pyc_agents);
	py::array py_viruses(viruses->size(), viruses->data(), pyc_viruses);
	py::array py_times(times->size(), times->data(), pyc_times);
	py::array py_gentimes(gentimes->size(), gentimes->data(), pyc_gentimes);

	py::dict ret("agents"_a = py_agents, "viruses"_a = py_viruses,
				 "times"_a = py_times, "gentimes"_a = py_gentimes);

	return ret;
}

static py::dict get_hist_transition_matrix(DataBase<int> &self,
										   bool skip_zeros) {
	std::vector<std::string> *state_from = new std::vector<std::string>();
	std::vector<std::string> *state_to = new std::vector<std::string>();
	std::vector<int> *dates = new std::vector<int>();
	std::vector<int> *counts = new std::vector<int>();

	self.get_hist_transition_matrix(*state_from, *state_to, *dates, *counts,
									skip_zeros);

	/* Return to Python. */
	py::capsule pyc_dates(
		dates, [](void *x) { delete reinterpret_cast<std::vector<int> *>(x); });
	py::capsule pyc_counts(counts, [](void *x) {
		delete reinterpret_cast<std::vector<int> *>(x);
	});

	py::array py_dates(dates->size(), dates->data(), pyc_dates);
	py::array py_counts(counts->size(), counts->data(), pyc_counts);

	py::dict ret("states_from"_a = state_from, "states_to"_a = state_to,
				 "dates"_a = py_dates, "counts"_a = py_counts);

	return ret;
}

static py::dict get_hist_virus(DataBase<int> &self) {
	std::vector<std::string> *states = new std::vector<std::string>();
	std::vector<int> *dates = new std::vector<int>();
	std::vector<int> *ids = new std::vector<int>();
	std::vector<int> *counts = new std::vector<int>();

	self.get_hist_virus(*dates, *ids, *states, *counts);

	/* Return to Python. */
	py::capsule pyc_dates(
		dates, [](void *x) { delete reinterpret_cast<std::vector<int> *>(x); });
	py::capsule pyc_ids(
		ids, [](void *x) { delete reinterpret_cast<std::vector<int> *>(x); });
	py::capsule pyc_counts(counts, [](void *x) {
		delete reinterpret_cast<std::vector<int> *>(x);
	});

	py::array py_dates(dates->size(), dates->data(), pyc_dates);
	py::array py_ids(ids->size(), ids->data(), pyc_ids);
	py::array py_counts(counts->size(), counts->data(), pyc_counts);

	py::dict ret("dates"_a = dates, "ids"_a = py_ids, "states"_a = states,
				 "counts"_a = py_counts);

	return ret;
}

static py::dict get_hist_tool(DataBase<int> &self) {
	std::vector<std::string> *states = new std::vector<std::string>();
	std::vector<int> *dates = new std::vector<int>();
	std::vector<int> *ids = new std::vector<int>();
	std::vector<int> *counts = new std::vector<int>();

	self.get_hist_tool(*dates, *ids, *states, *counts);

	/* Return to Python. */
	py::capsule pyc_dates(
		dates, [](void *x) { delete reinterpret_cast<std::vector<int> *>(x); });
	py::capsule pyc_ids(
		ids, [](void *x) { delete reinterpret_cast<std::vector<int> *>(x); });
	py::capsule pyc_counts(counts, [](void *x) {
		delete reinterpret_cast<std::vector<int> *>(x);
	});

	py::array py_dates(dates->size(), dates->data(), pyc_dates);
	py::array py_ids(ids->size(), ids->data(), pyc_ids);
	py::array py_counts(counts->size(), counts->data(), pyc_counts);

	py::dict ret("dates"_a = dates, "ids"_a = py_ids, "states"_a = states,
				 "counts"_a = py_counts);

	return ret;
}

static py::dict get_today_transition_matrix(DataBase<int> &self) {
	std::vector<int> *counts = new std::vector<int>();

	self.get_today_transition_matrix(*counts);

	/* Return to Python. */
	py::capsule pyc_counts(counts, [](void *x) {
		delete reinterpret_cast<std::vector<int> *>(x);
	});

	py::array py_counts(counts->size(), counts->data(), pyc_counts);

	py::dict ret("counts"_a = py_counts);

	return ret;
}

static py::dict get_today_virus(DataBase<int> &self) {
	std::vector<std::string> *states = new std::vector<std::string>();
	std::vector<int> *ids = new std::vector<int>();
	std::vector<int> *counts = new std::vector<int>();

	self.get_today_virus(*states, *ids, *counts);

	/* Return to Python. */
	py::capsule pyc_ids(
		ids, [](void *x) { delete reinterpret_cast<std::vector<int> *>(x); });
	py::capsule pyc_counts(counts, [](void *x) {
		delete reinterpret_cast<std::vector<int> *>(x);
	});

	py::array py_ids(ids->size(), ids->data(), pyc_ids);
	py::array py_counts(counts->size(), counts->data(), pyc_counts);

	py::dict ret("states"_a = states, "ids"_a = py_ids, "counts"_a = py_counts);

	return ret;
}

static py::dict get_today_total(DataBase<int> &self) {
	std::vector<std::string> *states = new std::vector<std::string>();
	std::vector<int> *counts = new std::vector<int>();

	self.get_today_total(states, counts);

	/* Return to Python. */
	py::capsule pyc_counts(counts, [](void *x) {
		delete reinterpret_cast<std::vector<int> *>(x);
	});

	py::array py_counts(counts->size(), counts->data(), pyc_counts);

	py::dict ret("states"_a = states, "counts"_a = py_counts);

	return ret;
}

void epiworldpy::export_database(
	py::class_<epiworld::DataBase<int>,
			   std::shared_ptr<epiworld::DataBase<int>>> &c) {
	c.def("add_user_data",
		  pybind11::detail::overload_cast_impl<std::vector<epiworld_double>>()(
			  &epiworld::DataBase<int>::add_user_data),
		  "Add a list of user data.")
		.def("add_user_data",
			 pybind11::detail::overload_cast_impl<epiworld_fast_uint,
												  epiworld_double>()(
				 &epiworld::DataBase<int>::add_user_data),
			 "Add a list of user data.")
		.def("get_n_tools", &epiworld::DataBase<int>::get_n_tools,
			 "Get the number of tools.")
		.def("get_n_viruses", &epiworld::DataBase<int>::get_n_viruses,
			 "Get the number of viruses.")
		.def("record_transmission",
			 &epiworld::DataBase<int>::record_transmission,
			 "Record a transmission event.")
		.def("write_data", &epiworld::DataBase<int>::write_data,
			 "Write some data.")
		.def("get_hist_virus", &get_hist_virus, "Get historical virus data.")
		.def("get_hist_tool", &get_hist_tool, "Get historical tool data.")
		.def("get_today_transition_matrix", &get_today_transition_matrix,
			 "Get today's transition matrix.")
		.def("get_today_virus", &get_today_virus, "Get today's virus data.")
		//.def("get_today_total",
		// pybind11::detail::overload_cast_impl<epiworld_fast_uint>()(&epiworld::DataBase<int>::get_today_total),
		//"Get today's total data.")
		.def("get_today_total", &get_today_total, "Get today's total data.")
		.def("size", &epiworld::DataBase<int>::size,
			 "Get the size (number of entries).")
		.def("record", &epiworld::DataBase<int>::record,
			 "Register a new variant.") // ?
		.def("reset", &epiworld::DataBase<int>::reset,
			 "Reset the database.") // ?
		.def("record_tool", &epiworld::DataBase<int>::record_tool,
			 "Add a new tool to the database.")
		.def("record_virus", &epiworld::DataBase<int>::record_virus,
			 "Add a new virus to the database.")
		.def("get_hist_total", &get_hist_total,
			 "Get historical totals for this model run.")
		.def("get_reproductive_number", &get_reproductive_number,
			 "Get reproductive numbers over time for every virus in the model.")
		.def("get_transmissions", &get_transmissions,
			 "Get transmission data over time for every virus in the model.")
		.def("get_generation_time", &get_generation_time,
			 "Get generation times over time for every virus in the model.")
		.def("get_hist_transition_matrix", &get_hist_transition_matrix,
			 "Get historical transitions in a tabular format.")
		.def("get_transition_probability",
			 &epiworld::DataBase<int>::get_transition_probability,
			 "Get the transition probably.");
}
