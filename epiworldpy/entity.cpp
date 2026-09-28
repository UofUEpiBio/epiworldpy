#include "entity.hpp"
#include "docstrings/entity.hpp"

#include <pybind11/functional.h>
#include <pybind11/numpy.h>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

#include <utility>

using namespace epiworld;
using namespace epiworldpy;
using namespace pybind11::literals;
namespace py = pybind11;
namespace doc = epiworldpy::docstrings::entity;

static auto new_entity(std::string name, epiworld::EntityToAgentFun<int> fun)
	-> epiworld::Entity<int> {
	Entity<int> entity(std::move(name), std::move(fun));

	return entity;
}

static auto new_entity_to_agent_fun(
	const std::function<void(Entity<int> &, Model<int> *)> &fun)
	-> epiworld::EntityToAgentFun<int> {
	return {fun};
}

static auto entity_distribute_randomly(double prevalence, bool as_proportion,
									   bool to_unassigned)
	-> epiworld::EntityToAgentFun<int> {
	return distribute_entity_randomly<int>(prevalence, as_proportion,
										   to_unassigned);
}

static auto entity_distribute_to_range(size_t from, size_t to)
	-> epiworld::EntityToAgentFun<int> {
	return distribute_entity_to_range<int>(from, to);
}

static auto entity_distribute_to_set(std::vector<size_t> ids)
	-> epiworld::EntityToAgentFun<int> {
	return distribute_entity_to_set<int>(ids);
}

void epiworldpy::export_entity(
	pybind11::class_<epiworld::Entity<int>,
					 std::shared_ptr<epiworld::Entity<int>>> &c) {
	c.def(py::init(&new_entity), doc::init, py::arg("name"),
		  py::arg("fun") = nullptr)
		.def("get_id", &Entity<int>::get_id, doc::get_id)
		.def("get_name", &Entity<int>::get_name, doc::get_name)
		.def("size", &Entity<int>::size,
			 doc::size)
		.def("get_agents_ids", &Entity<int>::get_agents_ids,
			 doc::get_agents_ids)
		.def("set_location", &Entity<int>::set_location,
			 doc::set_location, py::arg("location"))
		.def("get_location", &Entity<int>::get_location,
			 py::return_value_policy::reference_internal,
			 doc::get_location)
		.def("set_state", &Entity<int>::set_state,
			 doc::set_state, py::arg("init"),
			 py::arg("post"))
		.def("set_queue", &Entity<int>::set_queue,
			 doc::set_queue, py::arg("init"),
			 py::arg("post"))
		.def("set_distribution", &Entity<int>::set_distribution,
			 doc::set_distribution, py::arg("fun"))
		.def("distribute", &Entity<int>::distribute,
			 doc::distribute, py::arg("model"))
		.def("print", &Entity<int>::print,
			 doc::print)
		.def_static("new_entity_to_agent_fun", &new_entity_to_agent_fun,
					doc::new_entity_to_agent_fun,
					py::arg("fun"))
		.def_static("distribute_randomly", &entity_distribute_randomly,
					doc::distribute_randomly,
					py::arg("prevalence"), py::arg("as_proportion") = true,
					py::arg("to_unassigned") = false)
		.def_static("distribute_to_range", &entity_distribute_to_range,
					doc::distribute_to_range,
					py::arg("from_"), py::arg("to_"))
		.def_static("distribute_to_set", &entity_distribute_to_set,
					doc::distribute_to_set,
					py::arg("ids"));
}
