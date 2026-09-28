#ifndef EPIWORLDPY_DOCSTRINGS_VIRUS_HPP
#define EPIWORLDPY_DOCSTRINGS_VIRUS_HPP

// NumPy-style docstrings for epiworldpy.Virus (see tests/test_docstrings.py).

// clang-format off
namespace epiworldpy::docstrings::virus {

inline constexpr const char *cls =
	R"doc(A virus (or any other contagious condition) that spreads between agents.

A virus sets the probabilities of infection, recovery, and death, and the
states infected agents move through. Add it to a model with
:meth:`Model.add_virus`; each infected agent carries its own copy.

Each probability can be a constant (``set_prob_infecting(0.3)``), a model
parameter (``set_prob_infecting("Transmission rate")``, which follows later
changes to the parameter), or a Python function
(:meth:`set_prob_infecting_fun`). The agents' tools then modify these
probabilities (see :class:`Tool`).

Examples
--------
>>> from epiworldpy import Virus, epimodels
>>> model = epimodels.ModelSEIRCONN("COVID-19", n=10000, prevalence=0.01,
...                                 contact_rate=2, transmission_rate=0.5,
...                                 incubation_days=7, recovery_rate=0.3)
>>> flu = Virus("Flu", prevalence=0.001, as_proportion=True,
...             prob_infecting=0.9, prob_recovery=1 / 7, prob_death=0.0)
>>> model.add_virus(flu)
)doc";

inline constexpr const char *init = R"doc(Create a virus.

Parameters
----------
name : str
    Name of the virus.
prevalence : float
    Proportion or number of agents infected at the start (see
    ``as_proportion``).
as_proportion : bool
    If ``True``, ``prevalence`` is a proportion; otherwise, a number of
    agents.
prob_infecting : float
    Probability of transmission per contact.
prob_recovery : float
    Daily probability of recovery.
prob_death : float
    Daily probability of death.
post_immunity : float, default 0.0
    Protection against reinfection after recovery: recovered agents receive
    a tool that reduces their susceptibility to this virus by this
    proportion. ``0`` gives no protection.
incubation : float or None, default None
    Average incubation period in days, used by models with an exposed
    state.
)doc";

inline constexpr const char *get_id = R"doc(Get the virus's ID in the model.

Returns
-------
int
    ID of the virus.
)doc";

inline constexpr const char *get_name = R"doc(Get the name of the virus.

Returns
-------
str
    Name of the virus.
)doc";

inline constexpr const char *set_name = R"doc(Set the name of the virus.

Parameters
----------
name : str
    New name.
)doc";

inline constexpr const char *get_date =
	R"doc(Get the day the agent carrying this copy of the virus was infected.

Returns
-------
int
    Simulation day.
)doc";

inline constexpr const char *set_date =
	R"doc(Set the day the agent carrying this copy of the virus was infected.

Parameters
----------
date : int
    Simulation day.
)doc";

#define EPIWORLDPY_DOC_PARAM_BINDING(what)                                     \
    "Read the "                                                                \
    what                                                                       \
    " from a model parameter.\n"                                               \
    "\n"                                                                       \
    "The virus reads the parameter's current value each time, so later\n"      \
    "changes to the parameter (see :meth:`Model.set_param`) take effect.\n"    \
    "\n"                                                                       \
    "Parameters\n"                                                             \
    "----------\n"                                                             \
    "param : str\n"                                                            \
    "    Name of the model parameter.\n"

inline constexpr const char *set_prob_infecting =
	R"doc(Set the probability of transmission per contact.

Parameters
----------
prob_infecting : float
    Probability between 0 and 1.
)doc";

inline constexpr const char *set_prob_infecting_param =
	EPIWORLDPY_DOC_PARAM_BINDING("probability of transmission per contact");

inline constexpr const char *set_prob_recovery =
	R"doc(Set the daily probability of recovery.

Parameters
----------
prob_recovery : float
    Probability between 0 and 1.
)doc";

inline constexpr const char *set_prob_recovery_param =
	EPIWORLDPY_DOC_PARAM_BINDING("daily probability of recovery");

inline constexpr const char *set_prob_death =
	R"doc(Set the daily probability of death.

Parameters
----------
prob_death : float
    Probability between 0 and 1.
)doc";

inline constexpr const char *set_prob_death_param =
	EPIWORLDPY_DOC_PARAM_BINDING("daily probability of death");

inline constexpr const char *set_incubation =
	R"doc(Set the average incubation period.

Parameters
----------
incubation : float
    Average number of days in the exposed state.
)doc";

inline constexpr const char *set_incubation_param =
	EPIWORLDPY_DOC_PARAM_BINDING("average incubation period");

#undef EPIWORLDPY_DOC_PARAM_BINDING

#define EPIWORLDPY_DOC_GETTER(what)                                            \
    "Get the "                                                                 \
    what                                                                       \
    " for the agent carrying this virus.\n"                                    \
    "\n"                                                                       \
    "Parameters\n"                                                             \
    "----------\n"                                                             \
    "model : Model\n"                                                          \
    "    The model the virus belongs to.\n"                                    \
    "\n"                                                                       \
    "Returns\n"                                                                \
    "-------\n"                                                                \
    "float\n"                                                                  \
    "    The "                                                                 \
    what                                                                       \
    ".\n"

