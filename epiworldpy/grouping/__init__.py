"""Groupings for mixing layers: groups drawn afresh at the start of every run.

A grouping (:func:`random`) goes to :func:`epiworldpy.layer.mixing` in place
of fixed groups, as a network generator goes to
:func:`epiworldpy.layer.network`.
"""

from .. import _core

Grouping = _core.grouping.Grouping
random = _core.grouping.random

__all__ = ["Grouping", "random"]
