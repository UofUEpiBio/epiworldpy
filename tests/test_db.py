import epiworldpy as epiworld
import epiworldpy.epimodels as epimodels
from epiworldpy import layer
import pytest
import numpy as np

DAYS = 100


@pytest.fixture
def covid19():
    covid19 = epimodels.ModelSEIR(
        name="covid-19",
        n=10000,
        prevalence=0.01,
        transmission_prob=0.1,
        latent_period=7.0,
        recovery_rate=0.14,
    )
    # Everyone meets "Contact rate" others a day, at random
    covid19.add_param(2.0, "Contact rate")
    everyone = layer.mixing("everyone", [0] * 10000, contact_matrix=[1.0])
    everyone.set_contact_rate("Contact rate")
    covid19.add_layer(everyone)

    covid19.run(DAYS, 223)

    return covid19


def test_db_simple(covid19):
    history = covid19.get_db().get_hist_total()
    dates = history["dates"]
    stages = history["stages"]
    statuses = history["statuses"]
    counts = history["counts"]

    # The SEIR model has four stages (susceptible, exposed, infected, and
    # removed) and no statuses ("Free"), so DAYS + 1 days (day 0 to DAYS) of
    # four rows each, adding up to the population every day.
    EXPECTED_ENTRIES = (DAYS + 1) * 4

    assert len(dates) == EXPECTED_ENTRIES
    assert list(stages["values"]) == ["Susceptible", "Exposed", "Infected", "Removed"]
    assert list(statuses["values"]) == ["Free"]
    assert len(stages["indexes"]) == EXPECTED_ENTRIES
    assert len(counts) == EXPECTED_ENTRIES
    assert counts.sum() == 10000 * (DAYS + 1)


def test_db_reproductive_number(covid19):
    effective_reproductive_data = covid19.get_db().get_reproductive_number()

    #assert isinstance(effective_reproductive_data[0][0], np.ndarray)


def test_db_transmissions(covid19):
    transmissions = covid19.get_db().get_transmissions()
    start = transmissions["source_exposure_dates"]
    end = transmissions["dates"]
    source = transmissions["sources"]
    viruses = transmissions["viruses"]

    assert len(start) * len(end) * len(source) * len(viruses) == len(start) ** 4
