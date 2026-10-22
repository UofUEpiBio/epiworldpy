#ifndef EPIWORLDPY_DOCSTRINGS_LAYER_HPP
#define EPIWORLDPY_DOCSTRINGS_LAYER_HPP

// NumPy-style docstrings for epiworldpy.graph and epiworldpy.layer (see
// tests/test_docstrings.py).

// clang-format off
namespace epiworldpy::docstrings::layer {

// -- epiworldpy.graph ---------------------------------------------------------

inline constexpr const char *graph_module =
	R"doc(Networks for contact layers: generators, fixed graphs, and rewiring.

A generator (:func:`smallworld`, :func:`bernoulli`, :func:`sbm`) draws a new
network at the start of every run and replicate; a fixed graph
(:func:`edgelist`) gives the same ties every time. Either goes to
:func:`epiworldpy.layer.network`.
)doc";

inline constexpr const char *generator_cls =
	R"doc(A network generator, from :func:`smallworld`, :func:`bernoulli`, or :func:`sbm`.

Pass it to :func:`epiworldpy.layer.network`; the layer draws a new network
with it at the start of every run.
)doc";

inline constexpr const char *graph_cls =
	R"doc(A fixed network, from :func:`edgelist`.

Pass it to :func:`epiworldpy.layer.network`; the layer starts every run with
its ties.
)doc";

inline constexpr const char *rewiring_cls =
	R"doc(A rewiring function, from :func:`rewire_degseq`.

Give it to a network layer with :meth:`epiworldpy.layer.Network.set_rewiring`.
)doc";

inline constexpr const char *smallworld =
	R"doc(Watts-Strogatz small-world networks.

Agents are placed on a ring and tied to their ``k`` nearest neighbors; then a
proportion ``p`` of the ties are rewired at random.

Parameters
----------
k : int
    Number of neighbors of each agent in the ring.
p : float
    Probability of rewiring each tie.
directed : bool, default False
    Whether the network is directed.

Returns
-------
Generator
    A generator that draws a new network for every run.
)doc";

inline constexpr const char *bernoulli =
	R"doc(Bernoulli (Erdos-Renyi) random networks.

Parameters
----------
p : float
    Probability of each tie.
directed : bool, default False
    Whether the network is directed.

Returns
-------
Generator
    A generator that draws a new network for every run.
)doc";

inline constexpr const char *sbm =
	R"doc(Stochastic block model networks.

Parameters
----------
block_sizes : list of int
    Number of agents in each block; they must add up to the population size.
mixing_matrix : list of float
    Probability of a tie between (and within) blocks, a square matrix
    flattened by rows (or by columns, see ``row_major``).
row_major : bool, default True
    Whether ``mixing_matrix`` is flattened by rows.

Returns
-------
Generator
    A generator that draws a new network for every run.
)doc";

inline constexpr const char *edgelist = R"doc(A fixed network from an edge list.

Parameters
----------
source : list of int
    Source of each tie (agent ids start at 0).
target : list of int
    Target of each tie.
size : int
    Number of agents in the network (must match the model's population).
directed : bool, default False
    Whether the network is directed. A directed tie ``source -> target`` is
    kept by the source only, so ``target`` can infect ``source``, not the
    other way around.

Returns
-------
Graph
    The network.
)doc";

inline constexpr const char *rewire_degseq =
	R"doc(Rewiring that keeps every agent's degree.

At the end of each step, a proportion of the ties (read from a model
parameter) swap ends, preserving the degree sequence.

Parameters
----------
param : str
    Name of the model parameter holding the proportion of ties rewired.

Returns
-------
Rewiring
    The rewiring function.
)doc";

// -- epiworldpy.layer ---------------------------------------------------------

inline constexpr const char *layer_module =
	R"doc(Contact layers: where agents meet.

A model draws the contacts of its agents from its layers, all of them in one
draw. A network layer (:class:`Network`, from :func:`network`) holds ties
between agents; a mixing layer (:class:`Mixing`, created by the mixing
models) samples contacts between groups with a contact matrix. Network models
start with no layers: add one with :meth:`epiworldpy.Model.add_layer`.

A layer object is the model's layer once added: editing it (for example,
with :meth:`Network.add_edge`) edits the model. An edit made during a run
lasts for that run; an edit made between runs is kept.

Examples
--------
>>> from epiworldpy import epimodels, graph, layer
>>> sir = epimodels.ModelSIR("flu", n=1000, prevalence=0.01,
...                          transmission_rate=0.3, recovery_rate=0.1)
>>> contacts = layer.network("contacts", graph.smallworld(k=5, p=0.01))
>>> sir.add_layer(contacts)
>>> sir.run(ndays=50, seed=1912)
)doc";

inline constexpr const char *contact_layer_cls =
	R"doc(Base class of every contact layer.

