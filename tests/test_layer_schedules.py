"""Layer schedules, persistence in mixing layers, and parameter cycles."""

import numpy as np
import pytest

import epiworldpy as epiworld
import epiworldpy.epimodels as epimodels
from epiworldpy import graph, layer


def sir(n=1000, transmission_prob=0.3, recovery_rate=0.1):
    m = epimodels.ModelSIR(
        "flu", n=n, prevalence=0.02, transmission_prob=transmission_prob,
        recovery_rate=recovery_rate,
    )
    m.verbose_off()
    return m


def transmissions(model):
    """Return (layers, dates) of transmissions not seeded by code."""
    tr = model.get_db().get_transmissions()
    keep = np.asarray(tr["sources"]) >= 0
    return (
        np.asarray(tr["layers"])[keep],
        np.asarray(tr["dates"])[keep],
        np.asarray(tr["sources"])[keep],
        np.asarray(tr["targets"])[keep],
    )


def test_schedule():
    m = sir()
    m.add_layer(layer.network("home", graph.smallworld(k=4, p=0.1)))
    work = layer.network("work", graph.smallworld(k=6, p=0.1))
    m.add_layer(work)
    assert not work.has_schedule()
    assert work.set_schedule([True] * 5 + [False] * 2) is work
    assert work.has_schedule()
    assert work.is_scheduled(5)
    assert not work.is_scheduled(6)
    assert work.is_scheduled(8)

    m.run(40, 11)
    layers, dates, _, _ = transmissions(m)
    weekend = (dates - 1) % 7 >= 5
    assert np.count_nonzero((layers == 1) & weekend) == 0
    assert np.count_nonzero(layers == 1) > 10
    assert np.count_nonzero((layers == 0) & weekend) > 0

    assert work.clear_schedule() is work
    assert not work.has_schedule()
    with pytest.raises(Exception):
        work.set_schedule([])


def test_persistence():
    n = 3000
    m = sir(n=n)
    transit = layer.mixing(
        "transit", [i // 60 for i in range(n)], contact_prob=0.05
    )
    m.add_layer(layer.network("home", graph.smallworld(k=4, p=0.1)))
    m.add_layer(transit)
    pools = [g // 5 for g in range(50)]
    assert transit.set_pools(pools) is transit
    assert transit.set_persistence(0.0) is transit
    assert transit.get_pools() == pools
    assert transit.get_persistence() == [0.0]

    m.run(30, 11)
    layers, _, src, tgt = transmissions(m)
    sel = layers == 1
    assert np.count_nonzero(sel) > 10
    route = lambda a: (a // 60) // 5
    assert np.all(route(src[sel]) == route(tgt[sel]))
    assert np.count_nonzero((src[sel] // 60) != (tgt[sel] // 60)) > 0
    assert transit.get_usual_group(0) == 0

    assert transit.clear_persistence() is transit
    assert transit.get_persistence() == []
    with pytest.raises(Exception):
        transit.set_persistence(1.5)


def cycle_model(values, cycle):
    m = sir()
    m.add_param(2.0, "Public rate")
    public = layer.mixing("public", [0] * 1000, contact_matrix=[1.0])
    public.set_contact_rate("Public rate")
    m.add_layer(public)
    if cycle:
        m.set_param_cycle("Public rate", values)
    else:
        def set_rate(model):
            model.set_param("Public rate", values[model.today() % 7])

        m.add_globalevent(set_rate, "weekly rate", -1)
    return m


def test_param_cycle():
    values = [2, 2, 2, 2, 2, 5, 5]
    a = cycle_model(values, True)
    b = cycle_model(values, False)
    assert a.has_param_cycle("Public rate")
    assert not b.has_param_cycle("Public rate")
    other = next(k for k in a.params() if k != "Public rate")
    assert not a.has_param_cycle(other)

    a.run(30, 7)
    b.run(30, 7)
    ta, tb = transmissions(a), transmissions(b)
    assert len(ta[0]) > 50
    for x, y in zip(ta, tb):
        assert np.array_equal(x, y)

    a.clear_param_cycle("Public rate")
    assert not a.has_param_cycle("Public rate")
    with pytest.raises(Exception):
        a.set_param_cycle("Public rate", [])
    with pytest.raises(Exception):
        a.set_param_cycle("No such parameter", [1.0])
