#ifndef EPIWORLDPY_DOCSTRINGS_VIRUS_HPP
#define EPIWORLDPY_DOCSTRINGS_VIRUS_HPP

// NumPy-style docstrings for epiworldpy.Virus (see tests/test_docstrings.py).

// clang-format off
namespace epiworldpy::docstrings::virus {

inline constexpr const char *cls =
	R"doc(A virus (or any other contagious condition) and its natural history.

A virus carries the stages of the disease and the transitions between them.
A stage is a **carrier** if its agents hold the virus, and **infectious** if
they transmit it (which implies carrier); the first stage is where agents
start. The transitions name the model parameters they read:

- :meth:`add_infection`: by contact with an infectious agent, with a
  per-contact probability;
- :meth:`add_rate`: with a daily probability (the rates out of a stage
  compete, and one draw a day picks one of them, or none);
- :meth:`add_after`: after a duration drawn when the agent enters the stage
  (:class:`Duration`);
- :meth:`add_external`: only when fired (:meth:`Model.fire`).

Add the virus to a model with :meth:`Model.add_virus`, which places it
(:mod:`epiworldpy.assign`). The declaring methods return the virus, so they
can be chained.

Examples
--------
>>> from epiworldpy import Model, Virus, assign
>>> flu = (Virus("Flu")
...        .add_stage("Susceptible")
...        .add_stage("Infected", infectious=True)
...        .add_stage("Recovered")
...        .add_infection("Susceptible", "Infected",
...                       rate="Transmission probability")
...        .add_rate("Infected", "Recovered", rate="Recovery rate",
...                  effect="recovery"))
>>> model = Model(1000)
>>> model.add_param(0.2, "Transmission probability")
>>> model.add_param(0.1, "Recovery rate")
>>> model.add_virus(flu, assign.share(0.01))
)doc";

inline constexpr const char *init = R"doc(Create a virus without stages.

Parameters
----------
name : str
    Name of the virus.
)doc";

inline constexpr const char *get_id = R"doc(Get the virus's ID in the model.

Returns
-------
int
    ID of the virus.
)doc";

inline constexpr const char *get_lineage_id =
	R"doc(Get the virus's lineage id.

A lineage is a virus added to a model together with all of its variants;
contact layers and tools target viruses by lineage.

Returns
-------
int
    Lineage id, or -99 if the virus has not been added to a model.
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

inline constexpr const char *set_mutation = R"doc(Set the mutation function.

The function is called on each copy of the virus at every step.

Parameters
----------
fun : callable
    Function ``fun(agent, virus, model) -> bool`` that may change ``virus``
    and returns ``True`` if it mutated, so the new variant is recorded in the
    database.
)doc";

inline constexpr const char *print = R"doc(Print information about the virus.
)doc";

inline constexpr const char *add_stage = R"doc(Add a stage to the natural history.

Parameters
----------
label : str
    Name of the stage (unique within the virus).
infectious : bool, default False
    Whether its agents transmit the virus.
carrier : bool or None, default None
    Whether its agents hold the virus; ``None`` takes ``infectious`` (an
    infectious stage is a carrier stage). Entering a stage that is not a
    carrier releases the virus.

Returns
-------
Virus
    The virus itself.
)doc";

inline constexpr const char *set_contact =
	R"doc(Set how much a stage's agents take part in the contact layers.

Parameters
----------
stage : str
    Name of the stage.
weight : float
    Contact weight in [0, 1]; ``0`` takes the stage's agents out of every
    layer (they are not met and do not transmit), as for a death.

Returns
-------
Virus
    The virus itself.
)doc";

inline constexpr const char *add_infection =
	R"doc(Add an infection: susceptible agents are infected by contact.

Parameters
----------
from_ : str or list of str
    The stage (or stages) the infection leaves.
to : str
    The stage it leads to (a carrier stage).
rate : float, str, or Value
    The per-contact probability of transmission: a number, the name of a
    model parameter, or a :class:`Value`.
transition_name : str or None, default None
    Name of the transition in the outputs (default ``"<from> -> <to>"``).

Returns
-------
Virus
    The virus itself.
)doc";

inline constexpr const char *add_rate =
	R"doc(Add a transition with a daily probability.

Parameters
----------
from_ : str or list of str
    The stage (or stages) it leaves.
to : str
    The stage it leads to.
rate : float, str, or Value
    The daily probability: a number, the name of a model parameter, or a
    :class:`Value`.
effect : str, default "none"
    How tools act on it: ``"recovery"`` (their recovery enhancer raises it),
    ``"death"`` (their death reduction lowers it), or ``"none"``.