The connected models (for example,
:class:`~epiworldpy.epimodels.ModelSIRCONN`) draw contacts from the list of
infected agents and ignore layers for now.
)doc";

inline constexpr const char *get_name = R"doc(Get the layer's name.

Returns
-------
str
    Name of the layer, unique within a model.
)doc";

inline constexpr const char *get_id = R"doc(Get the layer's id in its model.

Layers are numbered from 0 in the order they were added; the ``layers``
column of :meth:`epiworldpy.DataBase.get_transmissions` uses these ids.

Returns
-------
int
    Id of the layer.
)doc";

inline constexpr const char *set_weight =
	R"doc(Scale the odds of infection through the layer by a model parameter.

A weight of 0 turns the layer off, and 1 changes nothing; a layer without a
weight has weight 1.

Parameters
----------
param : str
    Name of the model parameter holding the weight (add it with
    :meth:`epiworldpy.Model.add_param`).
)doc";

inline constexpr const char *add_virus_target_id =
	R"doc(Restrict the layer to a virus lineage.

A layer without targets spreads every virus; after the first target, only
the targeted lineages spread through it.

Parameters
----------
lineage_id : int
    Lineage id of the virus (see :meth:`epiworldpy.Virus.get_lineage_id`).
)doc";

inline constexpr const char *add_virus_target =
	R"doc(Restrict the layer to a virus (and its variants).

A layer without targets spreads every virus; after the first target, only
the targeted lineages spread through it.

Parameters
----------
virus : Virus
    A virus already added to the model.
)doc";

inline constexpr const char *network_cls = R"doc(A network layer: ties between agents.

Created with :func:`network`.
)doc";

inline constexpr const char *size = R"doc(Get the number of agents in the network.

Returns
-------
int
    Number of agents.
)doc";

inline constexpr const char *is_directed = R"doc(Whether the network is directed.

Returns
-------
bool
    True if the network is directed.
)doc";

inline constexpr const char *get_degree = R"doc(Get an agent's number of ties.

Parameters
----------
i : int
    Id of the agent.

Returns
-------
int
    Number of ties of agent ``i`` in this layer.
)doc";

inline constexpr const char *add_edge = R"doc(Add a tie between two agents.

Parameters
----------
i : int
    Id of the first agent.
j : int
    Id of the second agent.

Returns
-------
bool
    Whether the network changed (False if the tie already existed).
)doc";

inline constexpr const char *rm_edge = R"doc(Remove the tie between two agents.

Parameters
----------
i : int
    Id of the first agent.
j : int
    Id of the second agent.

Returns
-------
bool
    Whether the network changed (False if there was no tie).
)doc";

inline constexpr const char *has_edge = R"doc(Whether two agents are tied.

Parameters
----------
i : int
    Id of the first agent.
j : int
    Id of the second agent.

Returns
-------
bool
    True if the tie exists.
)doc";

inline constexpr const char *get_edgelist = R"doc(Get the ties of the network.

An undirected tie is listed once.

Returns
-------
dict
    ``from`` and ``to``: arrays with the ends of each tie.
)doc";

inline constexpr const char *set_rewiring =
	R"doc(Rewire the network at the end of every step.

Parameters
----------
rewiring : Rewiring
    A rewiring function, from :func:`epiworldpy.graph.rewire_degseq`.
)doc";

inline constexpr const char *mixing_cls =
	R"doc(A mixing layer: contacts between groups, from a contact matrix.

The mixing models (for example,
:class:`~epiworldpy.epimodels.ModelSIRMixing`) create one named ``"mixing"``;
retrieve it with ``model.get_layer("mixing")``.
)doc";

inline constexpr const char *set_contact_matrix =
	R"doc(Set the contact matrix.

Parameters
----------
contact_matrix : list of float
    Expected number of contacts between groups, a square matrix flattened by
    columns.
)doc";

inline constexpr const char *get_contact_matrix = R"doc(Get the contact matrix.

Returns
-------
list of float
    The contact matrix, flattened by columns.
)doc";

inline constexpr const char *network =
	R"doc(Create a network layer drawn by a generator.

Parameters
----------
name : str
    Name of the layer (unique within a model).
graph : Generator
    A network generator, from :mod:`epiworldpy.graph`; the layer draws a new
    network at the start of every run.

Returns
-------
Network
    The layer, to add with :meth:`epiworldpy.Model.add_layer`.
)doc";

inline constexpr const char *network_graph =
	R"doc(Create a network layer with fixed ties.

Parameters
----------
name : str
    Name of the layer (unique within a model).
graph : Graph
    A fixed network, from :func:`epiworldpy.graph.edgelist`.

Returns
-------
Network
    The layer, to add with :meth:`epiworldpy.Model.add_layer`.
)doc";

} // namespace epiworldpy::docstrings::layer
// clang-format on

#endif /* EPIWORLDPY_DOCSTRINGS_LAYER_HPP */
