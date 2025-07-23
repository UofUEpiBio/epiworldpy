import epiworldpy as epiworld

def test_seirconn_simple():
    hypothetical = epiworld.ModelSEIRCONN(
        name              = 'hypothetical',
        n                 = 10000,
        prevalence        = 0.01,
        contact_rate      = 2.0,
        transmission_rate = 0.1,
        incubation_days   = 7.0,
        recovery_rate     = 0.14
    )

    hypothetical.run(100, 223)

def test_sirconn_simple():
    hypothetical = epiworld.ModelSIRCONN(
        name              = 'hypothetical',
        n                 = 10000,
        prevalence        = 0.01,
        contact_rate      = 2.0,
        transmission_rate = 0.1,
        recovery_rate     = 0.14
    )

    hypothetical.run(100, 223)

def test_smallworld():
    hypothetical = epiworld.ModelSIR(
        name              = 'hypothetical',
        prevalence        = 0.01,
        transmission_rate = 0.1,
        recovery_rate     = 0.14
    )

    hypothetical.agents_smallworld(100000, 10, False, 0.01)

    hypothetical.run(100, 223)
