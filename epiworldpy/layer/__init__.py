"""Contact layers: where agents meet.

A model draws the contacts of its agents from its layers, all of them in one
draw. A network layer (:class:`Network`, from :func:`network`) holds ties
between agents; a mixing layer (:class:`Mixing`, from :func:`mixing`, also
created by the mixing models) draws contacts by group. Network models start
with no layers: add one with
:meth:`epiworldpy.Model.add_layer`.
"""

from .. import _core

ContactLayer = _core.layer.ContactLayer
Mixing = _core.layer.Mixing
Network = _core.layer.Network
mixing = _core.layer.mixing
network = _core.layer.network

__all__ = ["ContactLayer", "Mixing", "Network", "mixing", "network"]
