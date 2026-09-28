#ifndef EPIWORLDPY_DOCSTRINGS_TOOL_HPP
#define EPIWORLDPY_DOCSTRINGS_TOOL_HPP

// NumPy-style docstrings for epiworldpy.Tool (see tests/test_docstrings.py).

// clang-format off
namespace epiworldpy::docstrings::tool {

inline constexpr const char *cls =
	R"doc(A tool that changes how a virus affects the agents who carry it.

Tools represent interventions such as vaccines, masks, or treatments. A tool
can reduce its carrier's susceptibility to infection, the chance that its
carrier transmits a virus, or its carrier's chance of dying, and can increase
its carrier's chance of recovering. Add it to a model with
:meth:`Model.add_tool`.

As with :class:`Virus`, each effect can be a constant, a model parameter, or
a Python function.

Examples
--------
>>> from epiworldpy import Tool, epimodels
>>> model = epimodels.ModelSIRCONN("COVID-19", n=10000, prevalence=0.01,
...                                contact_rate=5, transmission_rate=0.4,
...                                recovery_rate=0.95)
>>> vaccine = Tool("Vaccine", prevalence=0.5, as_proportion=True,
...                susceptibility_reduction=0.9, transmission_reduction=0.5)
>>> model.add_tool(vaccine)
)doc";

inline constexpr const char *init = R"doc(Create a tool.

Parameters
----------
name : str
    Name of the tool.
prevalence : float
    Proportion or number of agents who receive the tool at the start (see
    ``as_proportion``).
as_proportion : bool
    If ``True``, ``prevalence`` is a proportion; otherwise, a number of
    agents.
susceptibility_reduction : float, default 0.0
    Proportion by which the tool reduces its carrier's probability of
    being infected.
transmission_reduction : float, default 0.0
    Proportion by which the tool reduces the probability that its carrier
    transmits a virus.
recovery_enhancer : float, default 0.0
    Proportion by which the tool increases its carrier's probability of
    recovery.
death_reduction : float, default 0.0
    Proportion by which the tool reduces its carrier's probability of
    death.
)doc";

inline constexpr const char *get_id = R"doc(Get the tool's ID in the model.

Returns
-------
int
    ID of the tool.
)doc";

inline constexpr const char *get_name = R"doc(Get the name of the tool.

Returns
-------
str
    Name of the tool.
)doc";

inline constexpr const char *set_name = R"doc(Set the name of the tool.

Parameters
----------
name : str
    New name.
)doc";

inline constexpr const char *get_date =
	R"doc(Get the day the agent carrying this copy of the tool received it.

Returns
-------
int
    Simulation day.
)doc";

inline constexpr const char *set_date =
	R"doc(Set the day the agent carrying this copy of the tool received it.

Parameters
----------
date : int
    Simulation day.
)doc";

#define EPIWORLDPY_DOC_CONSTANT(arg, what)                                     \
    "Set the "                                                                 \
    what                                                                       \
    ".\n"                                                                      \
    "\n"                                                                       \
    "Parameters\n"                                                             \
    "----------\n"                                                             \
    arg                                                                        \
    " : float\n"                                                               \
    "    Proportion between 0 and 1.\n"

#define EPIWORLDPY_DOC_PARAM(what)                                             \
    "Read the "                                                                \
    what                                                                       \
    " from a model parameter.\n"                                               \
    "\n"                                                                       \
    "The tool reads the parameter's current value each time, so later\n"       \
    "changes to the parameter (see :meth:`Model.set_param`) take effect.\n"    \
    "\n"                                                                       \
    "Parameters\n"                                                             \
    "----------\n"                                                             \
    "param : str\n"                                                            \
    "    Name of the model parameter.\n"

#define EPIWORLDPY_DOC_FUN(what)                                               \
    "Compute the "                                                             \
    what                                                                       \
    " with a Python function.\n"                                               \
    "\n"                                                                       \
    "Parameters\n"                                                             \
    "----------\n"                                                             \
    "fun : callable\n"                                                         \
    "    Function ``fun(tool, agent, virus, model) -> float`` returning the\n" \
    "    "                                                                     \
    what                                                                       \
    " for ``agent`` against ``virus``.\n"

inline constexpr const char *set_susceptibility_reduction =
	EPIWORLDPY_DOC_CONSTANT("susceptibility_reduction",
							"reduction in the carrier's probability of "
							"being infected");
inline constexpr const char *set_transmission_reduction =
	EPIWORLDPY_DOC_CONSTANT("transmission_reduction",
							"reduction in the probability that the "
							"carrier transmits a virus");
inline constexpr const char *set_recovery_enhancer = EPIWORLDPY_DOC_CONSTANT(
	"recovery_enhancer", "increase in the carrier's probability of "
						 "recovery");
inline constexpr const char *set_death_reduction = EPIWORLDPY_DOC_CONSTANT(
	"death_reduction", "reduction in the carrier's probability of death");

inline constexpr const char *set_susceptibility_reduction_param =
	EPIWORLDPY_DOC_PARAM("susceptibility reduction");
inline constexpr const char *set_transmission_reduction_param =
	EPIWORLDPY_DOC_PARAM("transmission reduction");
inline constexpr const char *set_recovery_enhancer_param =
	EPIWORLDPY_DOC_PARAM("recovery enhancer");
inline constexpr const char *set_death_reduction_param =
	EPIWORLDPY_DOC_PARAM("death reduction");

inline constexpr const char *set_susceptibility_reduction_fun =
	EPIWORLDPY_DOC_FUN("susceptibility reduction");
inline constexpr const char *set_transmission_reduction_fun =
	EPIWORLDPY_DOC_FUN("transmission reduction");
inline constexpr const char *set_recovery_enhancer_fun =
	EPIWORLDPY_DOC_FUN("recovery enhancer");
inline constexpr const char *set_death_reduction_fun =
	EPIWORLDPY_DOC_FUN("death reduction");

#undef EPIWORLDPY_DOC_CONSTANT
#undef EPIWORLDPY_DOC_PARAM
#undef EPIWORLDPY_DOC_FUN

inline constexpr const char *set_state =
	R"doc(Set the states agents move to when this tool is acquired or lost.

Parameters
----------
init : int
    State of an agent when it receives the tool.
post : int
    State of an agent when it loses the tool.
)doc";

inline constexpr const char *set_queue =
	R"doc(Set how acquiring or losing this tool updates the queuing system.

Values follow :meth:`Agent.change_state`'s ``queue`` argument.

Parameters
----------
init : int
    Queue change when an agent receives the tool.
post : int
    Queue change when an agent loses the tool.
)doc";

inline constexpr const char *get_state =
	R"doc(Get the states set with :meth:`set_state`.

Returns
-------
dict
    Keys ``"init"`` and ``"post"``.
)doc";

inline constexpr const char *get_queue =
	R"doc(Get the queue changes set with :meth:`set_queue`.

Returns
-------
dict
    Keys ``"init"`` and ``"post"``.
)doc";

inline constexpr const char *set_distribution =
	R"doc(Set how the tool is distributed to agents at the start of a run.

By default, the tool goes to a random ``prevalence`` of the agents.

Parameters
----------
fun : callable
    Function ``fun(tool, model) -> None`` that gives the tool to the
    initial agents, for example with :meth:`Agent.add_tool`.
)doc";

inline constexpr const char *distribute =
	R"doc(Distribute the tool to agents with its distribution function.

Parameters
----------
model : Model
    The model to distribute the tool in.
)doc";

inline constexpr const char *print = R"doc(Print information about the tool.
)doc";

} // namespace epiworldpy::docstrings::tool
// clang-format on

#endif
