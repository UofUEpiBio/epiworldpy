"""Layer participation, closures, and tools restricted to named layers."""

import numpy as np
import pytest

import epiworldpy as epiworld
import epiworldpy.epimodels as epimodels
from epiworldpy import assign, graph, layer


def sir():
    """Build the two-layer SIR used for participation checks."""
    m = epimodels.ModelSIR(
        "flu", n=1000, prevalence=0.02, transmission_prob=0.5,
        recovery_rate=0.2,
    )
    m.verbose_off()
    m.add_layer(layer.network("home", graph.smallworld(k=4, p=0.1)))
    m.add_layer(layer.network("work", graph.smallworld(k=6, p=0.2)))
    assert m.get_layer("home").get_id() == 0
    assert m.get_layer("work").get_id() == 1
    return m


def transmissions(model):
    """Return transmission layers, excluding infections seeded by code."""
    tr = model.get_db().get_transmissions()
    return np.asarray(tr["layers"])[np.asarray(tr["sources"]) >= 0]


@pytest.mark.parametrize("mode", ["pull", "push"])
def test_mask_at_work_only(mode):
    m = sir()
    m.set_transmission_mode(mode)
    mask = epiworld.Tool("Mask", susceptibility_reduction=1.0)
    assert mask.add_layer_target("work") is mask
    m.add_tool(mask, assign.share(1.0))

    m.run(30, 99)
    layers = transmissions(m)
    assert np.count_nonzero(layers == 1) == 0
    assert np.count_nonzero(layers == 0) > 50


@pytest.mark.parametrize("mode", ["pull", "push"])
def test_mask_everywhere(mode):
    m = sir()
    m.set_transmission_mode(mode)
    mask = epiworld.Tool("Mask", susceptibility_reduction=1.0)
    assert mask.get_layer_targets() == []
    m.add_tool(mask, assign.share(1.0))

    m.run(30, 99)
    assert len(transmissions(m)) == 0


def test_layer_target_getters():
    mask = epiworld.Tool("Mask", susceptibility_reduction=1.0)
    assert mask.get_layer_targets() == []
    assert mask.add_layer_target("work") is mask
    mask.add_layer_target("work")
    assert mask.get_layer_targets() == ["work"]

    # Replacing targets drops old names and duplicates, keeping their order.
    mask.set_layer_targets(["home", "home", "work"])
    assert mask.get_layer_targets() == ["home", "work"]
    mask.set_layer_targets(["work"])
    assert mask.get_layer_targets() == ["work"]
    mask.set_layer_targets([])
    assert mask.get_layer_targets() == []
    mask.add_layer_target("work")
    mask.clear_layer_targets()
    assert mask.get_layer_targets() == []

    # Cleared targets really protect through every layer when the model runs.
    m = sir()
    m.add_tool(mask, assign.share(1.0))
    m.run(30, 99)
    assert len(transmissions(m)) == 0


@pytest.mark.parametrize("mode", ["pull", "push"])
def test_exclude_status(mode):
    m = sir()
    m.set_transmission_mode(mode)
    work = m.get_layer("work")
    assert work.get_excluded_statuses() == []
    assert work.exclude_status("Free") is work
    work.exclude_status("Free")
    assert work.get_excluded_statuses() == ["Free"]

    m.run(30, 99)
    layers = transmissions(m)
    assert np.count_nonzero(layers == 1) == 0
    assert np.count_nonzero(layers == 0) > 0


def test_close_and_reopen():
    m = sir()
    work = m.get_layer("work")
    work.enable_tracing()
    assert not work.is_closed()
    work.close()
    assert work.is_closed()

    # A closure set between runs persists, and logs no contacts in either run.
    for _ in range(2):
        m.run(30, 99)
        assert work.is_closed()
        layers = transmissions(m)
        assert np.count_nonzero(layers == 1) == 0
        assert np.count_nonzero(layers == 0) > 0
        assert len(work.get_tracing()["source"]) == 0

    work.reopen()
    assert not work.is_closed()
    m.run(30, 99)
    assert np.count_nonzero(transmissions(m) == 1) > 0
    assert len(work.get_tracing()["source"]) > 0


def test_closure_during_run():
    m = sir()

    def close_work(model):
        """Close after day one; exclusions cannot change during a run."""
        work = model.get_layer("work")
        if model.today() == 1:
            with pytest.raises(RuntimeError, match="between runs"):
                work.exclude_status("Free")
            assert not work.is_closed()
            work.close()
            assert not work.is_closed()  # takes effect the next day
        else:
            assert work.is_closed()

    m.add_globalevent(close_work, "close work")
    m.run(30, 99)
    tr = m.get_db().get_transmissions()
    at_work = ((np.asarray(tr["sources"]) >= 0)
               & (np.asarray(tr["layers"]) == 1))
    assert np.count_nonzero(at_work) > 0
    assert set(np.asarray(tr["dates"])[at_work]) == {1}

    # A closure during a run leaves the between-run setting open.
    m.rm_globalevent("close work")
    m.run(30, 99)
    assert not m.get_layer("work").is_closed()
    assert np.count_nonzero(transmissions(m) == 1) > 0


def test_mixing_inherits_participation():
    m = epimodels.ModelSIR(
        "flu", n=1000, prevalence=0.02, transmission_prob=0.5,
        recovery_rate=0.2,
    )
    m.verbose_off()
    work = layer.mixing("work", [0] * 1000, contact_prob=0.01)
    m.add_layer(work)
    work.close()
    assert work.is_closed()
    m.run(30, 99)
    assert len(transmissions(m)) == 0

    work.reopen()
    assert not work.is_closed()
    m.run(30, 99)
    assert len(transmissions(m)) > 0
    assert work.exclude_status("Free") is work
    assert work.get_excluded_statuses() == ["Free"]
    m.run(30, 99)
    assert len(transmissions(m)) == 0


def test_unknown_layer_target():
    m = sir()
    mask = epiworld.Tool("Mask", susceptibility_reduction=1.0)
    mask.set_layer_targets(["missing"])
    m.add_tool(mask, assign.share(1.0))
    with pytest.raises((ValueError, RuntimeError), match='targets the layer "missing"'):
        m.run(30, 99)


def test_unknown_excluded_status():
    m = sir()
    m.get_layer("work").exclude_status("missing")
    with pytest.raises(ValueError, match='excludes the status "missing"'):
        m.run(30, 99)
