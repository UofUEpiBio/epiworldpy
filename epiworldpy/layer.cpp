#include "layer.hpp"
#include "docstrings/layer.hpp"

#include <pybind11/functional.h>
#include <pybind11/stl.h>

using namespace epiworldpy;
using namespace epiworld;
namespace py = pybind11;
namespace doc = epiworldpy::docstrings::layer;

using Network = layer::Network<int>;
using Mixing = layer::Mixing<int>;
using Generator = Network::Generator;
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

void epiworldpy::export_graph(py::module &m) {
	py::class_<GraphGenerator>(m, "Generator", doc::generator_cls);
	py::class_<AdjList>(m, "Graph", doc::graph_cls);
	py::class_<GraphRewiring>(m, "Rewiring", doc::rewiring_cls);

	m.def(
		 "smallworld",
		 [](epiworld_fast_uint k, epiworld_double p, bool directed) {
			 return GraphGenerator{graph::smallworld<int>(k, p, directed)};
		 },
		 doc::smallworld, py::arg("k"), py::arg("p"),
		 py::arg("directed") = false)
		.def(
			"bernoulli",
			[](epiworld_double p, bool directed) {
				return GraphGenerator{graph::bernoulli<int>(p, directed)};
			},
			doc::bernoulli, py::arg("p"), py::arg("directed") = false)
		.def(
			"sbm",
			[](std::vector<size_t> block_sizes,
			   std::vector<double> mixing_matrix, bool row_major) {
				return GraphGenerator{graph::sbm<int>(std::move(block_sizes),
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
					graph::rewire_degseq<int>(std::move(param))};
			},
			doc::rewire_degseq, py::arg("param"));
}

void epiworldpy::export_layer(py::module &m) {
	py::class_<ContactLayer<int>, std::shared_ptr<ContactLayer<int>>>(
		m, "ContactLayer", doc::contact_layer_cls)
		.def("get_name", &ContactLayer<int>::get_name, doc::get_name)
		.def("get_id", &ContactLayer<int>::get_id, doc::get_id)
		.def("set_weight", &ContactLayer<int>::set_weight, doc::set_weight,
			 py::arg("param"))
		.def(
			"add_virus_target",
			[](ContactLayer<int> &self, int lineage_id) {
				self.add_virus_target(lineage_id);
			},
			doc::add_virus_target_id, py::arg("lineage_id"))
		.def(
			"add_virus_target",
			[](ContactLayer<int> &self, const Virus<int> &virus) {
				self.add_virus_target(virus);
			},
			doc::add_virus_target, py::arg("virus"));

	py::class_<Network, ContactLayer<int>, std::shared_ptr<Network>>(
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

	py::class_<Mixing, ContactLayer<int>, std::shared_ptr<Mixing>>(
		m, "Mixing", doc::mixing_cls)
		.def("set_contact_matrix", &Mixing::set_contact_matrix,
			 doc::set_contact_matrix, py::arg("contact_matrix"))
		.def("get_contact_matrix", &Mixing::get_contact_matrix,
			 doc::get_contact_matrix);

	m.def(
		 "network",
		 [](std::string name, const GraphGenerator &generator) {
			 return layer::network<int>(std::move(name), generator.fun);
		 },
		 doc::network, py::arg("name"), py::arg("graph"))
		.def(
			"network",
			[](std::string name, const AdjList &graph) {
				return layer::network<int>(std::move(name), graph);
			},
			doc::network_graph, py::arg("name"), py::arg("graph"));
}
