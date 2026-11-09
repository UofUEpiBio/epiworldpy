#include "bubbles.hpp"
#include "docstrings/bubbles.hpp"

#include <pybind11/stl.h>

using namespace epiworld;
namespace py = pybind11;
namespace doc = epiworldpy::docstrings::bubbles;

/* The C++ enums are given as strings, as in epiworldR's bubbles(). */
static BubbleFlavor to_flavor(const std::string &flavor) {
	if (flavor == "household")
		return BubbleFlavor::Household;
	if (flavor == "peer")
		return BubbleFlavor::Peer;
	throw std::invalid_argument(
		"flavor must be \"household\" or \"peer\" (got \"" + flavor + "\").");
}

static BubbleTies to_ties(const std::string &ties) {
	if (ties == "existing")
		return BubbleTies::Existing;
	if (ties == "complete")
		return BubbleTies::Complete;
	throw std::invalid_argument(
		"ties must be \"existing\" or \"complete\" (got \"" + ties + "\").");
}

void epiworldpy::export_bubbles(py::class_<Bubbles> &c) {
	c.def(py::init([](std::vector<size_t> household_id,
					  const std::string &flavor, size_t group_size,
					  epiworld_double transmission_factor, int start_day,
					  int end_day, int rewire_every, std::string name,
					  size_t max_households, std::string param_name,
					  const std::string &ties, std::string layer,
					  double bubble_contact_prob) {
			  return Bubbles(
				  std::move(household_id), to_flavor(flavor), group_size,
				  transmission_factor, start_day, end_day, rewire_every,
				  std::move(name), max_households, std::move(param_name),
				  to_ties(ties), std::move(layer), bubble_contact_prob);
		  }),
		  doc::init, py::arg("household_id"), py::arg("flavor"),
		  py::arg("group_size"), py::arg("transmission_factor") = 0.0,
		  py::arg("start_day") = 0, py::arg("end_day") = -1,
		  py::arg("rewire_every") = 0, py::arg("name") = "Social bubble",
		  py::arg("max_households") = 2u,
		  py::arg("param_name") = "Bubble transmission factor",
		  py::arg("ties") = "existing", py::arg("layer") = "",
		  py::arg("bubble_contact_prob") = 1.0)
		.def(
			"set_ties",
			[](Bubbles &self, const std::string &ties, std::string layer,
			   double bubble_contact_prob) {
				self.set_ties(to_ties(ties), std::move(layer),
							  bubble_contact_prob);
			},
			doc::set_ties, py::arg("ties"), py::arg("layer") = "",
			py::arg("bubble_contact_prob") = 1.0)
		.def("make_layer", &Bubbles::make_layer, doc::make_layer,
			 py::arg("household_contact_prob") = 1.0)
		.def_static("get_from", &Bubbles::get_from, doc::get_from,
					py::arg("model"), py::arg("name") = "Social bubble",
					py::return_value_policy::reference, py::keep_alive<0, 1>())
		.def("get_name", &Bubbles::get_name, doc::get_name)
		.def("get_bubble_id", &Bubbles::get_bubble_id, doc::get_bubble_id)
		.def(
			"get_flavor",
			[](const Bubbles &self) {
				return self.get_flavor() == BubbleFlavor::Peer ? "peer"
															   : "household";
			},
			doc::get_flavor)
		.def(
			"get_ties",
			[](const Bubbles &self) {
				return self.get_ties() == BubbleTies::Complete ? "complete"
															   : "existing";
			},
			doc::get_ties)
		.def("get_param_name", &Bubbles::get_param_name,
			 doc::get_param_name)
		.def("is_active", &Bubbles::is_active, doc::is_active,
			 py::arg("today"))
		.def("get_last_epoch", &Bubbles::get_last_epoch,
			 doc::get_last_epoch);
}
