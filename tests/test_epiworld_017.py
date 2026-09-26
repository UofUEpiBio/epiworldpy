"""Tests for the API added with epiworld 0.16/0.17, and for wrapper fixes."""

import numpy as np
import pytest

import epiworldpy as epiworld
import epiworldpy.epimodels as epimodels

DAYS = 30
SEED = 1


def make_sir(n=2000):
    m = epimodels.ModelSIR(
        name="flu", prevalence=0.01, transmission_rate=0.3, recovery_rate=0.1
    )
    m.agents_smallworld(n=n, k=5, d=False, p=0.01)
    m.verbose_off()
    return m


class TestEdges:
    @pytest.fixture
    def model(self):
        m = epiworld.Model()
        m.agents_empty_graph(5)
        return m

    def test_add_has_rm(self, model):
        assert not model.is_directed()
        assert model.add_edge(0, 1)
        assert not model.add_edge(0, 1)  # already tied
        assert model.has_edge(0, 1) and model.has_edge(1, 0)
        assert model.get_agent(0).has_neighbor(1)
        assert [a.get_id() for a in model.get_agent(0).get_neighbors(model)] == [1]

        assert model.rm_edge(1, 0)
        assert not model.rm_edge(1, 0)
        assert not model.has_edge(0, 1)
        assert model.get_agent(0).get_n_neighbors() == 0

    def test_out_of_range(self, model):
        with pytest.raises(ValueError):
            model.add_edge(0, 99)

    def test_rewire_during_run(self):
        # add_edge/rm_edge keep the queue in step, so they are safe mid-run.
        m = make_sir(500)

        def cut_ties(model):
            if model.today() == 5:
                agent = model.get_agent(0)
                for nb in agent.get_neighbors(model):
                    model.rm_edge(0, nb.get_id())

        m.add_globalevent(cut_ties, "cut")
        m.run(DAYS, SEED)
        assert m.get_agent(0).get_n_neighbors() == 0


class TestTransmissionMode:
    def test_default_and_setters(self):
        m = make_sir()
        assert m.get_transmission_mode() == "auto"
        assert m.get_transmission_kappa() == pytest.approx(0.25)
        m.set_transmission_mode("pull", kappa=0.5)
        assert m.get_transmission_mode() == "pull"
        assert m.get_transmission_kappa() == pytest.approx(0.5)
        with pytest.raises(ValueError):
            m.set_transmission_mode("bogus")

    @pytest.mark.parametrize("mode", ["push", "pull"])
    def test_last_mode(self, mode):
        m = make_sir()
        m.set_transmission_mode(mode)
        m.run(DAYS, SEED)
        assert m.get_last_transmission_mode() == mode

    def test_custom_model_can_push(self):
        # UpdateFun.default_update_susceptible() must be epiworld's own
        # function; the model only pushes for that exact function.
        m = epiworld.Model()
        m.add_state("S", epiworld.UpdateFun.default_update_susceptible())
        infected = m.add_state("I", epiworld.UpdateFun.default_update_exposed())
        recovered = m.add_state("R")
        virus = epiworld.Virus("v", 0.05, True, 0.5, 0.2, 0.0)
        virus.set_state(infected, recovered, recovered)
        m.add_virus(virus)
        m.agents_smallworld(n=2000, k=10, d=False, p=0.01)
        m.verbose_off()
        m.set_transmission_mode("push")
        m.run(5, SEED)
        assert m.get_last_transmission_mode() == "push"