inline constexpr const char *get_prob_infecting =
	EPIWORLDPY_DOC_GETTER("probability of transmission per contact");
inline constexpr const char *get_prob_recovery =
	EPIWORLDPY_DOC_GETTER("daily probability of recovery");
inline constexpr const char *get_prob_death =
	EPIWORLDPY_DOC_GETTER("daily probability of death");
inline constexpr const char *get_incubation =
	EPIWORLDPY_DOC_GETTER("average incubation period");

#undef EPIWORLDPY_DOC_GETTER

#define EPIWORLDPY_DOC_SETTER_FUN(what)                                        \
    "Compute the "                                                             \
    what                                                                       \
    " with a Python function.\n"                                               \
    "\n"                                                                       \
    "Parameters\n"                                                             \
    "----------\n"                                                             \
    "fun : callable\n"                                                         \
    "    Function ``fun(agent, virus, model) -> float`` returning the "        \
    what                                                                       \
    " for ``agent``.\n"

inline constexpr const char *set_prob_infecting_fun =
	EPIWORLDPY_DOC_SETTER_FUN("probability of transmission");
inline constexpr const char *set_prob_recovery_fun =
	EPIWORLDPY_DOC_SETTER_FUN("probability of recovery");
inline constexpr const char *set_prob_death_fun =
	EPIWORLDPY_DOC_SETTER_FUN("probability of death");
inline constexpr const char *set_incubation_fun =
	EPIWORLDPY_DOC_SETTER_FUN("incubation period");

#undef EPIWORLDPY_DOC_SETTER_FUN

inline constexpr const char *set_mutation = R"doc(Set the mutation function.

The function is called on each copy of the virus at every step (see
:meth:`mutate`).

Parameters
----------
fun : callable
    Function ``fun(agent, virus, model) -> bool`` that may change ``virus``
    (for example, with :meth:`set_sequence`) and returns ``True`` if it
    mutated, so the new variant is recorded in the database.
)doc";

inline constexpr const char *set_post_recovery =
	R"doc(Set a function called when an agent recovers from this virus.

Cannot be combined with :meth:`set_post_immunity`, which uses this hook.

Parameters
----------
fun : callable
    Function ``fun(agent, virus, model) -> None``.
)doc";

inline constexpr const char *set_post_immunity =
	R"doc(Protect recovered agents against reinfection.

Recovered agents receive a tool that reduces their susceptibility to this
virus. Cannot be combined with :meth:`set_post_recovery`.

Parameters
----------
prob : float
    Susceptibility reduction between 0 and 1; ``1`` gives full immunity.
)doc";

inline constexpr const char *set_post_immunity_param =
	R"doc(Protect recovered agents against reinfection, reading the protection from a model parameter.

Parameters
----------
param : str
    Name of the model parameter with the susceptibility reduction.
)doc";

inline constexpr const char *post_recovery =
	R"doc(Run the post-recovery function (see :meth:`set_post_recovery`).

Parameters
----------
model : Model
    The model the virus belongs to.
)doc";

inline constexpr const char *set_state =
	R"doc(Set the states agents move to when this virus is acquired or lost.

Parameters
----------
init : int
    State of an agent when it acquires the virus.
end : int
    State of an agent when it loses the virus (for example, on recovery).
removed : int
    State of an agent removed by the virus (for example, on death).
)doc";

inline constexpr const char *set_queue =
	R"doc(Set how acquiring or losing this virus updates the queuing system.

Values follow :meth:`Agent.change_state`'s ``queue`` argument.

Parameters
----------
init : int
    Queue change when an agent acquires the virus.
end : int
    Queue change when an agent loses the virus.
removed : int
    Queue change when an agent is removed by the virus.
)doc";

inline constexpr const char *get_state =
	R"doc(Get the states set with :meth:`set_state`.

Returns
-------
dict
    Keys ``"init"``, ``"end"``, and ``"removed"``.
)doc";

inline constexpr const char *get_queue =
	R"doc(Get the queue changes set with :meth:`set_queue`.

Returns
-------
dict
    Keys ``"init"``, ``"end"``, and ``"removed"``.
)doc";

inline constexpr const char *set_distribution =
	R"doc(Set how the virus is distributed to agents at the start of a run.

By default, the virus infects a random ``prevalence`` of the agents.

Parameters
----------
fun : callable
    Function ``fun(virus, model) -> None`` that infects the initial agents,
    for example with :meth:`Agent.set_virus`.
)doc";

inline constexpr const char *distribute =
	R"doc(Distribute the virus to agents with its distribution function.

Parameters
----------
model : Model
    The model to distribute the virus in.
)doc";

inline constexpr const char *set_sequence =
	R"doc(Set the virus's genetic sequence, which identifies its variant.

Parameters
----------
sequence : int
    The sequence.
)doc";

inline constexpr const char *mutate =
	R"doc(Run the mutation function (see :meth:`set_mutation`).

Parameters
----------
model : Model
    The model the virus belongs to.
)doc";

inline constexpr const char *print = R"doc(Print information about the virus.
)doc";

} // namespace epiworldpy::docstrings::virus
// clang-format on

#endif
