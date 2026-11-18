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

Each effect can be a constant, a model parameter, or a Python function. The
recovery enhancer acts on the transitions declared with
``effect="recovery"``, and the death reduction on those with
``effect="death"`` (see :meth:`Virus.add_rate`).

Examples
--------
>>> from epiworldpy import Tool, assign, epimodels
>>> model = epimodels.ModelSIR("COVID-19", n=10000, prevalence=0.01,
...                            transmission_prob=0.4, recovery_rate=0.95)
>>> vaccine = Tool("Vaccine", susceptibility_reduction=0.9,
...                transmission_reduction=0.5)
>>> model.add_tool(vaccine, assign.share(0.5))
)doc";

inline constexpr const char *init = R"doc(Create a tool.

The effects are keyword arguments. Who has the tool is set when it is added
to a model (:meth:`Model.add_tool`).

Parameters
----------
name : str
    Name of the tool.
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

inline constexpr const char *print = R"doc(Print information about the tool.
)doc";

inline constexpr const char *add_target = R"doc(Restrict this object's effects to a disease, by name.

Names are checked when the model runs. All strains of a disease are targeted.

Parameters
----------
disease : str
    Disease name (see Model.get_diseases()).

Returns
-------
Tool
    This object, for chaining.
)doc";

inline constexpr const char *set_targets = R"doc(Replace the targeted diseases.

Parameters
----------
diseases : list of str or None
    Disease names, or None (or []) to clear the targets.

Returns
-------
Tool
    This tool, for chaining.
)doc";

inline constexpr const char *get_targets = R"doc(Get the targeted disease names.

Returns
-------
list of str
    Disease names. An empty list means every disease; with several diseases,
    tools must explicitly name their targets. Layers may have no targets.
)doc";

inline constexpr const char *clear_targets = R"doc(Clear all disease targets.

Returns
-------
Tool
    This tool, for chaining. With one disease it acts on that disease;
    with several, a tool without explicit targets is an error at run time.
)doc";

inline constexpr const char *targets = R"doc(Check whether this tool acts on a virus's disease.

Call after the model resolves the targets at the start of a run.

Parameters
----------
v : Virus
    A virus belonging to the model.

Returns
-------
bool
    Whether the resolved targets include the virus's disease.
)doc";

inline constexpr const char *add_layer_target =
	R"doc(Restrict this tool's contact effects to a named layer.

Susceptibility and transmission reductions apply only through the targeted
layers. Without layer targets they apply through every layer. Recovery and
death effects are unaffected. An infection given by code, outside any layer,
sees only tools without layer targets. An unknown layer raises an error when
the model runs. Set targets before the run or before adding the tool to a
running model.

Parameters
----------
layer : str
    Name of a layer of the model. Repeated names are kept only once.

Returns
-------
Tool
    This tool, for chaining.
)doc";

inline constexpr const char *set_layer_targets =
	R"doc(Replace the layers targeted by this tool's contact effects.

See :meth:`add_layer_target` for which effects are restricted and when the
targets are checked.

Parameters
----------
layers : list of str
    Layer names. Duplicates are dropped; an empty list clears the targets.
)doc";

inline constexpr const char *get_layer_targets =
	R"doc(Get the layer names targeted by this tool's contact effects.

Returns
-------
list of str
    Layer names, in the order they were added. An empty list means every
    layer, including infections given outside any layer.
)doc";

inline constexpr const char *clear_layer_targets =
	R"doc(Clear all layer targets so the contact effects apply everywhere.

This also includes infections given by code, outside any layer.
)doc";

inline constexpr const char *clone = R"doc(Copy the tool.

The copy is a new tool: added to a model, it gets its own id, so the model
records it apart from the original. Rename it with :meth:`set_name`.

Returns
-------
Tool
    The copy.
)doc";

} // namespace epiworldpy::docstrings::tool
// clang-format on

#endif
