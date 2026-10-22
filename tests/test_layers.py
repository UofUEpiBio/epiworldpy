"""Contact layers: network layers from generators and edge lists, the layer
objects shared with the model, mixing layers, several layers in one draw, and
the defunct functions."""

import numpy as np
import pytest

import epiworldpy as epiworld
import epiworldpy.epimodels as epimodels
from epiworldpy import graph, layer


def transmissions(model):
    """Infections by another agent (seeds excluded)."""
    tr = model.get_db().get_transmissions()
    keep = tr["sources"] >= 0
    return {k: np.asarray(v)[keep] for k, v in tr.items()}


def sir(n=1000):
    m = epimodels.ModelSIR(
        "flu", n=n, prevalence=0.05, transmission_rate=1, recovery_rate=0.1
    )
    m.verbose_off()
    return m


def test_network_layers():
    # Without layers, nobody meets anybody
    m = sir()
    m.run(20, 1)
    assert len(transmissions(m)["sources"]) == 0

    # A generator draws a new network every run; the same seed, the same one
    contacts = layer.network("contacts", graph.smallworld(k=4, p=0.2))
    assert isinstance(contacts, layer.Network)
    m.add_layer(contacts)
    with pytest.raises(Exception):
        m.add_layer(contacts)  # one model per layer

    m.run(20, 1)
    el1 = contacts.get_edgelist()
    assert len(transmissions(m)["sources"]) > 0
    assert len(el1["from"]) == 1000 * 4 // 2  # each undirected tie once

    m.run(20, 2)
    assert not np.array_equal(el1["to"], contacts.get_edgelist()["to"])

    m.run(20, 1)
    assert np.array_equal(el1["to"], contacts.get_edgelist()["to"])

    # get_layer() gives the same object, by name or by id
    assert m.get_layer("contacts") is contacts
    assert m.get_layer(0) is contacts
    with pytest.raises(Exception):
        m.get_layer("nope")


def test_fixed_network_edits_persist():
    m = epimodels.ModelSIS(
        "cold", n=4, prevalence=0.25, transmission_rate=0.5, recovery_rate=0.5
    )
    m.verbose_off()
    ring = layer.network("ring", graph.edgelist([0, 1, 2], [1, 2, 3], 4))
    m.add_layer(ring)

    assert ring.has_edge(0, 1) and ring.has_edge(1, 0)  # undirected
    assert not ring.has_edge(0, 3)

    # An edit between runs is kept
    assert ring.add_edge(0, 3)
    m.run(5, 1)
    assert ring.has_edge(0, 3)
    assert len(ring.get_edgelist()["from"]) == 4


def test_mixing_layer():
    cm = [9.0, 1.0, 1.0, 9.0]
    m = epimodels.ModelSEIRMixing(
        "flu", n=1000, prevalence=0.01, transmission_rate=0.1,
        avg_incubation_days=3, recovery_rate=0.2, contact_matrix=cm,
    )
    mixing = m.get_layer("mixing")
    assert isinstance(mixing, layer.Mixing)
    assert mixing.get_contact_matrix() == cm

    mixing.set_contact_matrix([1.0, 9.0, 9.0, 1.0])
    assert m.get_layer("mixing").get_contact_matrix() == [1.0, 9.0, 9.0, 1.0]


def test_several_layers_weights_and_targets():
    # Two network layers, drawn in one: transmissions come through both, and
    # each records the layer it came through.
    m = sir()
    a = layer.network("a", graph.smallworld(k=4, p=0.1))
    b = layer.network("b", graph.smallworld(k=4, p=0.1))
    m.add_layer(a)
    m.add_layer(b)

    m.run(20, 1)
    assert set(transmissions(m)["layers"]) == {a.get_id(), b.get_id()} == {0, 1}
    seeds = m.get_db().get_transmissions()
    assert set(np.asarray(seeds["layers"])[np.asarray(seeds["sources"]) < 0]) == {-1}

    # Weight 0 turns a layer off
    m.add_param(0.0, "a weight")
    a.set_weight("a weight")
    m.run(20, 1)
    assert set(transmissions(m)["layers"]) == {1}

    # A layer that targets another virus does not spread this one
    other = epiworld.Virus("other", 0.0, True, 0.5, 0.1, 0.0)
    other.set_state(1, 2, 2)  # Infected, Recovered
    m.add_virus(other)
    b.add_virus_target(other)
    m.run(20, 1)
    assert len(transmissions(m)["layers"]) == 0


def test_connected_models_ignore_layers():
    m = epimodels.ModelSIRCONN(
        "flu", n=1000, prevalence=0.05, contact_rate=0, transmission_rate=1,
        recovery_rate=0.1,
    )
    m.verbose_off()
    m.add_layer(layer.network("contacts", graph.smallworld(k=4, p=0.1)))
    m.run(10, 1)
    assert len(transmissions(m)["sources"]) == 0


@pytest.mark.parametrize(
    "call,replacement",
    [
        (lambda m: m.agents_smallworld(1000, 5, False, 0.01), "graph.smallworld"),
        (lambda m: m.agents_from_edgelist([0], [1], 2, False), "graph.edgelist"),
        (lambda m: m.add_edge(0, 1), "get_layer"),
        (lambda m: m.get_agent(0).get_neighbors(m), "get_edgelist"),
    ],
)
def test_defunct(call, replacement):
    with pytest.raises(RuntimeError, match="defunct.*" + replacement):
        call(sir())
