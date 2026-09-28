#include "database.hpp"
#include "docstrings/database.hpp"
#include "common.hpp"
#include "config.hpp"

#include <pybind11/numpy.h>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

using namespace epiworld;
using namespace epiworldpy;
using namespace pybind11::literals;
namespace py = pybind11;
namespace doc = epiworldpy::docstrings::database;

static auto get_hist_total(DataBase<int> &self) -> py::dict {
	auto states = std::vector<std::string>();
	auto dates = new std::vector<int>;
	auto counts = new std::vector<int>;

	self.get_hist_total(dates, &states, counts);

	return make_dict(make_dict_entry("dates", *dates),
					 make_dict_entry("states", states),
					 make_dict_entry("counts", *counts));
}

static auto get_reproductive_number(DataBase<int> &self)
	-> py::array_t<long long> {
	auto raw_rt = self.get_reproductive_number();

	auto nrows = static_cast<py::ssize_t>(raw_rt.size());
	py::ssize_t ncols = 4;

	py::array_t<long long> arr({nrows, ncols});
	auto buf = arr.mutable_unchecked<2>();

	py::ssize_t i = 0;
	for (const auto &kv : raw_rt) {
		const auto &key = kv.first;
		buf(i, 0) = static_cast<long long>(key[0]);
		buf(i, 1) = static_cast<long long>(key[2]);
		buf(i, 2) = static_cast<long long>(key[1]);
		buf(i, 3) = static_cast<long long>(kv.second);
		++i;
	}

	return arr;
}

