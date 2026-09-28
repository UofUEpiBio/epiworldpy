#ifndef EPIWORLDPY_DOCSTRINGS_AGENT_HPP
#define EPIWORLDPY_DOCSTRINGS_AGENT_HPP

// NumPy-style docstrings for epiworldpy.Agent (see tests/test_docstrings.py).

// clang-format off
namespace epiworldpy::docstrings::agent {

inline constexpr const char *cls =
	R"doc(An individual in the simulated population.

Agents are created by the model (for example, with
:meth:`Model.agents_smallworld`) and accessed with :meth:`Model.get_agent` or
:meth:`Model.get_agents`; they cannot be created directly. Each agent has a
state (an index into :meth:`Model.get_states`), at most one active virus, any
number of tools, and may belong to entities.

Methods that change an agent (``set_virus``, ``add_tool``, ``change_state``,
...) do not act immediately: they record an event that the model applies at
the end of the current simulation step.
)doc";

inline constexpr const char *get_id = R"doc(Get the agent's ID.

Returns
-------
int
    The agent's ID, which is also its index in :meth:`Model.get_agents`.
)doc";

inline constexpr const char *get_state = R"doc(Get the agent's current state.

Returns
-------
int
    Index of the state in :meth:`Model.get_states`.
)doc";

inline constexpr const char *get_state_prev =
	R"doc(Get the agent's previous state.

Returns
-------
int
    Index of the state the agent was in before its last state change.
)doc";

inline constexpr const char *get_state_last_changed =
	R"doc(Get the day the agent's state last changed.

Returns
-------
int
    Simulation day of the last state change.
)doc";

inline constexpr const char *get_virus = R"doc(Get the agent's active virus.

Returns
-------
Virus or None
    The virus currently infecting the agent, or ``None`` if the agent is not
    infected.
)doc";

inline constexpr const char *get_tools = R"doc(Get the tools the agent carries.

Returns
-------
list of Tool
    The agent's tools, in the order they were received.
)doc";

inline constexpr const char *get_n_tools =
	R"doc(Get the number of tools the agent carries.

Returns
-------
int
    Number of tools.
)doc";

inline constexpr const char *get_n_neighbors =
	R"doc(Get the number of neighbors in the contact network.

Returns
-------
int
    Number of neighbors.
)doc";

inline constexpr const char *get_n_entities =
	R"doc(Get the number of entities the agent belongs to.

Returns
-------
int
    Number of entities.
)doc";

inline constexpr const char *has_tool_id =
	R"doc(Check whether the agent has a tool, by ID.

Parameters
----------
t : int
    ID of the tool (see :meth:`Tool.get_id`).

Returns
-------
bool
    ``True`` if the agent carries the tool.
)doc";

inline constexpr const char *has_tool_name =
	R"doc(Check whether the agent has a tool, by name.

Parameters
----------
name : str
    Name of the tool.

Returns
-------
bool
    ``True`` if the agent carries a tool with that name.
)doc";

inline constexpr const char *has_virus_id =
	R"doc(Check whether the agent is infected with a virus, by ID.

Parameters
----------
t : int
    ID of the virus (see :meth:`Virus.get_id`).

Returns
-------
bool
    ``True`` if the agent's active virus has that ID.
)doc";

inline constexpr const char *has_virus_name =
	R"doc(Check whether the agent is infected with a virus, by name.

Parameters
----------
name : str
    Name of the virus.

Returns
-------
bool
    ``True`` if the agent's active virus has that name.
)doc";

inline constexpr const char *has_entity =
	R"doc(Check whether the agent belongs to an entity, by ID.

Parameters
----------
t : int
    ID of the entity (see :meth:`Entity.get_id`).

Returns
-------
bool
    ``True`` if the agent belongs to the entity.
)doc";

inline constexpr const char *change_state = R"doc(Move the agent to a new state.

The change takes effect at the end of the current step.

Parameters
----------
model : Model
    The model the agent belongs to.
new_state : int
    Index of the new state (see :meth:`Model.get_states`).
queue : int, default 0
    How to update the queuing system (see :meth:`Model.queuing_on`): ``2``
    adds the agent and its neighbors to the queue, ``1`` adds only the
    agent, ``-1`` and ``-2`` undo those, and ``0`` leaves the queue
    unchanged.
)doc";

inline constexpr const char *rm_virus = R"doc(Remove the agent's active virus.

The agent moves to the virus's ``end`` state (see :meth:`Virus.set_state`)
at the end of the current step.

Parameters
----------
model : Model
    The model the agent belongs to.
)doc";

inline constexpr const char *set_virus = R"doc(Infect the agent with a virus.

The agent receives a copy of ``virus`` and moves to the virus's ``init``
state (see :meth:`Virus.set_state`) at the end of the current step.

Parameters
----------
model : Model
    The model the agent belongs to.
virus : Virus
    The virus to infect the agent with.
)doc";

inline constexpr const char *add_tool = R"doc(Give the agent a tool.

The agent receives a copy of ``tool`` at the end of the current step.

Parameters
----------
model : Model
    The model the agent belongs to.
tool : Tool
    The tool to give to the agent.
)doc";

inline constexpr const char *mutate_virus =
	R"doc(Mutate the agent's active virus.

Calls the virus's mutation function (see :meth:`Virus.set_mutation`).

Parameters
----------
model : Model
    The model the agent belongs to.

Raises
------
RuntimeError
    If the agent has no active virus.
)doc";

inline constexpr const char *has_neighbor =
	R"doc(Check whether another agent is a neighbor.

Parameters
----------
neighbor_id : int
    ID of the other agent.

Returns
-------
bool
    ``True`` if the other agent is one of this agent's neighbors.
)doc";

inline constexpr const char *get_neighbors =
	R"doc(Get the agent's neighbors in the contact network.

Parameters
----------
model : Model
    The model the agent belongs to.

Returns
-------
list of Agent
    The neighboring agents.
)doc";

inline constexpr const char *rm_tool = R"doc(Remove one of the agent's tools.

The tool is removed at the end of the current step.

Parameters
----------
model : Model
    The model the agent belongs to.
tool_idx : int
    Position of the tool in :meth:`get_tools` (not the tool's ID).
)doc";

inline constexpr const char *add_entity = R"doc(Add the agent to an entity.

The agent joins the entity at the end of the current step.

Parameters
----------
model : Model
    The model the agent belongs to.
entity : Entity
    The entity to join.
)doc";

inline constexpr const char *rm_entity = R"doc(Remove the agent from an entity.

The agent leaves the entity at the end of the current step.

Parameters
----------
model : Model
    The model the agent belongs to.
entity : Entity
    The entity to leave.
)doc";

inline constexpr const char *get_entities =
	R"doc(Get the entities the agent belongs to.

Returns
-------
list of int
    IDs of the entities (see :meth:`Model.get_entity`).
)doc";

} // namespace epiworldpy::docstrings::agent
// clang-format on

#endif
