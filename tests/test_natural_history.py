"""Natural histories: a virus carries its stages and transitions; placement
(epiworldpy.assign); strains; values and durations; the outputs by status
and stage; the removed state API."""

import gc

import numpy as np
import pytest

import epiworldpy as epiworld
from epiworldpy import Duration, Model, Value, Virus, assign, epimodels, graph, layer


def sir_virus(name="Flu"):
    return (Virus(name)
            .add_stage("Susceptible")
            .add_stage("Infected", infectious=True)
            .add_stage("Recovered")
            .add_infection("Susceptible", "Infected",
                           rate="Transmission probability")
            .add_rate("Infected", "Recovered", rate="Recovery rate",
                      effect="recovery"))


def contacts(model, k=6, p=0.05):
    model.add_layer(layer.network("contacts", graph.smallworld(k=k, p=p)))
    model.verbose_off()
    return model


def test_virus_built_in_python_is_the_built_in_model():
    # The same parameters, stages and transitions give the same history, seed
    # by seed, as the registry's SIR and SEIR
    flu = sir_virus()
    assert flu.get_stages() == {
        "stage": ["Susceptible", "Infected", "Recovered"],
        "carrier": [False, True, False],
        "infectious": [False, True, False],
        "contact": [1.0, 1.0, 1.0],
    }

    built_in = contacts(epimodels.ModelSIR("Flu", n=2000, prevalence=0.01,
                                           transmission_prob=0.2,
                                           recovery_rate=0.1))
    from_scratch = Model(2000)
    from_scratch.add_param(0.1, "Recovery rate")
    from_scratch.add_param(0.2, "Transmission probability")
    from_scratch.add_virus(flu, assign.share(0.01))
    contacts(from_scratch)

    hist = []
    for m in (built_in, from_scratch):
        m.run(60, 2231)
        h = m.get_db().get_hist_total()
        hist.append((h["dates"].tolist(), list(h["stages"]["values"]),
                     h["stages"]["indexes"].tolist(), h["counts"].tolist()))
    assert hist[0] == hist[1]
    assert built_in.get_stages() == from_scratch.get_stages()
    assert from_scratch.get_agents_stages().count("Recovered") > 100

    # The SEIR's latent period is a duration
    seir = (Virus("Flu")
            .add_stage("Susceptible")
            .add_stage("Exposed", carrier=True)
            .add_stage("Infected", infectious=True)
            .add_stage("Removed")
            .add_infection("Susceptible", "Exposed",
                           rate="Transmission probability")
            .add_after("Exposed", "Infected",
                       Duration.geometric("Latent period"))
            .add_rate("Infected", "Removed", rate="Recovery rate",
                      effect="recovery"))
    built_in = contacts(epimodels.ModelSEIR("Flu", 2000, 0.01, 0.2, 4, 0.1))
    from_scratch = Model(2000)
    from_scratch.add_param(0.2, "Transmission probability")
    from_scratch.add_param(4, "Latent period")
    from_scratch.add_param(0.1, "Recovery rate")
    from_scratch.add_virus(seir, assign.share(0.01))
    contacts(from_scratch)
    for m in (built_in, from_scratch):
        m.run(60, 77)
    assert built_in.get_agents_stages() == from_scratch.get_agents_stages()

    # A fixed duration: the seeds (in Exposed on day 0, with no transmission)
    # all become infectious on day 4
    fixed = (Virus("Fixed")
             .add_stage("Susceptible")
             .add_stage("Exposed", carrier=True)
             .add_stage("Infected", infectious=True)
             .add_infection("Susceptible", "Exposed", rate=0)
             .add_after("Exposed", "Infected", Duration.fixed(4)))
    m = Model(100)
    m.add_virus(fixed, assign.count(10))
    m.verbose_off()
    m.run(6, 1)
    h = m.get_db().get_hist_total()
    infected = h["counts"][h["stages"]["indexes"] == 2]
    assert infected.tolist() == [0, 0, 0, 0, 10, 10, 10]

    # Stages are named by their labels, and checked
    with pytest.raises(ValueError, match='no stage "Dead"'):
        flu.add_rate("Infected", "Dead", rate=0.1)
    with pytest.raises(ValueError, match="effect"):
        sir_virus().add_rate("Infected", "Recovered", rate=0.1, effect="x")
    with pytest.raises(ValueError, match="carrier"):
        Virus("x").add_stage("X", infectious=True, carrier=False)
    with pytest.raises(TypeError, match="A value is"):
        sir_virus().add_rate("Infected", "Recovered", rate=None)
    with pytest.raises(RuntimeError, match="natural history"):
        Model(10).add_virus(Virus("empty"))
    # The natural history of a virus in a model cannot change
    with pytest.raises(RuntimeError, match="is in a model"):
        built_in.get_virus(0).add_stage("Other")