static auto get_transmissions(DataBase<int> &self) -> py::dict {
	auto dates = new std::vector<int>();
	auto sources = new std::vector<int>();
	auto targets = new std::vector<int>();
	auto viruses = new std::vector<int>();
	auto source_exposure_dates = new std::vector<int>();

	self.get_transmissions(*dates, *sources, *targets, *viruses,
						   *source_exposure_dates);

	return make_dict(
		make_dict_entry("dates", *dates), make_dict_entry("sources", *sources),
		make_dict_entry("targets", *targets),
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

	auto state_from = std::vector<std::string>();
	auto state_to = std::vector<std::string>();
	auto dates = new std::vector<int>();
	auto counts = new std::vector<int>();

	self.get_hist_transition_matrix(state_from, state_to, *dates, *counts,
									skip_zeros);

	return make_dict(make_dict_entry("state_from", state_from),
					 make_dict_entry("state_to", state_to),
					 make_dict_entry("dates", *dates),
					 make_dict_entry("counts", *counts));
}

static auto get_hist_virus(DataBase<int> &self) -> py::dict {
	auto dates = new std::vector<int>();
	auto ids = new std::vector<int>();
	auto counts = new std::vector<int>();
	auto states = std::vector<std::string>();

	self.get_hist_virus(*dates, *ids, states, *counts);

	return make_dict(
		make_dict_entry("dates", *dates), make_dict_entry("ids", *ids),
		make_dict_entry("states", states), make_dict_entry("counts", *counts));
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
	auto states = std::vector<std::string>();
	auto ids = new std::vector<int>();
	auto counts = new std::vector<int>();

	self.get_today_virus(states, *ids, *counts);

	return make_dict(make_dict_entry("states", states),
					 make_dict_entry("ids", *ids),
					 make_dict_entry("counts", *counts));
}

static auto get_today_total(DataBase<int> &self) -> py::dict {
	auto counts = new std::vector<int>();
	auto states = std::vector<std::string>();

	self.get_today_total(&states, counts);

	return make_dict(make_dict_entry("states", states),
					 make_dict_entry("counts", *counts));
}

static auto get_active_cases(DataBase<int> &self) -> py::dict {
	auto dates = new std::vector<int>();
	auto virus_id = new std::vector<int>();
	auto counts = new std::vector<int>();

	self.get_active_cases(*dates, *virus_id, *counts);

	return make_dict(make_dict_entry("dates", *dates),
					 make_dict_entry("virus_id", *virus_id),
					 make_dict_entry("counts", *counts));
}

static auto get_outbreak_size(DataBase<int> &self) -> py::dict {
	auto dates = new std::vector<int>();
	auto virus_id = new std::vector<int>();
	auto size = new std::vector<int>();

	self.get_outbreak_size(*dates, *virus_id, *size);

	return make_dict(make_dict_entry("dates", *dates),
					 make_dict_entry("virus_id", *virus_id),
					 make_dict_entry("size", *size));
}

static auto get_hospitalizations(DataBase<int> &self) -> py::dict {
	auto dates = new std::vector<int>();
	auto virus_id = new std::vector<int>();
	auto tool_id = new std::vector<int>();
	auto count = new std::vector<int>();
	auto weight = new std::vector<double>();

	self.get_hospitalizations(*dates, *virus_id, *tool_id, *count, *weight);

	return make_dict(make_dict_entry("dates", *dates),
					 make_dict_entry("virus_id", *virus_id),
					 make_dict_entry("tool_id", *tool_id),
					 make_dict_entry("count", *count),
					 make_dict_entry("weight", *weight));
}

void epiworldpy::export_database(
	py::class_<DataBase<int>, std::shared_ptr<DataBase<int>>> &c) {
	c.def("add_user_data",
		  pybind11::detail::overload_cast_impl<std::vector<epiworld_double>>()(
			  &DataBase<int>::add_user_data),
		  doc::add_user_data_row, py::arg("x"))
		.def("add_user_data",
			 pybind11::detail::overload_cast_impl<epiworld_fast_uint,
												  epiworld_double>()(
				 &DataBase<int>::add_user_data),
			 doc::add_user_data_value, py::arg("j"), py::arg("x"))
		.def("get_n_tools", &DataBase<int>::get_n_tools,
			 doc::get_n_tools)
		.def("get_n_viruses", &DataBase<int>::get_n_viruses,
			 doc::get_n_viruses)
		.def("record_transmission", &DataBase<int>::record_transmission,
			 doc::record_transmission, py::arg("i"), py::arg("j"),
			 py::arg("virus"), py::arg("i_expo_date"))
		.def(
			"write_data",
			[](const DataBase<int> &db, std::string fn_virus_info,
			   std::string fn_virus_hist, std::string fn_tool_info,
			   std::string fn_tool_hist, std::string fn_total_hist,
			   std::string fn_transmission, std::string fn_transition,
			   std::string fn_reproductive_number,
			   std::string fn_generation_time, std::string fn_active_cases,
			   std::string fn_outbreak_size, std::string fn_hospitalizations) {
				db.write_data(fn_virus_info, fn_virus_hist, fn_tool_info,
							  fn_tool_hist, fn_total_hist, fn_transmission,
							  fn_transition, fn_reproductive_number,
							  fn_generation_time, fn_active_cases,
							  fn_outbreak_size, fn_hospitalizations);
			},
			doc::write_data, py::arg("fn_virus_info"),
			py::arg("fn_virus_hist"), py::arg("fn_tool_info"),
			py::arg("fn_tool_hist"), py::arg("fn_total_hist"),
			py::arg("fn_transmission"), py::arg("fn_transition"),
			py::arg("fn_reproductive_number"), py::arg("fn_generation_time"),
			py::arg("fn_active_cases") = std::string(""),
			py::arg("fn_outbreak_size") = std::string(""),
			py::arg("fn_hospitalizations") = std::string(""))
		.def("get_hist_virus", &get_hist_virus, doc::get_hist_virus)
		.def("get_hist_tool", &get_hist_tool, doc::get_hist_tool)
		.def("get_today_transition_matrix", &get_today_transition_matrix,
			 doc::get_today_transition_matrix)
		.def("get_today_virus", &get_today_virus, doc::get_today_virus)
		.def("get_today_total", &get_today_total, doc::get_today_total)
		.def("size", &DataBase<int>::size, doc::size)
		.def("record", &DataBase<int>::record, doc::record)
		.def("reset", &DataBase<int>::reset, doc::reset)
		.def("record_tool", &DataBase<int>::record_tool,
			 doc::record_tool, py::arg("t"))
		.def("record_virus", &DataBase<int>::record_virus,
			 doc::record_virus, py::arg("v"))
		.def("get_hist_total", &get_hist_total,
			 doc::get_hist_total)
		.def("get_reproductive_number", &get_reproductive_number,
			 doc::get_reproductive_number)
		.def("get_transmissions", &get_transmissions,
			 doc::get_transmissions)
		.def("get_generation_time", &get_generation_time,
			 doc::get_generation_time)
		.def("get_hist_transition_matrix", &get_hist_transition_matrix,
			 doc::get_hist_transition_matrix,
			 py::arg("skip_zeros") = false)
		.def("get_active_cases", &get_active_cases,
			 doc::get_active_cases)
		.def("get_outbreak_size", &get_outbreak_size,
			 doc::get_outbreak_size)
		.def("get_hospitalizations", &get_hospitalizations,
			 doc::get_hospitalizations)
		.def("get_transition_probability",
			 &DataBase<int>::get_transition_probability,
			 doc::get_transition_probability,
			 py::arg("print") = false, py::arg("normalize") = true);
}
