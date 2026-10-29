#ifndef EPIWORLDPY_DOCSTRINGS_LAYER_HPP
#define EPIWORLDPY_DOCSTRINGS_LAYER_HPP

// NumPy-style docstrings for epiworldpy.graph and epiworldpy.layer (see
// tests/test_docstrings.py).

// clang-format off
namespace epiworldpy::docstrings::layer {

// -- epiworldpy.grouping ------------------------------------------------------

inline constexpr const char *grouping_module =
	R"doc(Groupings for mixing layers: groups drawn afresh at the start of every run.

A grouping (:func:`random`) goes to :func:`epiworldpy.layer.mixing` in place of
fixed groups, as a network generator goes to :func:`epiworldpy.layer.network`.
)doc";

inline constexpr const char *grouping_cls =
	R"doc(A grouping, from :func:`random`.

Pass it to :func:`epiworldpy.layer.mixing` as the groups; the layer draws them
with it at the start of every run.
)doc";

inline constexpr const char *grouping_random =
	R"doc(Random groups of given sizes.

Group by group, in order, the agents are drawn at random from those not yet in
a group; agents left over are in no group. With ``as_proportion``, a group of
size ``s`` takes ``floor(s * n)`` of the ``n`` agents still without a group.

Parameters
----------
sizes : list of float
    One size per group: a whole number of agents, or a proportion in [0, 1].
as_proportion : bool, default False
    Whether the sizes are proportions of the agents not yet in a group.

Returns
-------
Grouping
    A grouping that draws new groups for every run.
)doc";

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
between agents; a mixing layer (:class:`Mixing`, from :func:`mixing`) draws
contacts by group, fixed or drawn at every run by a grouping
(:mod:`epiworldpy.grouping`). Models start with no layers: add one with
:meth:`epiworldpy.Model.add_layer`.

A layer object is the model's layer once added: editing it (for example,
with :meth:`Network.add_edge`) edits the model. An edit made during a run
lasts for that run; an edit made between runs is kept.

Examples
--------
>>> from epiworldpy import epimodels, graph, layer
>>> sir = epimodels.ModelSIR("flu", n=1000, prevalence=0.01,
...                          transmission_prob=0.3, recovery_rate=0.1)
>>> contacts = layer.network("contacts", graph.smallworld(k=5, p=0.01))
>>> sir.add_layer(contacts)
>>> sir.run(ndays=50, seed=1912)
)doc";

inline constexpr const char *contact_layer_cls =
	R"doc(Base class of every contact layer.
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

inline constexpr const char *enable_tracing =
	R"doc(Log the contacts the layer samples.

Each contact of an infectious agent (the source) with another agent is logged
with the day. The log starts empty at every run and keeps the last
``max_contacts`` contacts of each source. Tracing is off by default; once
turned on (or off) it lasts across runs.

Parameters
----------
max_contacts : int
    Number of contacts the log keeps per agent (at least 1).
)doc";

inline constexpr const char *disable_tracing =
	R"doc(Stop logging contacts and drop the log.
)doc";

inline constexpr const char *is_tracing =
	R"doc(Whether the layer logs its contacts.

Returns
-------
bool
)doc";

inline constexpr const char *get_tracing =
	R"doc(Get the contact log of the last run.

Raises an error if the layer is not traced (see :meth:`enable_tracing`).

Returns
-------
dict
    ``source``, ``contact`` and ``day``: arrays with one entry per logged
    contact.
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
	R"doc(A mixing layer: contacts drawn afresh every day, by group.

The contacts come from a contact matrix (contacts per day across groups) or
from contact probabilities (within groups only: the probability that two
members of a group meet on a given day). Create one with :func:`mixing` and
add it to any model with :meth:`~epiworldpy.Model.add_layer`. Every agent of
a group can be met, and the agents in the model's infectious states transmit
(see :meth:`~epiworldpy.Model.set_infectious_states`).

With :meth:`set_contact_rate`, everyone in one group and a 1 x 1 contact
matrix of 1, every agent meets "Contact rate" others a day, at random (what
the former connected models, e.g. ``ModelSIRCONN``, did).
)doc";

inline constexpr const char *set_contact_rate =
	R"doc(Make the contacts a model parameter times the contact matrix.

The parameter is read at the end of every step (after the global events), so
a change to it applies from the next step. For a layer with a contact matrix
only; a layer with contact probabilities raises when the model runs.

