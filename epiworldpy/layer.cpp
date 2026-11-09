#include "layer.hpp"
#include "docstrings/layer.hpp"
#include "natural_history.hpp"

#include <pybind11/functional.h>
#include <pybind11/stl.h>

#include <cmath>
#include <optional>
#include <variant>

using namespace epiworldpy;
using namespace epiworld;
namespace py = pybind11;
namespace doc = epiworldpy::docstrings::layer;

using Network = layer::Network;
using Mixing = layer::Mixing;
using Generator = Network::Generator;
using ContactProb = std::variant<double, std::vector<double>>;
using Rewiring = Network::Rewiring;

/* Python objects that only carry a generator or a rewiring function from the
 * graph functions to the layer; they wrap the std::function so Python does not
 * convert it into a callable. */
struct GraphGenerator {
	Generator fun;
};

struct GraphRewiring {
	Rewiring fun;
};

/* Sets the contacts of a mixing layer: a contact matrix or contact
 * probabilities (the caller checks there is exactly one). */
static void set_contacts(Mixing &mix, std::optional<std::vector<double>> &cm,
						 std::optional<ContactProb> &cp) {
	if (cm)
		mix.set_contact_matrix(std::move(*cm));
	else
		std::visit([&](auto p) { mix.set_contact_prob(p); }, *cp);
}

void epiworldpy::export_graph(py::module &m) {
	py::class_<GraphGenerator>(m, "Generator", doc::generator_cls);
	py::class_<AdjList>(m, "Graph", doc::graph_cls);
	py::class_<GraphRewiring>(m, "Rewiring", doc::rewiring_cls);

	m.def(
		 "smallworld",
		 [](epiworld_fast_uint k, epiworld_double p, bool directed) {
			 return GraphGenerator{graph::smallworld(k, p, directed)};
		 },
		 doc::smallworld, py::arg("k"), py::arg("p"),
		 py::arg("directed") = false)
		.def(
			"bernoulli",
			[](epiworld_double p, bool directed) {
				return GraphGenerator{graph::bernoulli(p, directed)};
			},
			doc::bernoulli, py::arg("p"), py::arg("directed") = false)
		.def(
			"sbm",
			[](std::vector<size_t> block_sizes,
			   std::vector<double> mixing_matrix, bool row_major) {
				return GraphGenerator{graph::sbm(std::move(block_sizes),
													  std::move(mixing_matrix),
													  row_major)};
			},
			doc::sbm, py::arg("block_sizes"), py::arg("mixing_matrix"),
			py::arg("row_major") = true)
		.def(
			"edgelist",
			[](const std::vector<int> &source, const std::vector<int> &target,
			   int size, bool directed) {
				return AdjList(source, target, size, directed);
			},
			doc::edgelist, py::arg("source"), py::arg("target"),
			py::arg("size"), py::arg("directed") = false)
		.def(
			"rewire_degseq",
			[](std::string param) {
				return GraphRewiring{
					graph::rewire_degseq(std::move(param))};
			},
			doc::rewire_degseq, py::arg("param"));
}

