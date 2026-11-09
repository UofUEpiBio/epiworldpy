"""Groupings for mixing layers: removed in epiworldpy 1.0.0.

A mixing layer's groups drawn at every run are a placement whose labels are
the groups (see :mod:`epiworldpy.assign`). Calling :func:`random` raises with
its replacement.
"""

from .._defunct import grouping_stub

random = grouping_stub("random")

__all__ = ["random"]