Parameters
----------
param : str
    Name of the model parameter (add it with :meth:`Model.add_param`).
)doc";

inline constexpr const char *set_contact_prob =
	R"doc(Set one contact probability for every group.

Parameters
----------
contact_prob : float
    Probability, in [0, 1], that two members of a group meet on a given day;
    1 makes every group a clique.
)doc";

inline constexpr const char *set_contact_prob_list =
	R"doc(Set the contact probability of each group.

Parameters
----------
contact_prob : list of float
    Probability, in [0, 1], that two members of each group meet on a given
    day (one per group).
)doc";

inline constexpr const char *get_contact_prob =
	R"doc(Get the contact probabilities.

Returns
-------
list of float
    One per group once the model has run; empty with a contact matrix.
)doc";

inline constexpr const char *get_n_groups = R"doc(Get the number of groups.

Returns
-------
int
    The number of groups of the layer.
)doc";

inline constexpr const char *get_group = R"doc(Get the group of an agent.

Parameters
----------
agent : int
    Id of the agent (starting at 0).

Returns
-------
int or None
    The group of the agent, or None if it is not in the layer.
)doc";

inline constexpr const char *set_group =
	R"doc(Move an agent to another group.

Set during a run, the move takes effect at the end of the day and lasts for
that run; set between runs, it is kept.

Parameters
----------
agent : int
    Id of the agent (starting at 0).
group : int or None
    The new group, or None to take the agent out of the layer.
)doc";

inline constexpr const char *members_of = R"doc(Get the members of a group.

Parameters
----------
group : int
    Id of the group (starting at 0).

Returns
-------
list of int
    The ids of the agents in the group, in no particular order.
)doc";

inline constexpr const char *groupmates_of =
	R"doc(Get who shared a group with an agent, both present, over a window of days.

A traced layer with contact probabilities (:meth:`enable_tracing`, before the
run) keeps the run's history of who was in which group and who was absent (in
a state the model takes out of the layers, e.g., isolated in
:class:`~epiworldpy.epimodels.ModelSEIRQuarantine`). With a contact
probability of 1 that is exactly who the agent met; below 1, who it could have
met ("notify the household").

The history covers whole runs: on an untraced layer, with tracing turned on
during a run, or after the grouping is edited between runs, this raises an
error until the next run.

Parameters
----------
agent : int
    Id of the agent (starting at 0).
start : int, default 1
    First day of the window.
end : int, default the last day
    Last day of the window. Days after the last one simulated are left out.

Returns
-------
dict
    ``agent``, ``last_day`` (the last day they shared a group, both present)
    and ``n_days`` (how many days they did): arrays with one entry per
    groupmate, in ascending id order.
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

inline constexpr const char *mixing =
	R"doc(Create a mixing layer.

Give exactly one of ``contact_matrix`` and ``contact_prob``. The number of
groups is the size of the contact matrix, or the number of contact
probabilities; with a single probability, it is the largest group plus one.

Parameters
----------
name : str
    Name of the layer (unique within a model).
groups : list of int or None
    The group of each agent (starting at 0), or None to leave the agent out of
    the layer; one per agent of the model.
contact_matrix : list of float, optional
    Expected number of contacts per day between groups, a square matrix
    flattened by columns.
contact_prob : float or list of float, optional
    Probability, in [0, 1], that two members of a group meet on a given day:
    one for every group or one per group. 1 makes every group a clique
    (everyone meets everyone, every day), with no ties stored.

Returns
-------
Mixing
    The layer, to add with :meth:`epiworldpy.Model.add_layer`.
)doc";

inline constexpr const char *mixing_grouping =
	R"doc(Create a mixing layer whose groups are drawn at every run.

Give exactly one of ``contact_matrix`` and ``contact_prob``. The groups exist
once the model has run, and moving an agent between runs is an error (the next
run draws them again).

Parameters
----------
name : str
    Name of the layer (unique within a model).
groups : Grouping
    A grouping, from :mod:`epiworldpy.grouping`, that draws the groups at the
    start of every run.
contact_matrix : list of float, optional
    Expected number of contacts per day between groups, a square matrix
    flattened by columns (one row per group of the grouping).
contact_prob : float or list of float, optional
    Probability, in [0, 1], that two members of a group meet on a given day:
    one for every group or one per group.

Returns
-------
Mixing
    The layer, to add with :meth:`epiworldpy.Model.add_layer`.
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