transition_name : str or None, default None
    Name of the transition in the outputs (default ``"<from> -> <to>"``).

Returns
-------
Virus
    The virus itself.
)doc";

inline constexpr const char *add_after =
	R"doc(Add a transition after a duration, drawn when the agent enters the stage.

Parameters
----------
from_ : str
    The stage it leaves.
to : str
    The stage it leads to.
duration : Duration
    The time in the stage (e.g., ``Duration.geometric("Latent period")``).
since : str or None, default None
    Count the duration from the day the agent entered this stage in the same
    infection, instead of ``from_``.
transition_name : str or None, default None
    Name of the transition in the outputs (default ``"<from> -> <to>"``).

Returns
-------
Virus
    The virus itself.
)doc";

inline constexpr const char *add_external =
	R"doc(Add a transition that only happens when fired (see :meth:`Model.fire`).

Parameters
----------
from_ : str or list of str
    The stage (or stages) it leaves.
to : str
    The stage it leads to.
transition_name : str
    Name of the transition, used to fire it.

Returns
-------
Virus
    The virus itself.
)doc";

inline constexpr const char *use_param =
	R"doc(Make the virus's transitions read another model parameter.

Used by strains: a copy of the model's virus (:meth:`clone`) whose
transitions read, say, ``"Delta transmission probability"`` in place of
``"Transmission probability"``.

Parameters
----------
param : str
    The parameter the transitions name.
replacement : str
    The parameter this virus reads instead.

Returns
-------
Virus
    The virus itself.
)doc";

inline constexpr const char *get_stages = R"doc(Get the virus's stages.

Returns
-------
dict
    ``stage`` (labels), ``carrier`` and ``infectious`` (bools), and
    ``contact`` (weights), one entry per stage, in order.
)doc";

inline constexpr const char *clone = R"doc(Copy the virus.

The copy is a new variant: added to a model, it gets its own id (and
lineage), so the model records it apart from the original. Rename it with
:meth:`set_name`.

Returns
-------
Virus
    The copy.
)doc";

// Values and durations.

inline constexpr const char *value_cls =
	R"doc(The value of a transition: a number, a model parameter, or a function.

Transitions take a number or a parameter's name directly; a ``Value`` adds
``1 / parameter`` (:meth:`inverse`), ``1 - parameter`` (:meth:`complement`),
and a Python function of the agent (:meth:`fun`).
)doc";

inline constexpr const char *value_init = R"doc(Create a value.

Parameters
----------
x : float or str
    A number, or the name of a model parameter.
)doc";

inline constexpr const char *value_inverse = R"doc(One over a model parameter.

For example, a daily rate from a parameter holding a period in days.

Parameters
----------
param : str
    Name of the model parameter.

Returns
-------
Value
    ``1 / param``.
)doc";

inline constexpr const char *value_complement =
	R"doc(One minus a model parameter.

For example, the probability of infection from a vaccine's efficacy.

Parameters
----------
param : str
    Name of the model parameter.

Returns
-------
Value
    ``1 - param``.
)doc";

inline constexpr const char *value_fun = R"doc(A value from a Python function.

The function is called each time the value is needed: once per agent and
day it applies to.

Parameters
----------
fun : callable
    Function ``fun(agent, model) -> float``.

Returns
-------
Value
    The value.
)doc";

inline constexpr const char *duration_cls =
	R"doc(A duration in a stage, drawn when the agent enters it.

Each argument is a number, the name of a model parameter, or a
:class:`Value`. See :meth:`Virus.add_after`.
)doc";

inline constexpr const char *duration_fixed = R"doc(A fixed number of days.

Parameters
----------
days : float, str, or Value
    The number of days.

Returns
-------
Duration
    The duration.
)doc";

inline constexpr const char *duration_geometric =
	R"doc(A geometric number of days (a daily probability of ``1 / mean``).

Parameters
----------
mean : float, str, or Value
    The mean number of days.

Returns
-------
Duration
    The duration.
)doc";

inline constexpr const char *duration_lognormal =
	R"doc(A lognormal number of days, rounded.

Parameters
----------
mean : float, str, or Value
    The mean number of days.
sd : float, str, or Value
    The standard deviation.

Returns
-------
Duration
    The duration.
)doc";

inline constexpr const char *duration_gamma =
	R"doc(A gamma-distributed number of days, rounded.

Parameters
----------
mean : float, str, or Value
    The mean number of days.
sd : float, str, or Value
    The standard deviation.

Returns
-------
Duration
    The duration.
)doc";

} // namespace epiworldpy::docstrings::virus
// clang-format on

#endif
