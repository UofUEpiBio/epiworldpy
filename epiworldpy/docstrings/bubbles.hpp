#ifndef EPIWORLDPY_DOCSTRINGS_BUBBLES_HPP
#define EPIWORLDPY_DOCSTRINGS_BUBBLES_HPP

// NumPy-style docstrings for epiworldpy.Bubbles (see tests/test_docstrings.py).

// clang-format off
namespace epiworldpy::docstrings::bubbles {

inline constexpr const char *cls =
	R"doc(A social-bubble policy: a global event for network models.

A "social bubble" policy lets people keep seeing a small, fixed set of others
while cutting off the rest of their contacts, following the household-based
rules used during COVID-19. Households join bubbles along existing contacts
(a network layer), and bubbles never split a household.

Every agent gets a tool that, on each exposure, compares the bubble of the
susceptible agent with that of the infectious one: transmission within a
bubble is left untouched, and transmission between bubbles is multiplied by
the transmission factor. The factor is the model parameter ``param_name``,
which the intervention adds at the start of a run if the model does not have
it, so it can be changed with :meth:`Model.set_param`.

``flavor`` decides who bubbles together; ``ties`` decides who then meets.
With ``ties="existing"`` (the default) the contacts are untouched: the bubble
is only a transmission rule. With ``ties="complete"``, bubblemates also meet,
in a household layer named ``layer``: a mixing layer with one group per
household, built by :meth:`make_layer` and added with
:meth:`Model.add_layer`, in which each pair meets with probability
``household_contact_prob`` a day. While the policy is in force each agent is
in its bubble's group instead, where each pair meets with probability
``bubble_contact_prob`` a day; a household that bubbled with nobody keeps
``household_contact_prob``. When the bubbles are redrawn (``rewire_every``)
agents move to the new ones, and when the policy lifts at ``end_day`` they go
back to their households. Read the groups with the layer's
:meth:`~epiworldpy.layer.Mixing.get_group` and
:meth:`~epiworldpy.layer.Mixing.members_of`.

These are the closest settings for two styles of rule used during COVID-19,
not exact matches:

========================================================  =================================================
Policy style                                              Settings
========================================================  =================================================
Households join one or a few others (New Zealand; UK      ``"household"``, ``group_size`` = households per
support bubbles; Belgium, May 2020)                       bubble, ``ties="complete"``
One close contact per person (Belgium, October 2020)      ``"peer"``, ``max_households=2`` (the default),
                                                          ``ties="existing"``
========================================================  =================================================

The second is an approximation. ``"peer"`` commits whole households, so with
``max_households=2`` one accepted choice pairs two households and the other
members of either can choose no one else (``group_size`` then has no effect).
Under ``"existing"`` every tie between the two households is spared, not only
the chosen one. The model therefore gives a household one partner household
where the rule gives each person their own contact; a larger
``max_households`` lets more members choose, at the cost of larger bubbles.
``flavor="peer"`` with ``ties="complete"`` has both households meet in full:
more contact than the rule allows, but still allowed, for the case where a
person brings their household along.

Examples
--------
>>> from epiworldpy import Bubbles, epimodels, graph, layer
>>> model = epimodels.ModelSIR("flu", n=600, prevalence=0.01,
...                            transmission_prob=0.1, recovery_rate=0.1)
>>> model.add_layer(layer.network("contacts", graph.smallworld(k=6, p=0.1)))
>>> households = [i // 3 for i in range(600)]
>>> b = Bubbles(households, "household", 3, ties="complete",
...             layer="household", bubble_contact_prob=0.5)
>>> model.add_layer(b.make_layer(1.0))
>>> model.add_globalevent(b)
)doc";

inline constexpr const char *init = R"doc(Create a social-bubble policy.

Parameters
----------
household_id : list of int
    Household label of each agent, in agent order; its length must equal the
    number of agents. Labels are arbitrary and need not be consecutive.
    Agents sharing a label form a household and are always placed in the
    same bubble.
flavor : str
    ``"household"`` if whole households choose a bubble together, ``"peer"``
    if individuals choose contacts.
group_size : int
    For ``"household"``, the maximum number of households per bubble (``1``
    gives a strict household-only lockdown). For ``"peer"``, the maximum
    number of contacts each agent may keep outside its household.
transmission_factor : float, default 0.0
    In [0, 1]. Multiplier applied to transmission between agents of
    *different* bubbles: ``0`` cuts out-of-bubble contact entirely, ``1``
    turns the intervention off. The initial value of the model parameter
    ``param_name``.
start_day : int, default 0
    First day on which the policy applies.
end_day : int, default -1
    Day on which the policy is lifted (exclusive); a negative value means it
    never lifts. Must be greater than ``start_day``.
rewire_every : int, default 0
    Re-randomize the bubbles every this-many days; ``0`` keeps them fixed.
name : str, default "Social bubble"
    Name given to the intervention's tool and event.
max_households : int, default 2
    ``"peer"`` flavor only. The largest number of households a bubble may
    contain; a nomination that would exceed it is declined.
param_name : str, default "Bubble transmission factor"
    Name of the model parameter in which the transmission factor is stored.
ties : str, default "existing"
    ``"existing"`` or ``"complete"`` (see :meth:`set_ties`).
layer : str, default ""
    With ``ties="complete"``, the name of the household layer (e.g.,
    ``"household"``), built by :meth:`make_layer`.
bubble_contact_prob : float, default 1.0
    In [0, 1]. With ``ties="complete"``, the probability that two members of
    a bubble that merged households meet on a given day while the policy is
    in force.
)doc";