def test_transitions_by_name_strains_and_values():
    # The outputs count each transition by name, and fire() applies one
    vax = sir_virus("Measles").add_external("Susceptible", "Recovered",
                                            "Vaccination")
    m = Model(100)
    m.add_param(0.1, "Recovery rate")
    m.add_param(0.0, "Transmission probability")
    m.add_virus(vax, assign.agents(range(5)))
    m.verbose_off()
    m.run(5, 1)
    names = list(m.get_db().get_hist_transitions()["transitions"]["values"])
    assert names == ["Susceptible -> Infected", "Infected -> Recovered",
                     "Vaccination"]
    assert m.fire("Vaccination", 50)
    assert not m.fire("Vaccination", 0)  # infected, not susceptible
    with pytest.raises(Exception, match="Vaccinate"):
        m.fire("Vaccinate", 50)
    assert "Vaccination" in m.draw_natural_history()

    # A strain is a copy of the model's virus with its own name; this one reads
    # another parameter (use_param()) and does not transmit, so only its seeds
    # ever carry it
    model = contacts(epimodels.ModelSIR("A virus", 1000, 0.01, 0.3, 0.1))
    strain = model.get_virus(0).clone()
    strain.set_name("B")
    model.add_param(0.0, "B transmission")
    strain.use_param("Transmission probability", "B transmission")
    model.add_virus(strain, assign.count(20))
    model.run(30, 12)

    hv = model.get_db().get_hist_virus()
    ids, stages, counts = hv["ids"], hv["stages"], hv["counts"]
    carriers_b = counts[(ids == 1) & (stages["indexes"] != 0)]
    by_day = np.bincount(hv["dates"][(ids == 1) & (stages["indexes"] != 0)],
                         weights=carriers_b)
    assert by_day.max() <= 20
    assert counts[(ids == 0)].max() > 20
    assert model.get_virus(1).get_name() == "B"
    assert model.get_virus(1).get_lineage_id() == 1

    # Values: 1 - parameter, 1 / parameter, and a Python function
    v = (Virus("Values")
         .add_stage("Susceptible")
         .add_stage("Infected", infectious=True)
         .add_stage("Recovered")
         .add_infection("Susceptible", "Infected",
                        rate=Value.complement("Efficacy"))
         .add_rate("Infected", "Recovered",
                   rate=Value.inverse("Infectious period")))
    m = contacts(Model(500))
    m.add_param(1.0, "Efficacy")  # 1 - 1: nobody is infected
    m.add_param(1.0, "Infectious period")  # 1 / 1: all recover the next day
    m.add_virus(v, assign.count(50))
    m.run(3, 1)
    assert m.get_agents_stages().count("Recovered") == 50
    assert m.get_agents_stages().count("Susceptible") == 450

    calls = []

    def recover(agent, model):
        calls.append(agent.get_id())
        return 1.0

    v = (Virus("Python function")
         .add_stage("Susceptible")
         .add_stage("Infected", infectious=True)
         .add_stage("Recovered")
         .add_infection("Susceptible", "Infected", rate=0)
         .add_rate("Infected", "Recovered", rate=Value.fun(recover)))
    m = Model(100)
    m.add_virus(v, assign.agents(range(10)))
    m.verbose_off()
    m.run(2, 1)
    assert sorted(calls) == list(range(10))  # once per agent, the day it recovers
    assert m.get_agents_stages().count("Recovered") == 10

    # A stage with contact weight 0 is out of every layer: its agents do not
    # transmit
    away = sir_virus("Away").set_contact("Infected", 0)
    assert away.get_stages()["contact"] == [1.0, 0.0, 1.0]
    m = contacts(Model(1000))
    m.add_param(0.0, "Recovery rate")
    m.add_param(1.0, "Transmission probability")
    m.add_virus(away, assign.count(10))
    m.run(10, 1)
    assert m.get_agents_stages().count("Infected") == 10


