#include "database.hpp"
#include "common.hpp"
#include "config.hpp"

#include <pybind11/numpy.h>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

using namespace epiworld;
using namespace epiworldpy;
using namespace pybind11::literals;
namespace py = pybind11;

static auto get_hist_total(DataBase<int> &self) -> py::dict {
	auto states = new std::vector<std::string>;
	auto dates = new std::vector<int>;
	auto counts = new std::vector<int>;

	self.get_hist_total(dates, states, counts);

	return make_dict(make_dict_entry("dates", *dates),
					 make_dict_entry("states", *states),
					 make_dict_entry("counts", *counts));
}

static auto get_reproductive_number(DataBase<int> &self)
	-> std::vector<std::vector<std::map<int, int>>> {
	auto raw_rt = self.get_reproductive_number();

	/* Preallocate: [virus][day] -> { source: R } */
	std::vector<std::vector<std::map<int, int>>> result(
		self.get_n_viruses(),
		std::vector<std::map<int, int>>(self.get_model()->today() + 1));

	for (const auto &kv : raw_rt) {
		const auto &key = kv.first;
		int virus_id = key[0];
		int source = key[1];
		int exposure_day = key[2];
		int effective_rn = kv.second;

		result[virus_id][exposure_day][source] = effective_rn;
	}

	return result;
}

static auto get_transmissions(DataBase<int> &self) -> py::dict {
	auto dates = new std::vector<int>();
	auto *sources = new std::vector<int>();
	auto targets = new std::vector<int>();
	auto viruses = new std::vector<int>();
	auto source_exposure_dates = new std::vector<int>();

	self.get_transmissions(*dates, *sources, *targets, *viruses,
						   *source_exposure_dates);

	return make_dict(
		make_dict_entry("dates", *dates), make_dict_entry("sources", *sources),
		make_dict_entry("viruses", *viruses),
		make_dict_entry("source_exposure_dates", *source_exposure_dates));
}

static auto get_generation_time(DataBase<int> &self) -> py::dict {
	auto agents = new std::vector<int>();
	auto viruses = new std::vector<int>();
	auto times = new std::vector<int>();
	auto gentimes = new std::vector<int>();

	self.get_generation_time(*agents, *viruses, *times, *gentimes);

	return make_dict(make_dict_entry("agents", *agents),
					 make_dict_entry("viruses", *viruses),
					 make_dict_entry("times", *times),
					 make_dict_entry("generation_times", *gentimes));
}

static auto get_hist_transition_matrix(DataBase<int> &self, bool skip_zeros)
	-> py::dict {

	auto state_from = new std::vector<std::string>();
	auto state_to = new std::vector<std::string>();
	auto dates = new std::vector<int>();
	auto counts = new std::vector<int>();

	self.get_hist_transition_matrix(*state_from, *state_to, *dates, *counts,
									skip_zeros);

	return make_dict(make_dict_entry("state_from", *state_from),
					 make_dict_entry("state_to", *state_to),
					 make_dict_entry("dates", *dates),
					 make_dict_entry("counts", *counts));
}

static auto get_hist_virus(DataBase<int> &self) -> py::dict {
	auto dates = new std::vector<int>();
	auto ids = new std::vector<int>();
	auto counts = new std::vector<int>();
	auto states = new std::vector<std::string>();

	self.get_hist_virus(*dates, *ids, *states, *counts);

	return make_dict(
		make_dict_entry("dates", *dates), make_dict_entry("ids", *ids),
		make_dict_entry("states", *states), make_dict_entry("counts", *counts));
}

static auto get_hist_tool(DataBase<int> &self) -> py::dict {
	auto dates = new std::vector<int>();
	auto ids = new std::vector<int>();
	auto counts = new std::vector<int>();
	auto states = std::vector<std::string>();

	self.get_hist_tool(*dates, *ids, states, *counts);

	return make_dict(
		make_dict_entry("dates", *dates), make_dict_entry("ids", *ids),
		make_dict_entry("states", states), make_dict_entry("counts", *counts));
}

static auto get_today_transition_matrix(DataBase<int> &self) -> py::dict {
	auto counts = new std::vector<int>();
	self.get_today_transition_matrix(*counts);
	return make_dict(make_dict_entry("counts", *counts));
}

static auto get_today_virus(DataBase<int> &self) -> py::dict {
	auto states = new std::vector<std::string>();
	auto ids = new std::vector<int>();
	auto counts = new std::vector<int>();

	self.get_today_virus(*states, *ids, *counts);

	return make_dict(make_dict_entry("states", *states),
					 make_dict_entry("ids", *ids),
					 make_dict_entry("counts", *counts));
}

static auto get_today_total(DataBase<int> &self) -> py::dict {
	auto counts = new std::vector<int>();
	auto states = new std::vector<std::string>();

	self.get_today_total(states, counts);

	return make_dict(make_dict_entry("states", *states),
					 make_dict_entry("counts", *counts));
}

void epiworldpy::export_database(
	py::class_<DataBase<int>, std::shared_ptr<DataBase<int>>> &c) {
	c.def("add_user_data",
		  pybind11::detail::overload_cast_impl<std::vector<epiworld_double>>()(
			  &DataBase<int>::add_user_data),
		  "Add a list of user data.")
		.def("add_user_data",
			 pybind11::detail::overload_cast_impl<epiworld_fast_uint,
												  epiworld_double>()(
				 &DataBase<int>::add_user_data),
			 "Add a list of user data.")
		.def("get_n_tools", &DataBase<int>::get_n_tools,
			 "Get the number of tools.")
		.def("get_n_viruses", &DataBase<int>::get_n_viruses,
			 "Get the number of viruses.")
		.def("record_transmission", &DataBase<int>::record_transmission,
			 "Record a transmission event.")
		.def("write_data", &DataBase<int>::write_data, "Write some data.")
		.def("get_hist_virus", &get_hist_virus, "Get historical virus data.")
		.def("get_hist_tool", &get_hist_tool, "Get historical tool data.")
		.def("get_today_transition_matrix", &get_today_transition_matrix,
			 "Get today's transition matrix.")
		.def("get_today_virus", &get_today_virus, "Get today's virus data.")
		.def("get_today_total", &get_today_total, "Get today's total data.")
		.def("size", &DataBase<int>::size, "Get the size (number of entries).")
		.def("record", &DataBase<int>::record, "Register a new variant.")
		.def("reset", &DataBase<int>::reset, "Reset the database.")
		.def("record_tool", &DataBase<int>::record_tool,
			 "Add a new tool to the database.")
		.def("record_virus", &DataBase<int>::record_virus,
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
			 &DataBase<int>::get_transition_probability,
			 "Get the transition probably.");
}