void epiworldpy::export_layer(py::module &m) {
	py::class_<ContactLayer, std::shared_ptr<ContactLayer>>(
		m, "ContactLayer", doc::contact_layer_cls)
		.def("get_name", &ContactLayer::get_name, doc::get_name)
		.def("get_id", &ContactLayer::get_id, doc::get_id)
		.def("set_weight", &ContactLayer::set_weight, doc::set_weight,
			 py::arg("param"))
		.def(
			"add_virus_target",
			[](ContactLayer &self, int lineage_id) {
				self.add_virus_target(lineage_id);
			},
			doc::add_virus_target_id, py::arg("lineage_id"))
		.def(
			"add_virus_target",
			[](ContactLayer &self, const Virus &virus) {
				self.add_virus_target(virus);
			},
			doc::add_virus_target, py::arg("virus"))
		.def("enable_tracing", &ContactLayer::enable_tracing,
			 doc::enable_tracing, py::arg("max_contacts") = EPI_MAX_TRACKING)
		.def("disable_tracing", &ContactLayer::disable_tracing,
			 doc::disable_tracing)
		.def("is_tracing", &ContactLayer::is_tracing, doc::is_tracing)
		.def(
			"get_tracing",
			[](ContactLayer &self) -> py::dict {
				ContactTracing &log = self.get_tracing();
				auto source = new std::vector<int>();
				auto contact = new std::vector<int>();
				auto day = new std::vector<int>();
				for (size_t a = 0u; a < log.get_n_agents(); ++a) {
					const size_t n = std::min(log.get_n_contacts(a),
											  log.get_max_contacts());
					for (size_t k = 0u; k < n; ++k) {
						auto [c, d] = log.get_contact(a, k);
						source->push_back(static_cast<int>(a));
						contact->push_back(static_cast<int>(c));
						day->push_back(d);
					}
				}
				return make_dict(make_dict_entry("source", *source),
								 make_dict_entry("contact", *contact),
								 make_dict_entry("day", *day));
			},
			doc::get_tracing);

	py::class_<Network, ContactLayer, std::shared_ptr<Network>>(
		m, "Network", doc::network_cls)
		.def("size", &Network::size, doc::size)
		.def("is_directed", &Network::is_directed, doc::is_directed)
		.def("get_degree", &Network::get_degree, doc::get_degree, py::arg("i"))
		.def("add_edge", &Network::add_edge, doc::add_edge, py::arg("i"),
			 py::arg("j"))
		.def("rm_edge", &Network::rm_edge, doc::rm_edge, py::arg("i"),
			 py::arg("j"))
		.def("has_edge", &Network::has_edge, doc::has_edge, py::arg("i"),
			 py::arg("j"))
		.def(
			"get_edgelist",
			[](const Network &self) -> py::dict {
				auto source = new std::vector<int>();
				auto target = new std::vector<int>();
				self.get_edgelist(*source, *target);
				return make_dict(make_dict_entry("from", *source),
								 make_dict_entry("to", *target));
			},
			doc::get_edgelist)
		.def(
			"set_rewiring",
			[](Network &self, const GraphRewiring &rewiring) {
				self.set_rewiring(rewiring.fun);
			},
			doc::set_rewiring, py::arg("rewiring"));

	py::class_<Mixing, ContactLayer, std::shared_ptr<Mixing>>(
		m, "Mixing", doc::mixing_cls)
		.def("set_contact_matrix", &Mixing::set_contact_matrix,
			 doc::set_contact_matrix, py::arg("contact_matrix"))
		.def("set_contact_rate", &Mixing::set_contact_rate,
			 doc::set_contact_rate, py::arg("param"))
		.def("get_contact_matrix", &Mixing::get_contact_matrix,
			 doc::get_contact_matrix)
		.def("set_contact_prob",
			 py::overload_cast<double>(&Mixing::set_contact_prob),
			 doc::set_contact_prob, py::arg("contact_prob"))
		.def("set_contact_prob",
			 py::overload_cast<std::vector<double>>(&Mixing::set_contact_prob),
			 doc::set_contact_prob_list, py::arg("contact_prob"))
		.def("get_contact_prob", &Mixing::get_contact_prob,
			 doc::get_contact_prob)
		.def("get_n_groups", &Mixing::get_n_groups, doc::get_n_groups)
		.def(
			"get_group",
			[](const Mixing &self, size_t agent) -> std::optional<size_t> {
				size_t g = self.get_group(agent);
				if (g == Mixing::NO_GROUP)
					return std::nullopt;
				return g;
			},
			doc::get_group, py::arg("agent"))
		.def(
			"set_group",
			[](Mixing &self, size_t agent, std::optional<size_t> group) {
				self.set_group(agent, group.value_or(Mixing::NO_GROUP));
			},
			doc::set_group, py::arg("agent"), py::arg("group"))
		.def("members_of", &Mixing::members_of, doc::members_of,
			 py::arg("group"))
		.def(
			"groupmates_of",
			[](const Mixing &self, size_t agent, int start, int end) -> py::dict {
				std::vector<Mixing::Groupmate> mates;
				self.groupmates_of(agent, start, end, mates);
				auto id = new std::vector<int>();
				auto last_day = new std::vector<int>();
				auto n_days = new std::vector<int>();
				for (const auto &g : mates) {
					id->push_back(static_cast<int>(g.agent));
					last_day->push_back(g.last_day);
					n_days->push_back(g.n_days);
				}
				return make_dict(make_dict_entry("agent", *id),
								 make_dict_entry("last_day", *last_day),
								 make_dict_entry("n_days", *n_days));
			},
			doc::groupmates_of, py::arg("agent"), py::arg("start") = 1,
			py::arg("end") = std::numeric_limits<int>::max());

	m.def(
		 "network",
		 [](std::string name, const GraphGenerator &generator) {
			 return layer::network(std::move(name), generator.fun);
		 },
		 doc::network, py::arg("name"), py::arg("graph"))
		.def(
			"network",
			[](std::string name, const AdjList &graph) {
				return layer::network(std::move(name), graph);
			},
			doc::network_graph, py::arg("name"), py::arg("graph"))
		.def(
			"mixing",
			[](std::string name, const py::object &groups,
			   std::optional<std::vector<double>> contact_matrix,
			   std::optional<ContactProb> contact_prob) {
				if (contact_matrix.has_value() == contact_prob.has_value())
					throw std::invalid_argument(
						"Give exactly one of contact_matrix and contact_prob.");

				// Groups: one per agent (None: out of the layer), or a
				// placement whose labels are the groups
				PyAssign a;
				if (py::isinstance<PyAssign>(groups))
					a = groups.cast<PyAssign>();
				else {
					a.form = "labels";
					std::vector<std::optional<int>> gs;
					try {
						gs = groups.cast<std::vector<std::optional<int>>>();
					} catch (const py::cast_error &) {
						throw py::type_error(
							"groups are a list of whole numbers (None for no "
							"group) or a placement (e.g., assign.counts(sizes)).");
					}
					for (auto g : gs) {
						if (g && (*g < 0))
							throw std::invalid_argument(
								"groups are non-negative integers (None for no "
								"group) or a placement (e.g., "
								"assign.counts(sizes)).");
						a.labels.push_back(g ? py::object(py::int_(*g))
											 : py::object(py::none()));
					}
				}

				// The number of groups: the matrix's rows, one probability
				// per group, or the placement's labels
				const auto *q = contact_prob
									? std::get_if<std::vector<double>>(&*contact_prob)
									: nullptr;
				size_t n_groups = 0;
				if (contact_matrix) {
					n_groups = static_cast<size_t>(
						std::lround(std::sqrt(double(contact_matrix->size()))));
					if (n_groups * n_groups != contact_matrix->size())
						throw std::invalid_argument(
							"contact_matrix must be a square matrix (flattened).");
				} else if (q && (q->size() > 1))
					n_groups = q->size();
				else if ((a.form == "shares") || (a.form == "counts"))
					n_groups = a.values.size();
				else if (a.form == "labels") {
					for (const auto &l : a.labels)
						if (!l.is_none())
							n_groups = std::max(n_groups, l.cast<size_t>() + 1);
				} else
					throw std::invalid_argument(
						"With this placement, give one contact_prob per group "
						"(or a contact_matrix), so the layer knows how many "
						"groups there are.");

				auto mix = layer::mixing(std::move(name));
				mix->set_groups(assign_from_py(&a, LabelOf()), n_groups);
				set_contacts(*mix, contact_matrix, contact_prob);
				return mix;
			},
			doc::mixing, py::arg("name"), py::arg("groups"),
			py::arg("contact_matrix") = py::none(),
			py::arg("contact_prob") = py::none());
}
