"""Tests for the API added with epiworld 0.16/0.17, and for wrapper fixes."""

import numpy as np
import pytest

import epiworldpy as epiworld
import epiworldpy.epimodels as epimodels
from epiworldpy import assign, graph, layer

DAYS = 30
SEED = 1


def sir_virus(name):
    """A SIR natural history, reading two model parameters."""
    return (epiworld.Virus(name)
            .add_stage("Susceptible")
            .add_stage("Infected", infectious=True)
            .add_stage("Recovered")
            .add_infection("Susceptible", "Infected",
                           rate="Transmission probability")
            .add_rate("Infected", "Recovered", rate="Recovery rate",
                      effect="recovery"))


def make_sir(n=2000):
    m = epimodels.ModelSIR(
        name="flu", n=n, prevalence=0.01, transmission_prob=0.3,
        recovery_rate=0.1
    )
    m.add_layer(layer.network("contacts", graph.smallworld(k=5, p=0.01)))
    m.verbose_off()
    return m


class TestEdges:
    @pytest.fixture
    def model(self):
        m = epiworld.Model(5)
        m.add_layer(layer.network("ties", graph.edgelist([], [], 5)))
        return m

    def test_add_has_rm(self, model):
        net = model.get_layer("ties")
        assert not net.is_directed()
        assert net.add_edge(0, 1)
        assert not net.add_edge(0, 1)  # already tied
        assert net.has_edge(0, 1) and net.has_edge(1, 0)
        assert net.get_degree(0) == 1
        assert list(net.get_edgelist()["to"]) == [1]

        assert net.rm_edge(1, 0)
        assert not net.rm_edge(1, 0)
        assert not net.has_edge(0, 1)
        assert net.get_degree(0) == 0

    def test_out_of_range(self, model):
        with pytest.raises(ValueError):
            model.get_layer("ties").add_edge(0, 99)

    def test_rewire_during_run(self):
        # Edits during a run keep the queue in step, and last for that run.
        m = make_sir(500)
        net = m.get_layer("contacts")

        def cut_ties(model):
            if model.today() == 5:
                el = net.get_edgelist()
                for i, j in zip(el["from"], el["to"]):
                    if 0 in (i, j):
                        net.rm_edge(int(i), int(j))
                assert net.get_degree(0) == 0

        m.add_globalevent(cut_ties, "cut")
        m.run(DAYS, SEED)
        assert net.get_degree(0) == 0


class TestTransmissionMode:
    def test_default_and_setters(self):
        m = make_sir()
        assert m.get_transmission_mode() == "auto"
        assert m.get_transmission_kappa() == pytest.approx(0.5)
        m.set_transmission_mode("pull", kappa=0.25)
        assert m.get_transmission_mode() == "pull"
        assert m.get_transmission_kappa() == pytest.approx(0.25)
        with pytest.raises(ValueError):
            m.set_transmission_mode("bogus")

    @pytest.mark.parametrize("mode", ["push", "pull"])
    def test_last_mode(self, mode):
        m = make_sir()
        m.set_transmission_mode(mode)
        m.run(DAYS, SEED)
        assert m.get_last_transmission_mode() == mode

    def test_custom_model_can_push(self):
        # A model built from scratch pushes too: its susceptible stage is
        # sampled by the model itself
        m = epiworld.Model(2000)
        m.add_param(0.5, "Transmission probability")
        m.add_param(0.2, "Recovery rate")
        m.add_virus(sir_virus("v"), assign.share(0.05))
        m.add_layer(layer.network("contacts", graph.smallworld(k=10, p=0.01)))
        m.verbose_off()
        m.set_transmission_mode("push")
        m.run(5, SEED)
        assert m.get_last_transmission_mode() == "push"

    def test_push_and_pull_agree(self):
        finals = {}
        for mode in ("push", "pull"):
            recovered = []
            for seed in range(20):
                m = epimodels.ModelSEIR("v", n=500, prevalence=0.05,
                                        transmission_prob=0.5,
                                        latent_period=4, recovery_rate=0.2)
                m.add_layer(
                    layer.network("contacts", graph.smallworld(k=10, p=0.01))
                )
                m.verbose_off()
                m.set_transmission_mode(mode)
                m.run(DAYS, seed)
                recovered.append(m.get_agents_stages().count("Removed"))
            finals[mode] = np.mean(recovered)
        assert finals["push"] == pytest.approx(finals["pull"], rel=0.15)

    def test_rates_compete(self):
        # Two rates out of a stage: each agent makes at most one transition a
        # day, and equal rates reach both targets about equally often
        v = (epiworld.Virus("v")
             .add_stage("S")
             .add_stage("X", infectious=True)
             .add_stage("A")
             .add_stage("B")
             .add_infection("S", "X", rate=0)
             .add_rate("X", "A", rate="to A")
             .add_rate("X", "B", rate="to B"))
        m = epiworld.Model(1000)
        m.add_param(0.5, "to A")
        m.add_param(0.5, "to B")
        m.add_virus(v, assign.share(1.0))
        m.verbose_off()
        m.run(1, SEED)
        stages = m.get_agents_stages()
        a, b = stages.count("A"), stages.count("B")
        assert a + b + stages.count("X") == 1000
        assert a > 250 and b > 250
        assert abs(a - b) < 100


