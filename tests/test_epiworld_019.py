"""Tests for the API added with epiworld 0.19: tools that target viruses."""

import numpy as np
import pytest

import epiworldpy as epiworld

SEED = 1


def make_two_virus_model():
    m = epiworld.Model()
    m.add_state("S", epiworld.UpdateFun.default_update_susceptible())
    infected = m.add_state("I", epiworld.UpdateFun.default_update_exposed())
    recovered = m.add_state("R")

    viruses = []
    for name in ("flu", "covid"):
        v = epiworld.Virus(name, 0.01, True, 0.5, 0.1, 0.0)
        v.set_state(infected, recovered, recovered)
        m.add_virus(v)
        viruses.append(v)

    m.agents_smallworld(n=2000, k=10, d=False, p=0.01)
    m.verbose_off()
    return m, viruses


def test_lineage_id():
    v = epiworld.Virus("v", 0.01, True, 0.5, 0.1, 0.0)
    assert v.get_lineage_id() == -99
    _, (flu, covid) = make_two_virus_model()
    assert flu.get_lineage_id() == flu.get_id() == 0
    assert covid.get_lineage_id() == covid.get_id() == 1


def test_targets_api():
    _, (flu, covid) = make_two_virus_model()
    tool = epiworld.Tool("Vaccine", 1.0, True, susceptibility_reduction=1.0)
    assert tool.get_targets() == []
    assert tool.targets(flu) and tool.targets(covid)

    tool.add_target(flu)
    assert tool.get_targets() == [0]
    assert tool.targets(flu) and not tool.targets(covid)

    tool.set_targets([0, 1])
    assert tool.get_targets() == [0, 1]
    tool.set_targets([])
    assert tool.get_targets() == []

    tool.add_target(1)
    assert tool.get_targets() == [1]
    tool.clear_targets()
    assert tool.get_targets() == []

    with pytest.raises(Exception):
        tool.add_target(63)
    with pytest.raises(Exception):
        tool.add_target(epiworld.Virus("v", 0.01, True, 0.5, 0.1, 0.0))


def test_targeted_tool_blocks_only_its_lineage():
    # Everyone gets a perfect vaccine against flu only, so only covid spreads.
    m, (flu, _) = make_two_virus_model()
    tool = epiworld.Tool("Vaccine", 1.0, True, susceptibility_reduction=1.0)
    tool.add_target(flu)
    m.add_tool(tool)
    m.run(30, SEED)

    tr = m.get_db().get_transmissions()
    # Seeded cases have no source (-1); keep agent-to-agent transmissions.
    viruses = np.asarray(tr["viruses"])[np.asarray(tr["sources"]) >= 0]
    assert np.sum(viruses == 0) == 0
    assert np.sum(viruses == 1) > 0
