"""Contact layers: network layers from generators and edge lists, the layer
objects shared with the model, mixing layers (across and within groups),
several layers in one draw, and the defunct functions."""

import numpy as np
import pytest

import epiworldpy as epiworld
import epiworldpy.epimodels as epimodels
from epiworldpy import graph, grouping, layer


def transmissions(model):
    """Infections by another agent (seeds excluded)."""
    tr = model.get_db().get_transmissions()
    keep = tr["sources"] >= 0
    return {k: np.asarray(v)[keep] for k, v in tr.items()}


def sir(n=1000):
    m = epimodels.ModelSIR(
        "flu", n=n, prevalence=0.05, transmission_prob=1, recovery_rate=0.1
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
        "cold", n=4, prevalence=0.25, transmission_prob=0.5, recovery_rate=0.5
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
    m = epimodels.ModelSEIR(
        "flu", n=1000, prevalence=0.01, transmission_prob=0.1,
        latent_period=3, recovery_rate=0.2,
    )
    m.add_layer(layer.mixing("mixing", [i // 500 for i in range(1000)],
                             contact_matrix=cm))
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
        "flu", n=1000, prevalence=0.01, transmission_prob=1, recovery_rate=0
    )
    m.verbose_off()
    hh = layer.mixing(
        "households", [i // 10 for i in range(1000)], contact_prob=1,
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

    # A traced layer logs its contacts: every transmission is in the log
    assert not hh.is_tracing()
    with pytest.raises(RuntimeError, match="not traced"):
        hh.get_tracing()
    hh.enable_tracing(20)
    assert hh.is_tracing()
    m.run(1, 1)
    log = hh.get_tracing()
    assert sorted(log) == ["contact", "day", "source"]
    logged = set(zip(log["source"].tolist(), log["contact"].tolist()))
    tr = transmissions(m)
    assert len(tr["sources"]) > 0
    assert set(zip(tr["sources"].tolist(), tr["targets"].tolist())) <= logged

    # ...and keeps who shared a group: agent 500's nine housemates on day 1
    mates = hh.groupmates_of(500)
    assert sorted(mates) == ["agent", "last_day", "n_days"]
    assert mates["agent"].tolist() == list(range(501, 510))
    assert set(mates["last_day"].tolist()) == {1}
    assert set(mates["n_days"].tolist()) == {1}

    # The history covers whole runs: a move between runs drops it
    hh.set_group(501, 0)
    with pytest.raises(RuntimeError, match="no history"):
        hh.groupmates_of(500)
    hh.set_group(501, 50)
    hh.disable_tracing()
    assert not hh.is_tracing()

    # Probability .5 per housemate is the same run as 4.5 contacts a day with
    # the 9 housemates, from a diagonal contact matrix
    def same_run(**contacts):
        s = epimodels.ModelSIR(
            "flu", n=1000, prevalence=0.02, transmission_prob=0.3,
            recovery_rate=0.1
        )
        s.verbose_off()
        s.add_layer(layer.mixing(
            "households", [i // 10 for i in range(1000)], **contacts
        ))
        s.run(30, 2026)
        return list(s.get_agents_states())

    diag = [4.5 if i % 101 == 0 else 0.0 for i in range(100 * 100)]
    assert same_run(contact_prob=0.5) == same_run(contact_matrix=diag)

    with pytest.raises(ValueError, match="exactly one"):
        layer.mixing("x", [0, 1])
    with pytest.raises(TypeError):
        layer.mixing("x", [0, -1], contact_prob=1)
    with pytest.raises(TypeError):
        layer.mixing("x", [0, 0.5], contact_prob=1)
    with pytest.raises(ValueError):
        hh.get_group(1000)
    with pytest.raises(ValueError):
        hh.members_of(100)
    with pytest.raises(ValueError):
        hh.set_contact_prob(1.5)


def test_grouping_random():
    # Groups drawn at random at every run: the sizes hold (agents left over
    # are in no group), the same seed draws the same groups and another seed
    # others, and a move between runs is an error
    m = epimodels.ModelSIR(
        "flu", n=1000, prevalence=0.01, transmission_prob=0.2,
        recovery_rate=0.1
    )
    m.verbose_off()
    schools = layer.mixing(
        "schools", grouping.random([300, 200]), contact_prob=0.05
    )
    halves = layer.mixing(
        "halves", grouping.random([0.5, 0.5], as_proportion=True),
        contact_prob=0.05
    )
    m.add_layer(schools)
    m.add_layer(halves)

    def draw(seed):
        m.run(10, seed)
        return [schools.get_group(i) for i in range(1000)]

    first = draw(1)
    assert [first.count(g) for g in (0, 1, None)] == [300, 200, 500]
    assert draw(1) == first
    assert draw(2) != first
    with pytest.raises(RuntimeError, match="drawn at every reset"):
        schools.set_group(0, 1)

    # Proportions are of the agents still without a group
    assert [len(halves.members_of(g)) for g in (0, 1)] == [500, 250]
    with pytest.raises(ValueError, match="non-negative"):
        grouping.random([-1])


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


def test_contact_rate_and_infectious_states():
    # Random mixing: "Contact rate" is read every day, so a global event that
    # sets it to 0 on day 10 stops new infections from day 11 on
    m = epimodels.ModelSIR(
        "flu", n=2000, prevalence=0.01, transmission_prob=0.1,
        recovery_rate=1 / 14,
    )
    m.verbose_off()
    m.add_param(5.0, "Contact rate")
    everyone = layer.mixing("everyone", [0] * 2000, contact_matrix=[1.0])
    everyone.set_contact_rate("Contact rate")
    m.add_layer(everyone)
    m.add_globalevent(
        lambda model: model.set_param("Contact rate", 0.0)
        if model.today() == 10 else None,
        "lockdown",
    )
    m.run(30, 2211)
    assert max(transmissions(m)["dates"]) == 10

    # Exposed agents that never leave Exposed infect nobody, until every state
    # is made infectious
    s = epimodels.ModelSEIR(
        "flu", n=2000, prevalence=0.01, transmission_prob=0.5,
        latent_period=1e9, recovery_rate=0,
    )
    s.verbose_off()
    assert s.get_infectious_states() == [2]
    s.add_layer(layer.mixing("everyone", [0] * 2000, contact_matrix=[5.0]))
    s.run(5, 1)
    assert len(transmissions(s)["sources"]) == 0
    s.set_infectious_states([])
    s.run(5, 1)
    assert len(transmissions(s)["sources"]) > 0

    # A contact rate needs a contact matrix
    hh = layer.mixing("hh", [0] * 2000, contact_prob=1)
    hh.set_contact_rate("Contact rate")
    m2 = epimodels.ModelSIR("flu", n=2000, prevalence=0.01,
                            transmission_prob=0.1, recovery_rate=0.1)
    m2.add_param(1.0, "Contact rate")
    m2.add_layer(hh)
    with pytest.raises(RuntimeError, match="contact rate"):
        m2.run(1, 1)


def test_seir_quarantine_traces_every_layer():
    # One carrier in a ring network and a household of four is detected the
    # day it becomes infectious; nobody is infected (transmission probability
    # 0), but its contacts that day are in the logs, so its ring neighbors and
    # housemates are quarantined, and nobody else.
    n = 40
    m = epimodels.ModelSEIRQuarantine(
        "flu", n=n, prevalence=1 / n, transmission_prob=0.0, latent_period=1.0,
        recovery_rate=0.0, hospitalization_rate=0.0, hospitalization_period=7,
        detection_delay=0.0, quarantine_period=14, quarantine_willingness=1.0,
        isolation_willingness=1.0, isolation_period=14,
    )
    m.verbose_off()
    ring = layer.network(
        "ring", graph.edgelist(list(range(n)), [(i + 1) % n for i in range(n)],
                               n, False)
    )
    hh = layer.mixing("households", [i // 4 for i in range(n)], contact_prob=1)
    m.add_layer(ring)
    m.add_layer(hh)
    m.run(3, 1)

    assert ring.is_tracing() and hh.is_tracing()
    carrier = int(m.get_db().get_transmissions()["targets"][0])
    expected = {(carrier - 1) % n, (carrier + 1) % n}
    expected |= {i for i in range(n) if i // 4 == carrier // 4} - {carrier}

    states = m.get_agents_states()
    labels = m.get_states()
    assert labels[states[carrier]] == "Isolated"
    quarantined = {i for i in range(n)
                   if labels[states[i]] == "Quarantined Susceptible"}
    assert quarantined == expected
    assert m.get_param("Detection delay") == 0.0


@pytest.mark.parametrize(
    "name,replacement",
    [
        ("ModelSIRCONN", "set_contact_rate"),
        ("ModelSEIRMixing", "layer.mixing"),
        ("ModelSEIRNetworkQuarantine", "ModelSEIRQuarantine"),
    ],
)
def test_defunct_models(name, replacement):
    with pytest.raises(RuntimeError, match="defunct.*" + replacement):
        getattr(epimodels, name)("flu", 1000, 0.01, 2.0, 0.5, 0.1)
    with pytest.raises(ValueError, match="set_infectious_states"):
        epiworld.UpdateFun.susceptible(exclude=[1])


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
