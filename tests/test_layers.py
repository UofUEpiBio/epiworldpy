"""Contact layers: network layers from generators and edge lists, the layer
objects shared with the model, mixing layers (across and within groups),
several layers in one draw, and the defunct functions."""

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


def test_mixing_within_groups():
    # 100 households of 10. With probability 1 the households are cliques:
    # carriers, transmitting for sure, infect their housemates in a day, and
    # nobody else.
    m = epimodels.ModelSIR(
        "flu", n=1000, prevalence=0.01, transmission_rate=1, recovery_rate=0
    )
    m.verbose_off()
    hh = layer.mixing(
        "households", [i // 10 for i in range(1000)], infectious_states=[1],
        available_states=[0, 1, 2], contact_prob=1,
    )
    assert isinstance(hh, layer.Mixing)
    m.add_layer(hh)

    def infected_vs_expected():
        m.run(1, 1)
        tr = m.get_db().get_transmissions()
        seeds = {t for s, t in zip(tr["sources"], tr["targets"]) if s < 0}
        reached = {hh.get_group(i) for i in seeds} - {None}
        expected = seeds | {i for i in range(1000) if hh.get_group(i) in reached}
        states = m.get_agents_states()
        return {i for i in range(1000) if states[i] == 1}, expected

    infected, expected = infected_vs_expected()
    assert infected == expected
    assert len(infected) > 10 * 5
    assert sorted(hh.members_of(1)) == list(range(10, 20))
    assert hh.get_contact_prob() == [1.0] * 100

    # Moves between runs are kept: everyone leaves the first 50 households
    for i in range(500):
        hh.set_group(i, None)
    assert hh.get_group(0) is None
    infected, expected = infected_vs_expected()
    assert infected == expected
    assert hh.members_of(0) == []

    # A probability set from a global event (at the end of each day) lasts for
    # that run only
    m.add_globalevent(
        lambda model: model.get_layer("households").set_contact_prob(0.0),
        "lockdown",
    )
    m.run(1, 1)
    assert hh.get_contact_prob() == [0.0] * 100
    m.rm_globalevent("lockdown")
    m.run(1, 1)
    assert hh.get_contact_prob() == [1.0] * 100

    # Probability .5 per housemate is the same run as 4.5 contacts a day with
    # the 9 housemates, from a diagonal contact matrix
    def same_run(**contacts):
        s = epimodels.ModelSIR(
            "flu", n=1000, prevalence=0.02, transmission_rate=0.3,
            recovery_rate=0.1
        )
        s.verbose_off()
        s.add_layer(layer.mixing(
            "households", [i // 10 for i in range(1000)], [1], [0, 1, 2],
            **contacts
        ))
        s.run(30, 2026)
        return list(s.get_agents_states())

    diag = [4.5 if i % 101 == 0 else 0.0 for i in range(100 * 100)]
    assert same_run(contact_prob=0.5) == same_run(contact_matrix=diag)

    with pytest.raises(ValueError, match="exactly one"):
        layer.mixing("x", [0, 1], [1], [0, 1, 2])
    with pytest.raises(TypeError):
        layer.mixing("x", [0, -1], [1], [0, 1, 2], contact_prob=1)
    with pytest.raises(TypeError):
        layer.mixing("x", [0, 0.5], [1], [0, 1, 2], contact_prob=1)
    with pytest.raises(ValueError):
        hh.get_group(1000)
    with pytest.raises(ValueError):
        hh.members_of(100)
    with pytest.raises(ValueError):
        hh.set_contact_prob(1.5)


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
