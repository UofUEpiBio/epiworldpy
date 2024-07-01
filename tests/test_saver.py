import epiworldpy as epiworld

def test_saver_basic():
    covid19 = epiworld.ModelSEIR(
        name              = 'covid-19',
        n                 = 10000,
        prevalence        = .01,
        contact_rate      = 2.0,
        transmission_rate = .1,
        incubation_days   = 7.0,
        recovery_rate     = 0.14
    )

    saver = epiworld.Saver("total_hist", "virus_hist")

    saver.run_multiple(covid19, 100, 4, nthreads=1)

    # TODO: Verify things worked correctly, as is the point of tesing.