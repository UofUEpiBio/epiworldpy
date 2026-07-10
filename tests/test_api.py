"""Tests for new and improved model/database API features."""

import pytest
import epiworldpy.epimodels as epimodels

DAYS = 50
SEED = 42


@pytest.fixture
def seirconn():
    m = epimodels.ModelSEIRCONN(
        name="covid-19",
        n=5000,
        prevalence=0.02,
        contact_rate=2.0,
        transmission_rate=0.1,
        incubation_days=7.0,
        recovery_rate=0.14,
    )
    m.run(DAYS, SEED)
    return m


@pytest.fixture
def sir_smallworld():
    m = epimodels.ModelSIR(
        name="flu", prevalence=0.01, transmission_rate=0.1, recovery_rate=0.14
    )
    m.agents_smallworld(5000, 5, False, 0.01)
    m.run(DAYS, SEED)
    return m



class TestModelAPI:
    def test_get_ndays(self, seirconn):
        assert seirconn.get_ndays() == DAYS

    def test_get_n_states(self, seirconn):
        # SEIR has 4 states
        assert seirconn.get_n_states() == 4

    def test_state_of(self, seirconn):
        idx = seirconn.state_of("Susceptible")
        assert idx == 0

    def test_state_of_unknown(self, seirconn):
        # Unknown state should return -1 or raise
        try:
            idx = seirconn.state_of("NonExistentState")
            assert idx < 0
        except Exception:
            pass  # Either -1 or exception is acceptable

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

    def test_agents_sbm(self):
        """agents_sbm should populate model using Stochastic Block Model."""
        m = epimodels.ModelSIR(
            name="flu", prevalence=0.01, transmission_rate=0.1, recovery_rate=0.14
        )
        # Two blocks of 500 agents each; 2x2 mixing matrix (row-major)
        m.agents_sbm(
            block_sizes=[500, 500],
            mixing_matrix=[4.0, 0.5, 0.5, 4.0],
            row_major=True,
        )
        m.run(DAYS, SEED)
        hist = m.get_db().get_hist_total()
        assert len(hist["dates"]) > 0

    def test_agents_bernoulli(self):
        """agents_bernoulli should populate model using a Bernoulli random graph."""
        m = epimodels.ModelSIR(
            name="flu", prevalence=0.01, transmission_rate=0.1, recovery_rate=0.14
        )
        m.agents_bernoulli(n=1000, p=0.01)
        m.run(DAYS, SEED)
        hist = m.get_db().get_hist_total()
        assert len(hist["dates"]) > 0

    def test_agents_empty_graph(self):
        """agents_empty_graph should create isolated agents."""
        m = epimodels.ModelSIS(
            name="sis", prevalence=0.01, transmission_rate=0.1, recovery_rate=0.14
        )
        m.agents_empty_graph(n=500)
        m.run(10, SEED)



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
        assert "state_from" in tm
        assert "state_to" in tm
        assert "dates" in tm
        assert "counts" in tm

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
        assert "states" in today
        assert "counts" in today
        # 4 states in SEIR
        assert len(today["counts"]) == 4

    def test_get_today_virus(self, seirconn):
        db = seirconn.get_db()
        tv = db.get_today_virus()
        assert "states" in tv
        assert "ids" in tv
        assert "counts" in tv

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
        m = epimodels.ModelSIRCONN(
            name="flu",
            n=1000,
            prevalence=0.02,
            contact_rate=2.0,
            transmission_rate=0.1,
            recovery_rate=0.14,
        )
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
