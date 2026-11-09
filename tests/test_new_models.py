"""Tests for models added or newly bound in the updated epiworld bindings."""

import pytest
import epiworldpy as epiworld
import epiworldpy.epimodels as epimodels
from epiworldpy import graph, layer

DAYS = 50
SEED = 42
N = 2000


class TestSEIRDRandomMixing:
    def make_model(self):
        m = epimodels.ModelSEIRD(
            name="ebola",
            n=5000,
            prevalence=0.005,
            transmission_prob=0.15,
            latent_period=6.0,
            recovery_rate=0.10,
            death_rate=0.02,
        )
        m.add_param(2.0, "Contact rate")
        everyone = layer.mixing("everyone", [0] * 5000, contact_matrix=[1.0])
        everyone.set_contact_rate("Contact rate")
        m.add_layer(everyone)
        return m

    def test_runs(self):
        m = self.make_model()
        m.run(DAYS, SEED)

    def test_stages(self):
        m = self.make_model()
        assert m.get_stages() == [
            "Susceptible", "Exposed", "Infected", "Removed", "Deceased"
        ]
        assert m.get_statuses() == ["Free"]


class TestSEIRQuarantine:
    def make_model(self):
        m = epimodels.ModelSEIRQuarantine(
            name="pathogen",
            n=2000,
            prevalence=0.02,
            transmission_prob=0.1,
            latent_period=5.0,
            recovery_rate=0.14,
            hospitalization_rate=0.05,
            hospitalization_period=7,
            detection_delay=2.0,
            quarantine_period=10,
            quarantine_willingness=0.8,
            isolation_willingness=0.9,
            isolation_period=10,
        )
        m.add_layer(layer.network("contacts", graph.smallworld(k=5, p=0.01)))
        return m

    def test_runs(self):
        m = self.make_model()
        m.run(DAYS, SEED)

    def test_stages_and_statuses(self):
        m = self.make_model()
        assert m.get_stages() == [
            "Susceptible", "Exposed", "Infected", "Severe", "Recovered"
        ]
        assert m.get_statuses() == [
            "Free", "Quarantined", "Isolated", "Hospitalized"
        ]

    def test_history_has_data(self):
        m = self.make_model()
        m.run(DAYS, SEED)
        hist = m.get_db().get_hist_total()
        assert len(hist["dates"]) > 0
