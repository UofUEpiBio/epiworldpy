"""Tests for new and improved model/database API features."""

import pytest
import epiworldpy.epimodels as epimodels
from epiworldpy import assign, graph, layer

DAYS = 50
SEED = 42


@pytest.fixture
def seirconn():
    m = epimodels.ModelSEIR(
        name="covid-19",
        n=5000,
        prevalence=0.02,
        transmission_prob=0.1,
        latent_period=7.0,
        recovery_rate=0.14,
    )
    # Everyone meets "Contact rate" others a day, at random
    m.add_param(2.0, "Contact rate")
    everyone = layer.mixing("everyone", [0] * 5000, contact_matrix=[1.0])
    everyone.set_contact_rate("Contact rate")
    m.add_layer(everyone)
    m.run(DAYS, SEED)
    return m


@pytest.fixture
def sir_smallworld():
    m = epimodels.ModelSIR(
        name="flu", n=5000, prevalence=0.01, transmission_prob=0.1,
        recovery_rate=0.14
    )
    m.add_layer(layer.network("contacts", graph.smallworld(k=5, p=0.01)))
    m.run(DAYS, SEED)
    return m



class TestModelAPI:
    def test_get_ndays(self, seirconn):
        assert seirconn.get_ndays() == DAYS

    def test_stages(self, seirconn):
        assert seirconn.get_stages() == [
            "Susceptible", "Exposed", "Infected", "Removed"
        ]
        assert seirconn.get_statuses() == ["Free"]

    def test_get_n_viruses(self, seirconn):
        assert seirconn.get_n_viruses() == 1

    def test_get_n_tools(self, seirconn):
        assert seirconn.get_n_tools() == 0

    def test_today(self, seirconn):
        assert seirconn.today() == DAYS

    def test_verbose_off_on(self, seirconn):
        seirconn.verbose_off()
        assert not seirconn.get_verbose()
        seirconn.verbose_on()
        assert seirconn.get_verbose()

    def test_params(self, seirconn):
        p = seirconn.params()
        assert isinstance(p, dict)
        # SEIRCOV should have transmission rate etc.
        assert len(p) > 0

    def test_reset(self, seirconn):
        seirconn.reset()
        assert seirconn.today() == 0

    def test_sbm(self):
        """A network layer drawn from a stochastic block model."""
        m = epimodels.ModelSIR(
            name="flu", n=1000, prevalence=0.01, transmission_prob=0.1,
            recovery_rate=0.14
        )
        # Two blocks of 500 agents each; 2x2 mixing matrix (row-major)
        m.add_layer(layer.network(
            "blocks", graph.sbm(block_sizes=[500, 500],
                                mixing_matrix=[0.01, 0.001, 0.001, 0.01])
        ))
        m.run(DAYS, SEED)
        hist = m.get_db().get_hist_total()
        assert len(hist["dates"]) > 0

    def test_bernoulli(self):
        """A network layer drawn from a Bernoulli random graph."""
        m = epimodels.ModelSIR(
            name="flu", n=1000, prevalence=0.01, transmission_prob=0.1,
            recovery_rate=0.14
        )
        m.add_layer(layer.network("random", graph.bernoulli(p=0.01)))
        m.run(DAYS, SEED)
        hist = m.get_db().get_hist_total()
        assert len(hist["dates"]) > 0

    def test_no_layers(self):
        """A network model without layers has no contacts."""
        m = epimodels.ModelSIS(
            name="sis", n=500, prevalence=0.01, transmission_prob=0.1,
            recovery_rate=0.14
        )
        m.run(10, SEED)
        tr = m.get_db().get_transmissions()
        assert (tr["sources"] >= 0).sum() == 0



