#ifndef EPIWORLDPY_DOCSTRINGS_ENTITY_HPP
#define EPIWORLDPY_DOCSTRINGS_ENTITY_HPP

// NumPy-style docstrings for epiworldpy.Entity (see tests/test_docstrings.py).

// clang-format off
namespace epiworldpy::docstrings::entity {

inline constexpr const char *cls =
	R"doc(A group of agents, such as a household, school, or age group.

Mixing models (for example, :class:`~epiworldpy.epimodels.ModelSIRMixing`)
use entities as the groups of their contact matrix. Add an entity to a model
with :meth:`Model.add_entity`; its distribution function assigns agents to it
when the model is run.

Examples
--------
>>> from epiworldpy import Entity
>>> # Agents 0-4999 in one group, 5000-9999 in the other.
>>> young = Entity("Young", Entity.distribute_to_range(0, 5000))
>>> old = Entity("Old", Entity.distribute_to_range(5000, 10000))
)doc";

inline constexpr const char *init = R"doc(Create an entity.

Parameters
----------
name : str
    Name of the entity.
fun : callable or None, default None
    Function ``fun(entity, model) -> None`` that assigns agents to the
    entity, for example one from :meth:`distribute_randomly`,
    :meth:`distribute_to_range`, or :meth:`distribute_to_set`.
)doc";

inline constexpr const char *get_id = R"doc(Get the entity's ID in the model.

Returns
-------
int
    ID of the entity.
)doc";

inline constexpr const char *get_name = R"doc(Get the name of the entity.

Returns
-------
str
    Name of the entity.
)doc";

inline constexpr const char *size =
	R"doc(Get the number of agents in the entity.

Returns
-------
int
    Number of agents.
)doc";

inline constexpr const char *get_agents_ids =
	R"doc(Get the agents in the entity.

Returns
-------
list of int
    IDs of the agents.
)doc";

inline constexpr const char *set_location = R"doc(Set the entity's location.

Parameters
----------
location : list of float
    Coordinates of the entity.
)doc";

inline constexpr const char *get_location = R"doc(Get the entity's location.

Returns
-------
list of float
    Coordinates of the entity.
)doc";

inline constexpr const char *set_state =
	R"doc(Set the states agents move to when they join or leave the entity.

Parameters
----------
init : int
    State of an agent when it joins the entity.
post : int
    State of an agent when it leaves the entity.
)doc";

inline constexpr const char *set_queue =
	R"doc(Set how joining or leaving the entity updates the queuing system.

Values follow :meth:`Agent.change_state`'s ``queue`` argument.

Parameters
----------
init : int
    Queue change when an agent joins the entity.
post : int
    Queue change when an agent leaves the entity.
)doc";

inline constexpr const char *set_distribution =
	R"doc(Set how agents are assigned to the entity at the start of a run.

Parameters
----------
fun : callable
    Function ``fun(entity, model) -> None``, for example one from
    :meth:`distribute_randomly`, :meth:`distribute_to_range`, or
    :meth:`distribute_to_set`.
)doc";

inline constexpr const char *distribute =
	R"doc(Assign agents to the entity with its distribution function.

Parameters
----------
model : Model
    The model the entity belongs to.
)doc";

inline constexpr const char *print = R"doc(Print information about the entity.
)doc";

inline constexpr const char *new_entity_to_agent_fun =
	R"doc(Wrap a Python function as a distribution function.

Parameters
----------
fun : callable
    Function ``fun(entity, model) -> None`` that assigns agents to
    ``entity``.

Returns
-------
callable
    The distribution function, for :class:`Entity` or
    :meth:`set_distribution`.
)doc";

inline constexpr const char *distribute_randomly =
	R"doc(Get a distribution function that assigns random agents to the entity.

Parameters
----------
prevalence : float
    Proportion or number of agents to assign (see ``as_proportion``).
as_proportion : bool, default True
    If ``True``, ``prevalence`` is a proportion; otherwise, a number of
    agents.
to_unassigned : bool, default False
    If ``True``, only pick agents that do not belong to any entity yet.

Returns
-------
callable
    The distribution function, for :class:`Entity` or
    :meth:`set_distribution`.
)doc";

inline constexpr const char *distribute_to_range =
	R"doc(Get a distribution function that assigns a range of agents to the entity.

Parameters
----------
from_ : int
    ID of the first agent.
to_ : int
    ID after the last agent (the range is ``[from_, to_)``).

Returns
-------
callable
    The distribution function, for :class:`Entity` or
    :meth:`set_distribution`.
)doc";

inline constexpr const char *distribute_to_set =
	R"doc(Get a distribution function that assigns given agents to the entity.

Parameters
----------
ids : list of int
    IDs of the agents.

Returns
-------
callable
    The distribution function, for :class:`Entity` or
    :meth:`set_distribution`.
)doc";

} // namespace epiworldpy::docstrings::entity
// clang-format on

#endif