def test_placement():
    vaccine = epiworld.Tool("Vaccine", susceptibility_reduction=1.0)

    # A subset to draw from is a group of a mixing layer: 30 of agents
    # 900..999 get the virus, and half of them the tool
    m = epimodels.ModelSIR("a virus", n=1000, prevalence=0,
                           transmission_prob=0, recovery_rate=0)
    m.verbose_off()
    m.add_layer(layer.mixing("subset", [None] * 900 + [0] * 100,
                             contact_prob=0.0))
    m.assign_virus(0, assign.group_counts("subset", [30]))
    m.add_tool(vaccine, assign.group_shares("subset", [0.5]))
    m.run(0, 123)
    stages = m.get_agents_stages()
    infected = [i for i, s in enumerate(stages) if s == "Infected"]
    assert len(infected) == 30 and min(infected) >= 900
    assert m.get_db().get_hist_tool()["counts"].sum() == 50

    # Fixed labels, and labels from a function (once per agent and run)
    m.assign_virus(0, assign.labels(["Infected"] * 5 + [None] * 995))
    m.set_initial(assign.by(lambda agent, model: None))
    m.run(0, 1)
    stages = m.get_agents_stages()
    assert [i for i, s in enumerate(stages) if s == "Infected"] == list(range(5))
    assert "one label per agent (1)" in repr(assign.labels([None]))

    # The placement keeps its own copy of the stages: a function's stage
    # names are resolved after the virus given to add_virus() is gone
    def place_flu(model):
        flu = (Virus("Flu")
               .add_stage("Susceptible")
               .add_stage("Infected", infectious=True)
               .add_infection("Susceptible", "Infected", rate=0))
        model.add_virus(flu, assign.by(lambda agent, model: "Infected"))

    m = Model(200)
    place_flu(m)
    gc.collect()
    m.verbose_off()
    m.run(0, 1)
    assert m.get_agents_stages() == ["Infected"] * 200

    # A stage that cannot hold the virus is not a seed's stage
    m = epimodels.ModelSIR("a virus", n=100, prevalence=0,
                           transmission_prob=0, recovery_rate=0)
    m.assign_virus(0, assign.counts({"Recovered": 10}))
    with pytest.raises(Exception, match="carrier|virus"):
        m.run(0, 1)

    with pytest.raises(TypeError):
        m.add_tool(vaccine, 0.5)


def test_registry():
    ids = [info["id"] for info in epimodels.list_models()]
    assert {"SIR", "SEIR", "SEIRD", "SIRD", "SIS", "SISD", "SURV",
            "SEIRQuarantine"} <= set(ids)
    info = epimodels.get_model_info("SEIR")
    assert [p["arg"] for p in info["params"]] == [
        "n", "prevalence", "transmission_prob", "latent_period",
        "recovery_rate",
    ]

    # Each named constructor is make_model() with the same arguments
    named = epimodels.ModelSEIR("Flu", n=1000, prevalence=0.02,
                                transmission_prob=0.2, latent_period=3,
                                recovery_rate=0.1)
    made = epimodels.make_model(
        "SEIR",
        {"n": 1000, "prevalence": 0.02, "transmission_prob": 0.2,
         "latent_period": 3, "recovery_rate": 0.1},
        virus_name="Flu",
    )
    for m in (named, made):
        contacts(m, k=8, p=0.1)
        m.run(50, 1)
    assert named.get_agents_stages() == made.get_agents_stages()
    assert named.get_virus(0).get_name() == "Flu"
    assert isinstance(made, Model)

    with pytest.raises(ValueError, match="SIRS|known|Unknown|unknown"):
        epimodels.make_model("SIRS")
    with pytest.raises(ValueError, match="transmission_prob"):
        epimodels.make_model("SIR", {"transmission_prob": 2.0})


@pytest.mark.parametrize(
    "call,replacement",
    [
        (lambda m: m.add_state("S"), "add_stage"),
        (lambda m: m.get_states(), "get_stages"),
        (lambda m: m.get_agents_states(), "get_agents_stages"),
        (lambda m: m.initial_states([0.5]), "set_initial"),
        (lambda m: m.set_infectious_states([1]), "infectious=True"),
        (lambda m: m.get_agent(0).get_state(), "get_stage"),
        (lambda m: m.get_agent(0).change_state(m, 1), "fire"),
        (lambda m: m.get_agent(0).rm_virus(m), "add_external"),
        (lambda m: Virus("v").set_prob_infecting(0.1), "add_infection"),
        (lambda m: Virus("v").set_distribution(None), "add_virus"),
        (lambda m: epiworld.Tool("t").set_distribution(None), "add_tool"),
        (lambda m: epiworld.UpdateFun.susceptible(), "add_stage"),
        (lambda m: epiworld.UpdateFun(), "add_stage"),
    ],
)
def test_defunct(call, replacement):
    m = epimodels.ModelSIR("flu", 100, 0.01, 0.1, 0.1)
    with pytest.raises(RuntimeError, match="defunct.*" + replacement):
        call(m)


def test_old_constructors_are_rejected():
    with pytest.raises(TypeError, match="takes only a name.*add_virus"):
        Virus("Flu", 0.01, True, 0.5, 0.1, 0.0)
    with pytest.raises(TypeError, match="takes only a name.*add_tool"):
        epiworld.Tool("Vaccine", 0.5, True, susceptibility_reduction=0.9)
