import epiworldpy as epiworld
import epiworldpy.epimodels as epimodels


def test_distribution():
    hypothetical = epimodels.ModelSIR(
        name="hypothetical", prevalence=0.01, transmission_rate=0.1, recovery_rate=0.14
    )

    hypothetical.agents_smallworld(100000, 10, False, 0.01)

    hypothetical.run(100, 223)
