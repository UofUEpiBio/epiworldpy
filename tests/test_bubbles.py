"""Social bubbles (epiworld's Bubbles): with ties="complete", bubblemates meet
as groups of a household layer while the policy is in force."""

import numpy as np
import pytest

import epiworldpy as epiworld
import epiworldpy.epimodels as epimodels
from epiworldpy import graph, layer

HOUSEHOLDS = [1, 1, 1, 2, 2, 2]  # two households of 3


def path_of_six(start_day, end_day):
    """Two households on a path 0-1-2-3-4-5, which form one bubble of 6."""
    m = epimodels.ModelSIR(
        "x", n=6, prevalence=0, transmission_prob=0, recovery_rate=0
    )
    m.add_layer(layer.network("network", graph.edgelist(
        [0, 1, 2, 3, 4], [1, 2, 3, 4, 5], 6
    )))
    b = epiworld.Bubbles(
        HOUSEHOLDS, "household", 2, start_day=start_day, end_day=end_day,
        ties="complete", layer="household"
    )
    m.add_layer(b.make_layer())
    m.add_globalevent(b)

    # The size of agent 0's group in the household layer, every day
    sizes = []

    def count(model):
        hh = model.get_layer("household")
        sizes.append(len(hh.members_of(hh.get_group(0))))

    m.add_globalevent(count, "Count groupmates")
    m.verbose_off()
    return m, b, sizes


def test_complete_ties_move_agents_between_groups():
    # The policy's event runs before the counting one, and its moves take
    # effect at the end of the step: agents are in the bubble on days 2-4 and
    # back in their households from day 5 (end_day).
    m, b, sizes = path_of_six(start_day=2, end_day=5)
    assert b.get_flavor() == "household" and b.get_ties() == "complete"

    m.run(8, 1)
    assert sizes == [3, 6, 6, 6, 3, 3, 3, 3]
    assert len(m.get_layer("network").get_edgelist()["from"]) == 5  # untouched

    # The model ran its own copy: the bubbles are read from there
    assert b.get_bubble_id() == []
    mine = epiworld.Bubbles.get_from(m)
    assert mine.get_last_epoch() == 0
    assert len(set(mine.get_bubble_id())) == 1  # everyone in one bubble
    assert epiworld.Bubbles.get_from(m, "nope") is None

    # Every replicate starts from the households (the layer's reset puts
    # everyone back), including after a zero-day run
    m, _, sizes = path_of_six(start_day=0, end_day=3)
    m.run(0, 1)
    m.run_multiple(6, 2, seed_=1, verbose=False, nthreads=1)
    assert sizes == [6, 6, 3, 3, 3, 3] * 2


def test_bubble_contact_prob():
    # Households of 3, scattered over a small world so that they are not
    # near-cliques; a bubble contact probability of 0 lets through less
    # transmission than 1.
    n = 1200
    households = np.random.default_rng(771).permutation(
        np.arange(n) // 3
    ).tolist()

    def total_removed(bubble_contact_prob):
        total = 0
        for seed in range(1, 6):
            m = epimodels.ModelSEIR(
                "Flu", n=n, prevalence=0.01, transmission_prob=0.03,
                latent_period=3, recovery_rate=1 / 7
            )
            m.add_layer(layer.network("network", graph.smallworld(k=8, p=0.1)))
            b = epiworld.Bubbles(
                households, "household", 2, transmission_factor=0.2,
                start_day=5, ties="complete", layer="household",
                bubble_contact_prob=bubble_contact_prob
            )
            m.add_layer(b.make_layer())
            m.add_globalevent(b)
            m.verbose_off()
            m.run(60, seed)
            removed = m.get_states().index("Removed")
            total += sum(s == removed for s in m.get_agents_states())
        return total

    assert total_removed(0.0) < total_removed(1.0)


def test_errors():
    with pytest.raises(ValueError, match="flavor"):
        epiworld.Bubbles(HOUSEHOLDS, "family", 2)
    with pytest.raises(ValueError, match="ties"):
        epiworld.Bubbles(HOUSEHOLDS, "household", 2, ties="all")

    b = epiworld.Bubbles(HOUSEHOLDS, "peer", 1)
    with pytest.raises(ValueError, match="ties"):
        b.set_ties("all", "household")
    with pytest.raises(ValueError, match=r"\[0, 1\]"):
        b.set_ties("complete", "household", bubble_contact_prob=1.5)
    with pytest.raises(ValueError, match=r"\[0, 1\]"):
        epiworld.Bubbles(
            HOUSEHOLDS, "household", 2, ties="complete", layer="household",
            bubble_contact_prob=-0.1
        )

    # make_layer() needs the layer's name, and checks its probability
    with pytest.raises(RuntimeError, match="set_ties"):
        b.make_layer()
    b.set_ties("complete", "household", 0.5)
    assert b.get_ties() == "complete" and b.get_flavor() == "peer"
    with pytest.raises(ValueError, match=r"\[0, 1\]"):
        b.make_layer(2.0)
    assert isinstance(b.make_layer(0.5), layer.Mixing)

    # Complete ties without the household layer in the model
    m = epimodels.ModelSIR(
        "x", n=6, prevalence=0, transmission_prob=0, recovery_rate=0
    )
    m.add_globalevent(b)
    with pytest.raises(RuntimeError, match="make_layer"):
        m.run(3, 1)