class TestModelQueries:
    @pytest.fixture
    def ran(self):
        m = make_sir()
        m.run(DAYS, SEED)
        return m

    def test_size(self, ran):
        assert ran.size() == len(ran) == 2000

    def test_agents_stages(self, ran):
        stages = ran.get_agents_stages()
        assert len(stages) == ran.size()
        assert set(stages) <= set(ran.get_stages())
        assert [ran.get_stage(ran.get_agent(i)) for i in range(10)] == stages[:10]
        assert ran.get_agents_statuses() == ["Free"] * ran.size()
        assert ran.get_status(ran.get_agent(0)) == "Free"

    def test_has_param(self, ran):
        assert ran.has_param("Recovery rate")
        assert not ran.has_param("not a parameter")

    def test_globalevents(self):
        m = make_sir()
        assert m.get_n_globalevents() == 0
        assert not m.has_globalevent("noop")
        m.add_globalevent(lambda model: None, "noop")
        assert m.get_n_globalevents() == 1
        assert m.has_globalevent("noop")

    def test_queuing(self):
        m = make_sir()
        assert m.is_queuing_on()
        m.queuing_off()
        assert not m.is_queuing_on()
        m.queuing_on()
        assert m.is_queuing_on()

    def test_seed_reproducible(self):
        a, b = make_sir(), make_sir()
        for m in (a, b):
            m.seed(123)
            m.run(DAYS, -1)
        assert a.get_agents_stages() == b.get_agents_stages()

    def test_elapsed(self, ran):
        elapsed = ran.get_elapsed()
        assert set(elapsed) == {"last", "total", "unit"}

    def test_write_data(self, ran, tmp_path):
        ran.write_data(fn_total_hist=str(tmp_path / "total.csv"))
        assert (tmp_path / "total.csv").exists()


class TestFixes:
    def test_transmissions_have_targets(self):
        m = make_sir()
        m.run(DAYS, SEED)
        tr = m.get_db().get_transmissions()
        assert len(tr["targets"]) == len(tr["sources"]) > 0

    def test_deaths_are_deaths(self):
        # Deaths used to be recorded as recoveries.
        m = epimodels.ModelSIRD("v", n=200, prevalence=0.5,
                                transmission_prob=0.0, recovery_rate=0.0,
                                death_rate=1.0)
        m.verbose_off()
        m.run(10, SEED)
        stages = m.get_agents_stages()
        assert stages.count("Recovered") == 0
        assert stages.count("Deceased") == 100

    def test_mutate_virus(self):
        m = make_sir()
        m.run(1, SEED)
        mutated = []
        virus = next(
            a for a in m.get_agents() if a.get_virus() is not None
        ).get_virus()
        virus.set_mutation(lambda agent, v, model: mutated.append(1) or True)
        carrier = next(a for a in m.get_agents() if a.get_virus() is not None)
        carrier.mutate_virus(m)
        assert mutated == [1]

    def test_mutate_virus_without_virus(self):
        m = make_sir()
        m.run(1, SEED)
        healthy = next(a for a in m.get_agents() if a.get_virus() is None)
        with pytest.raises(RuntimeError):
            healthy.mutate_virus(m)


class TestAgentTools:
    def test_rm_tool(self):
        # Agent changes are queued as events and applied at the end of a step,
        # so we make them from global events during a run.
        m = make_sir(100)
        m.add_tool(epiworld.Tool("mask", transmission_reduction=0.5),
                   assign.share(1.0))

        def change(model):
            if model.today() == 1:
                model.get_agent(0).rm_tool(model, 0)

        m.add_globalevent(change, "change")

        m.run(2, SEED)
        assert m.get_agent(0).get_n_tools() == 0