class TestNativeUpdateFun:
    @staticmethod
    def make_seir(n=2000):
        # SEIR from epiworld's own update functions: exposed agents do not
        # transmit, and every transition runs in C++.
        m = epiworld.Model()
        m.add_param(0.25, "Incubation rate")
        m.add_param(0.2, "Recovery rate")
        m.add_state("S", epiworld.UpdateFun.susceptible(exclude=[1]))
        m.add_state("E", epiworld.UpdateFun.rate(["Incubation rate"], [2]))
        m.add_state("I", epiworld.UpdateFun.rate(["Recovery rate"], [3]))
        m.add_state("R")
        virus = epiworld.Virus("v", 0.05, True, 0.5, 0.0, 0.0)
        virus.set_state(1, 3, 3)
        m.add_virus(virus)
        m.agents_smallworld(n=n, k=10, d=False, p=0.01)
        m.verbose_off()
        return m

    @pytest.mark.parametrize("mode", ["push", "pull"])
    def test_seir_runs_natively(self, mode):
        m = self.make_seir()
        m.set_transmission_mode(mode)
        m.run(DAYS, SEED)
        # The susceptible sampler must reach the model unchanged, or the model
        # could not recognize it and push.
        assert m.get_last_transmission_mode() == mode
        counts = m.get_db().get_today_total()["counts"]
        assert counts.sum() == m.size()
        assert counts[3] > 0  # someone recovered through the rate function

    def test_push_and_pull_agree(self):
        finals = {}
        for mode in ("push", "pull"):
            recovered = []
            for seed in range(20):
                m = self.make_seir(500)
                m.set_transmission_mode(mode)
                m.run(DAYS, seed)
                recovered.append(m.get_db().get_today_total()["counts"][3])
            finals[mode] = np.mean(recovered)
        assert finals["push"] == pytest.approx(finals["pull"], rel=0.15)

    def test_set_state_function(self):
        m = self.make_seir()
        m.set_state_function("I", epiworld.UpdateFun.rate(["Recovery rate"], [3]))
        m.set_state_function(0, epiworld.UpdateFun.susceptible([1]))
        m.set_transmission_mode("push")
        m.run(5, SEED)
        assert m.get_last_transmission_mode() == "push"

    def test_rate_competing_transitions(self):
        m = epiworld.Model()
        m.add_param(0.5, "to A")
        m.add_param(0.5, "to B")
        m.add_state("X", epiworld.UpdateFun.rate(["to A", "to B"], [1, 2]))
        m.add_state("A")
        m.add_state("B")
        m.agents_empty_graph(1000)
        m.verbose_off()
        # Without a virus the queue would skip every agent.
        m.queuing_off()
        m.run(1, SEED)
        # Each agent makes at most one transition, and equal rates reach both
        # targets about equally often.
        counts = m.get_db().get_today_total()["counts"]
        assert counts.sum() == 1000
        assert counts[1] > 250 and counts[2] > 250
        assert abs(int(counts[1]) - int(counts[2])) < 100

    def test_rate_checks_lengths(self):
        with pytest.raises(RuntimeError):
            epiworld.UpdateFun.rate(["a", "b"], [1])
        with pytest.raises(RuntimeError):
            epiworld.UpdateFun.rate([], [])


class TestModelQueries:
    @pytest.fixture
    def ran(self):
        m = make_sir()
        m.run(DAYS, SEED)
        return m

    def test_size(self, ran):
        assert ran.size() == len(ran) == 2000

    def test_agents_states_and_in_state(self, ran):
        states = np.asarray(ran.get_agents_states())
        assert len(states) == ran.size()
        for s in range(ran.get_n_states()):
            ids = ran.get_agents_in_state(s)
            assert sorted(ids) == list(np.flatnonzero(states == s))

    def test_agents_in_state_before_run(self):
        with pytest.raises(RuntimeError):
            make_sir().get_agents_in_state(0)

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
        assert list(a.get_agents_states()) == list(b.get_agents_states())

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

    def test_default_update_exposed_kills(self):
        # Deaths used to be recorded as recoveries.
        m = epiworld.Model()
        m.add_state("S", epiworld.UpdateFun.default_update_susceptible())
        infected = m.add_state("I", epiworld.UpdateFun.default_update_exposed())
        recovered = m.add_state("R")
        dead = m.add_state("D")
        virus = epiworld.Virus("v", 0.5, True, 0.0, 0.0, 1.0)
        virus.set_state(infected, recovered, dead)
        m.add_virus(virus)
        m.agents_empty_graph(200)
        m.verbose_off()
        m.run(10, SEED)
        counts = np.bincount(m.get_agents_states(), minlength=4)
        assert counts[recovered] == 0
        assert counts[dead] == 100

    def test_virus_incubation_default(self):
        m = make_sir()
        virus = epiworld.Virus("v", 1, False, 0.1, 0.1, 0.1)
        assert virus.get_incubation(m) == pytest.approx(7.0)
        virus = epiworld.Virus("v", 1, False, 0.1, 0.1, 0.1, incubation=3.0)
        assert virus.get_incubation(m) == pytest.approx(3.0)

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


class TestAgentToolsEntities:
    def test_rm_tool_add_rm_entity(self):
        # Agent changes are queued as events and applied at the end of a step,
        # so we make them from global events during a run.
        m = make_sir(100)
        m.add_tool(epiworld.Tool("mask", 1.0, True, transmission_reduction=0.5))
        m.add_entity(epiworld.Entity("home"))

        def change(model):
            agent = model.get_agent(0)
            if model.today() == 1:
                agent.rm_tool(model, 0)
                agent.add_entity(model, model.get_entity(0))
            elif model.today() == 3:
                agent.rm_entity(model, model.get_entity(0))

        m.add_globalevent(change, "change")

        m.run(2, SEED)
        agent = m.get_agent(0)
        assert agent.get_n_tools() == 0
        assert list(agent.get_entities()) == [0]
        assert 0 in list(m.get_entity(0).get_agents_ids())

        m.run(4, SEED)
        assert list(m.get_agent(0).get_entities()) == []
