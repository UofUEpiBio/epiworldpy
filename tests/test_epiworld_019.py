"""Disease targets, ported from epiworldR's test-tool-targets.R."""

import pytest

import epiworldpy as epiworld
from epiworldpy import assign, epimodels, graph, layer


def make_two_virus_model():
    m = epimodels.ModelSIR("flu", n=1000, prevalence=0.02,
                           transmission_prob=0.5, recovery_rate=0.2)
    covid = (epiworld.Virus("covid").add_stage("Susceptible")
             .add_stage("Infected", infectious=True).add_stage("Recovered")
             .add_infection("Susceptible", "Infected", rate=0.5)
             .add_rate("Infected", "Recovered", rate=0.2))
    m.add_virus(covid, assign.count(20))
    m.add_param(4.0, "Contact rate")
    contacts = layer.mixing("everyone", [0] * 1000, contact_matrix=[1.0])
    contacts.set_contact_rate("Contact rate")
    m.add_layer(contacts)
    m.verbose_off()
    return m, (m.get_virus(0), m.get_virus(1))


def vaccine():
    return epiworld.Tool("Perfect vaccine", susceptibility_reduction=1.0)


def n_transmissions(model, disease):
    return (model.get_db().get_transmissions(disease=disease)["sources"] >= 0).sum()


def test_lineage_id():
    # The removed feature remains a stub naming its replacement.
    with pytest.raises(RuntimeError, match="defunct.*add_target"):
        epiworld.Virus("v").get_lineage_id()
    contacts = layer.network("contacts", graph.smallworld(k=4, p=0.1))
    with pytest.raises(RuntimeError, match="defunct.*add_target"):
        contacts.add_virus_target(0)


def test_targets_api():
    tool = vaccine()
    assert tool.get_targets() == []
    assert tool.add_target("flu") is tool
    tool.add_target("covid")
    assert tool.get_targets() == ["flu", "covid"]
    assert tool.set_targets(["covid"]) is tool
    assert tool.get_targets() == ["covid"]
    tool.set_targets(None)
    assert tool.get_targets() == []
    tool.set_targets(["flu", "covid"])
    assert tool.clear_targets() is tool
    assert tool.get_targets() == []
    tool.set_targets([])
    for bad in (0, 63, epiworld.Virus("v"), ["flu", "covid"], None):
        with pytest.raises(TypeError, match="disease's name"):
            tool.add_target(bad)
    for bad in ([1, 2], ["flu", None], "flu", epiworld.Virus("v")):
        with pytest.raises(TypeError, match="disease"):
            tool.set_targets(bad)


@pytest.mark.parametrize("target,other", [("flu", "covid"), ("covid", "flu")])
def test_targeted_tool_blocks_only_its_disease(target, other):
    m, viruses = make_two_virus_model()
    tool = vaccine().add_target(target)
    m.add_tool(tool, assign.share(1.0))
    assert m.get_tool(0).get_targets() == [target]
    m.run(20, 1231)
    assert n_transmissions(m, target) == 0
    assert n_transmissions(m, other) > 0
    assert len(m.get_db().get_hist_tool(disease=other)["counts"]) == 0
    assert len(m.get_db().get_hist_tool(disease=target)["counts"]) > 0
    for v in viruses:
        assert m.get_tool(0).targets(v) == (v.get_name() == target)


def test_missing_or_unknown_targets():
    for target in (None, "unknown"):
        m, _ = make_two_virus_model()
        tool = vaccine()
        if target is not None:
            tool.add_target(target)
        m.add_tool(tool, assign.share(1.0))
        with pytest.raises((ValueError, RuntimeError), match="target|unknown"):
            m.run(5, 1)


def test_one_disease_default_targets():
    m = epimodels.ModelSIR("flu", 1000, 0.02, 0.5, 0.2)
    m.add_layer(layer.network("contacts", graph.smallworld(k=6, p=0.1)))
    m.verbose_off()
    m.add_tool(vaccine(), assign.share(1.0))
    m.run(20, 1231)
    assert n_transmissions(m, "flu") == 0


def test_targets_survive_run_multiple():
    m, _ = make_two_virus_model()
    m.add_tool(vaccine().add_target("flu"), assign.share(1.0))
    results = []

    def save(sim, model):
        results.append((n_transmissions(model, "flu"),
                        n_transmissions(model, "covid")))

    m.run_multiple(ndays=20, nexperiments=2, seed_=1231, fun=save,
                   nthreads=1, verbose=False)
    assert len(results) == 2
    assert all(a == 0 and b > 0 for a, b in results)
