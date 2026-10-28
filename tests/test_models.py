import epiworldpy as epiworld
import epiworldpy.epimodels as epimodels
from epiworldpy import graph, layer


def test_smallworld():
    hypothetical = epimodels.ModelSIR(
        name="hypothetical",
        n=100000,
        prevalence=0.01,
        transmission_prob=0.1,
        recovery_rate=0.14,
    )

    hypothetical.add_layer(
        layer.network("contacts", graph.smallworld(k=10, p=0.01))
    )

    hypothetical.run(100, 223)
