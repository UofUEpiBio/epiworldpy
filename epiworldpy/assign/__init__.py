"""Placement: who gets a virus, a tool, an initial stage, or a group.

One vocabulary places everything at the start of each run:

- a virus: :meth:`epiworldpy.Model.add_virus` (labels are its stages);
- a tool: :meth:`epiworldpy.Model.add_tool`;
- agents in stages without the virus: :meth:`epiworldpy.Model.set_initial`;
- the groups of a mixing layer: :func:`epiworldpy.layer.mixing`.

Agent ids are 0-based. Each function returns an :class:`Assign`.

Examples
--------
>>> from epiworldpy import assign, epimodels
>>> model = epimodels.ModelSIR("flu", n=1000, prevalence=0,
...                            transmission_prob=0.1, recovery_rate=0.1)
>>> model.assign_virus(0, assign.count(10))
>>> model.set_initial(assign.shares({"Recovered": 0.3}))
"""

from __future__ import annotations

from collections.abc import Callable, Mapping, Sequence
from numbers import Integral, Real

from .. import _core

Assign = _core.assign.Assign


def _make(form, values=(), labels=(), ids=(), layer="", fun=None):
    return _core.assign._make(form, list(values), list(labels), list(ids),
                              layer, fun)


def _is_number(x) -> bool:
    return isinstance(x, Real) and not isinstance(x, bool) and x == x


def _check_values(x, what: str, whole: bool = False, at_most_one: bool = False):
    if not all(_is_number(v) and v >= 0 for v in x) or not len(x):
        raise ValueError(f"`{what}` must be non-negative numbers.")
    if at_most_one and any(v > 1 for v in x):
        raise ValueError(f"`{what}` must be shares in [0, 1].")
    if whole and any(v != int(v) for v in x):
        raise ValueError(f"`{what}` must be whole numbers.")


def _labelled(x) -> tuple[list, list]:
    """The labels and values of shares or counts: a dict's keys (stages or
    groups), or 0, 1, ... for a list."""
    if isinstance(x, Mapping):
        return list(x.keys()), list(x.values())
    values = list(x)
    return list(range(len(values))), values


def share(p: float) -> Assign:
    """A share of the eligible agents, drawn at random.

    Parameters
    ----------
    p : float
        The share, in [0, 1]; ``floor(p * eligible)`` agents.

    Returns
    -------
    Assign
        The placement.
    """
    if not _is_number(p) or not 0 <= p <= 1:
        raise ValueError("`p` must be in [0, 1].")
    return _make("share", [p])


def count(n: int) -> Assign:
    """A number of the eligible agents, drawn at random.

    Parameters
    ----------
    n : int
        The number of agents.

    Returns
    -------
    Assign
        The placement.
    """
    if not _is_number(n) or n < 0 or n != int(n):
        raise ValueError("`n` must be a whole, non-negative number.")
    return _make("count", [n])


def shares(x: Mapping | Sequence[float]) -> Assign:
    """Shares by label, drawn in order from the agents not yet labelled.

    Each share is ``floor(p * eligible)`` of all the eligible agents, drawn
    from those not labelled yet.

    Parameters
    ----------
    x : dict or list of float
        The shares, in [0, 1]: a dict by label (a stage name, or a group
        number), or a list (labels 0, 1, ...).

    Returns
    -------
    Assign
        The placement.
    """
    labels, values = _labelled(x)
    _check_values(values, "x", at_most_one=True)
    return _make("shares", values, labels)


def counts(x: Mapping | Sequence[int]) -> Assign:
    """Numbers of agents by label, drawn in order from those not yet labelled.

    Parameters
    ----------
    x : dict or list of int
        The counts: a dict by label (a stage name, or a group number), or a
        list (labels 0, 1, ...).

    Returns
    -------
    Assign
        The placement.
    """
    labels, values = _labelled(x)
    _check_values(values, "x", whole=True)
    return _make("counts", values, labels)


def group_shares(layer: str, x: Sequence[float]) -> Assign:
    """A share of each group of a mixing layer.

    Parameters
    ----------
    layer : str
        Name of the mixing layer.
    x : list of float
        One share per group (or one for every group), in [0, 1].

    Returns
    -------
    Assign
        The placement.
    """
    _check_values(list(x), "x", at_most_one=True)
    return _make("group_shares", list(x), layer=layer)


def group_counts(layer: str, x: Sequence[int]) -> Assign:
    """A number of agents of each group of a mixing layer.

    Parameters
    ----------
    layer : str
        Name of the mixing layer.
    x : list of int
        One count per group (or one for every group).

    Returns
    -------
    Assign
        The placement.
    """
    _check_values(list(x), "x", whole=True)
    return _make("group_counts", list(x), layer=layer)


def agents(ids: Sequence[int]) -> Assign:
    """Given agents.

    Parameters
    ----------
    ids : list of int
        The agents' ids (starting at 0).

    Returns
    -------
    Assign
        The placement.
    """
    ids = list(ids)
    if not all(isinstance(i, Integral) and not isinstance(i, bool) and i >= 0
               for i in ids):
        raise ValueError("`ids` must be agent ids (whole numbers, starting at 0).")
    return _make("agents", ids=ids)


def labels(labels: Sequence) -> Assign:
    """One label per agent.

    Parameters
    ----------
    labels : list of str, int, or None
        The label of each agent, by id: a stage name, a group (a whole number
        starting at 0), or None for no label.

    Returns
    -------
    Assign
        The placement.
    """
    labels = list(labels)
    for x in labels:
        if x is None or isinstance(x, str):
            continue
        if isinstance(x, Integral) and not isinstance(x, bool) and x >= 0:
            continue
        raise ValueError(
            "`labels` must be stages (str) or groups (whole numbers starting "
            "at 0), with None for none."
        )
    return _make("labels", labels=labels)


def by(fun: Callable) -> Assign:
    """A label from a function of the agent, at the start of each run.

    Parameters
    ----------
    fun : callable
        Function ``fun(agent, model)`` returning the agent's label: a stage
        name, a group number, or None for no label. It is called once per
        agent and run.

    Returns
    -------
    Assign
        The placement.
    """
    if not callable(fun):
        raise ValueError("`fun` must be a function of (agent, model).")
    return _make("by", fun=fun)


__all__ = [
    "Assign",
    "agents",
    "by",
    "count",
    "counts",
    "group_counts",
    "group_shares",
    "labels",
    "share",
    "shares",
]