inline constexpr const char *set_ties = R"doc(Choose how the bubbles are realized.

Set it before adding the policy to the model, which keeps its own copy.

Parameters
----------
ties : str
    ``"existing"`` leaves the contacts untouched: the bubble is only a
    transmission rule. ``"complete"`` also makes bubblemates meet, as groups
    of the household layer ``layer``.
layer : str, default ""
    With ``ties="complete"``, the name of the household layer.
bubble_contact_prob : float, default 1.0
    In [0, 1]. With ``ties="complete"``, the probability that two members of
    a bubble that merged households meet on a given day.
)doc";

inline constexpr const char *make_layer =
	R"doc(Build the household layer of ``ties="complete"``.

A within-group mixing layer named after ``layer``, with one group per
household. Add it with :meth:`Model.add_layer`; while the policy is in force,
the bubbles are larger groups of it.

Parameters
----------
household_contact_prob : float, default 1.0
    In [0, 1]. The probability that two housemates meet on a given day
    (``1``: every day).

Returns
-------
epiworldpy.layer.Mixing
    The layer.
)doc";

inline constexpr const char *get_from =
	R"doc(Get the model's copy of a social-bubble policy.

:meth:`Model.add_globalevent` copies the policy, so the bubbles of a run are
read from the model's copy.

Parameters
----------
model : Model
    The model the policy was added to.
name : str, default "Social bubble"
    The name it was added under.

Returns
-------
Bubbles or None
    The model's copy, or ``None`` if the model has no social-bubble policy by
    that name.
)doc";

inline constexpr const char *get_name = R"doc(Get the name of the policy.

Returns
-------
str
    Name of the policy.
)doc";

inline constexpr const char *get_bubble_id =
	R"doc(Get the current bubble of every agent.

Populated at the start of each run, and only on the model's copy of the
policy (see :meth:`get_from`).

Returns
-------
list of int
    Bubble label of each agent, by agent id (``-1``: unassigned).
)doc";

inline constexpr const char *get_flavor =
	R"doc(Get how bubbles are formed.

Returns
-------
str
    ``"household"`` or ``"peer"``.
)doc";

inline constexpr const char *get_ties =
	R"doc(Get how the bubbles are realized.

Returns
-------
str
    ``"existing"`` or ``"complete"``.
)doc";

inline constexpr const char *get_param_name =
	R"doc(Get the name of the model parameter holding the transmission factor.

Returns
-------
str
    Name of the parameter (see :meth:`Model.get_param`).
)doc";

inline constexpr const char *is_active =
	R"doc(Check whether the policy is in force on a day.

Parameters
----------
today : int
    The day.

Returns
-------
bool
    ``True`` if ``start_day <= today`` and the policy has not lifted.
)doc";

inline constexpr const char *get_last_epoch =
	R"doc(Get the rewiring epoch the current bubbles were drawn for.

Returns
-------
int
    ``0`` for the bubbles drawn at setup, incrementing every
    ``rewire_every`` days while the policy is active; ``-1`` before the
    first draw.
)doc";

} // namespace epiworldpy::docstrings::bubbles
// clang-format on

#endif
