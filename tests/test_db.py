import epiworldpy as epiworld
import epiworldpy.epimodels as epimodels
import pytest
import numpy as np

DAYS = 100


@pytest.fixture
def covid19():
    covid19 = epimodels.ModelSEIRCONN(
        name="covid-19",
        n=10000,
        prevalence=0.01,
        contact_rate=2.0,
        transmission_rate=0.1,
        incubation_days=7.0,
        recovery_rate=0.14,
    )

    covid19.run(DAYS, 223)

    return covid19


def test_db_simple(covid19):
    history = covid19.get_db().get_hist_total()
    dates = history["dates"]
    states = history["states"]
    counts = history["counts"]

    # Considering that the SEIR model has four states (susceptible, exposed, infected, and
    # recovered), we expect DAYS + 1 * 4 (we do the plus one since the resulting time series
    # starts at 0 and the upper bound is treated as inclusive by epiworld).
    EXPECTED_ENTRIES = (DAYS + 1) * 4

    assert len(dates) == EXPECTED_ENTRIES
    assert len(states["values"]) == 4
    assert len(states["indexes"]) == EXPECTED_ENTRIES
    assert len(counts) == EXPECTED_ENTRIES


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