class TestDatabaseAPI:
    def test_get_active_cases(self, seirconn):
        db = seirconn.get_db()
        active = db.get_active_cases()
        assert "dates" in active
        assert "virus_id" in active
        assert "counts" in active
        assert len(active["dates"]) > 0

    def test_get_hist_transition_matrix(self, seirconn):
        db = seirconn.get_db()
        tm = db.get_hist_transition_matrix(skip_zeros=True)
        assert sorted(tm) == [
            "counts", "dates", "stage_from", "stage_to", "status_from",
            "status_to",
        ]
        assert set(tm["stage_from"]["values"]) <= set(seirconn.get_stages())

    def test_get_hist_transitions(self, seirconn):
        # How many times each transition fired: the infections are the
        # agent-to-agent transmissions
        tr = seirconn.get_db().get_hist_transitions()
        names = list(tr["transitions"]["values"])
        assert names == [
            "Susceptible -> Exposed", "Exposed -> Infected",
            "Infected -> Removed",
        ]
        infections = tr["counts"][tr["transitions"]["indexes"] == 0].sum()
        tm = seirconn.get_db().get_transmissions()
        assert infections == (tm["sources"] >= 0).sum()

    def test_get_generation_time(self, sir_smallworld):
        db = sir_smallworld.get_db()
        gt = db.get_generation_time()
        assert "agents" in gt
        assert "viruses" in gt
        assert "times" in gt
        assert "generation_times" in gt

    def test_get_today_total(self, seirconn):
        db = seirconn.get_db()
        today = db.get_today_total()
        assert sorted(today) == ["counts", "stages", "statuses"]
        # 4 stages in SEIR, no statuses
        assert len(today["counts"]) == 4
        assert today["counts"].sum() == 5000

    def test_get_today_virus(self, seirconn):
        db = seirconn.get_db()
        tv = db.get_today_virus()
        assert sorted(tv) == ["counts", "ids", "stages", "statuses"]

    def test_transition_probability(self, seirconn):
        db = seirconn.get_db()
        tp = db.get_transition_probability()
        assert tp is not None

    def test_write_data(self, seirconn, tmp_path):
        db = seirconn.get_db()
        db.write_data(
            fn_virus_info=str(tmp_path / "virus_info.csv"),
            fn_virus_hist=str(tmp_path / "virus_hist.csv"),
            fn_tool_info=str(tmp_path / "tool_info.csv"),
            fn_tool_hist=str(tmp_path / "tool_hist.csv"),
            fn_total_hist=str(tmp_path / "total_hist.csv"),
            fn_transmission=str(tmp_path / "transmission.csv"),
            fn_transition=str(tmp_path / "transition.csv"),
            fn_reproductive_number=str(tmp_path / "repro.csv"),
            fn_generation_time=str(tmp_path / "gentime.csv"),
        )
        assert (tmp_path / "total_hist.csv").exists()



class TestRunMultiple:
    def test_run_multiple(self):
        m = epimodels.ModelSIR(
            name="flu",
            n=1000,
            prevalence=0.02,
            transmission_prob=0.1,
            recovery_rate=0.14,
        )
        # Everyone meets "Contact rate" others a day, at random
        m.add_param(2.0, "Contact rate")
        everyone = layer.mixing("everyone", [0] * 1000, contact_matrix=[1.0])
        everyone.set_contact_rate("Contact rate")
        m.add_layer(everyone)
        results = []

        def collector(sim_id, model):
            hist = model.get_db().get_hist_total()
            results.append(len(hist["dates"]))

        m.run_multiple(
            ndays=30,
            nexperiments=5,
            seed_=SEED,
            fun=collector,
            reset=True,
            verbose=False,
        )
        assert len(results) == 5
        for r in results:
            assert r > 0


class TestInitialStages:
    @staticmethod
    def _day0(model):
        hist = model.get_db().get_hist_total()
        names = hist["stages"]["values"]
        return {
            str(names[stage]): int(count)
            for date, stage, count in zip(
                hist["dates"], hist["stages"]["indexes"], hist["counts"]
            )
            if date == 0
        }

    @staticmethod
    def _seir(n, prevalence):
        m = epimodels.ModelSEIR(
            name="covid-19",
            n=n,
            prevalence=prevalence,
            transmission_prob=0.1,
            latent_period=7.0,
            recovery_rate=0.14,
        )
        # Everyone meets "Contact rate" others a day, at random
        m.add_param(2.0, "Contact rate")
        everyone = layer.mixing("everyone", [0] * n, contact_matrix=[1.0])
        everyone.set_contact_rate("Contact rate")
        m.add_layer(everyone)
        m.verbose_off()
        return m

    def test_seeds_and_initial_stages(self):
        """The seeds go to carrier stages, and set_initial() places agents
        in stages without the virus."""
        m = self._seir(10000, 0.0)
        m.assign_virus(0, assign.counts({"Exposed": 11, "Infected": 1116}))
        m.set_initial(assign.counts({"Removed": 1761}))
        m.run(0, SEED)
        assert self._day0(m) == {
            "Susceptible": 7112,
            "Exposed": 11,
            "Infected": 1116,
            "Removed": 1761,
        }

    def test_initial_stages_survive_rerun(self):
        """The placement is drawn on every run, not only the first: the seeds
        start in the stage the infection leads to, and a share is of the
        agents left."""
        m = self._seir(1000, 0.1)
        m.set_initial(assign.shares({"Removed": 0.5}))
        m.run(0, SEED)
        first = self._day0(m)
        m.run(0, SEED + 1)
        assert self._day0(m) == first
        assert first["Exposed"] == 100
        assert first["Removed"] == 450

    def test_labels_are_checked(self):
        m = self._seir(100, 0.1)
        with pytest.raises(ValueError, match='no stage "Recover"'):
            m.set_initial(assign.shares({"Recover": 0.1}))
        with pytest.raises(ValueError, match=r"\[0, 1\]"):
            assign.share(1.5)
        with pytest.raises(ValueError, match="whole"):
            assign.count(2.5)
        with pytest.raises(ValueError, match="agent ids"):
            assign.agents([-1])
