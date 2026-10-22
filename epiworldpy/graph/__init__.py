"""Networks for contact layers: generators, fixed graphs, and rewiring.

A generator (:func:`smallworld`, :func:`bernoulli`, :func:`sbm`) draws a new
network at the start of every run and replicate; a fixed graph
(:func:`edgelist`) gives the same ties every time. Either goes to
:func:`epiworldpy.layer.network`.
"""

from .. import _core

Generator = _core.graph.Generator
Graph = _core.graph.Graph
Rewiring = _core.graph.Rewiring
bernoulli = _core.graph.bernoulli
edgelist = _core.graph.edgelist
rewire_degseq = _core.graph.rewire_degseq
sbm = _core.graph.sbm
smallworld = _core.graph.smallworld

__all__ = [
    "Generator",
    "Graph",
    "Rewiring",
    "bernoulli",
    "edgelist",
    "rewire_degseq",
    "sbm",
    "smallworld",
]
