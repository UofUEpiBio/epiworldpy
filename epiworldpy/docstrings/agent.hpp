#ifndef EPIWORLDPY_DOCSTRINGS_AGENT_HPP
#define EPIWORLDPY_DOCSTRINGS_AGENT_HPP

// NumPy-style docstrings for epiworldpy.Agent (see tests/test_docstrings.py).

// clang-format off
namespace epiworldpy::docstrings::agent {

inline constexpr const char *cls =
	R"doc(An individual in the simulated population.

Agents are created by the model (``n`` of them, given to its constructor) and
accessed with :meth:`Model.get_agent` or :meth:`Model.get_agents`; they cannot
be created directly. Their contacts come from the model's layers (see
:mod:`epiworldpy.layer`). Each agent is in a stage
(:meth:`Model.get_stage`), and a status if the model has them
(:meth:`Model.get_status`); it has at most one active virus, and any number
of tools.

Methods that change an agent (``set_virus``, ``add_tool``, ...) do not act
immediately: they record an event that the model applies at the end of the
current simulation step. Give them the model's copy of the virus or tool
(:meth:`Model.get_virus`, :meth:`Model.get_tool`).
)doc";

inline constexpr const char *get_id = R"doc(Get the agent's ID.

Returns
-------
int
    The agent's ID, which is also its index in :meth:`Model.get_agents`.
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

inline constexpr const char *set_virus = R"doc(Infect the agent with a virus.

At the end of the current step, the agent follows the infection transition
out of its stage (see :meth:`Virus.add_infection`), carrying a copy of
``virus``.

Parameters
----------
model : Model
    The model the agent belongs to.
virus : Virus
    The model's copy of the virus (:meth:`Model.get_virus`).
)doc";

inline constexpr const char *add_tool = R"doc(Give the agent a tool.

The agent receives a copy of ``tool`` at the end of the current step.

Parameters
----------
model : Model
    The model the agent belongs to.
tool : Tool
    The model's copy of the tool (:meth:`Model.get_tool`).
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

inline constexpr const char *rm_tool = R"doc(Remove one of the agent's tools.

The tool is removed at the end of the current step.

Parameters
----------
model : Model
    The model the agent belongs to.
tool_idx : int
    Position of the tool in :meth:`get_tools` (not the tool's ID).
)doc";

} // namespace epiworldpy::docstrings::agent
// clang-format on

#endif
