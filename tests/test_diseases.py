"""Several diseases and strains on one population (epiworld NH-5b-2/3)."""

import csv
from collections import defaultdict

import numpy as np
import pytest

from epiworldpy import (Model, Virus, assign, epimodels, graph, layer,
                       extract_database_results, write_db_results_multiple_csv)


def decode(table):
    """Expand the binding's categorical string columns."""
    return {k: v["values"][v["indexes"]] if isinstance(v, dict) else v
            for k, v in table.items()}


def test_diseases(tmp_path):
    n = 3000
    model = epimodels.ModelSIR("Flu", n=n, prevalence=0.01,
                               transmission_prob=0.3, recovery_rate=0.2)
    model.add_layer(layer.network("contacts", graph.smallworld(k=8, p=0.05)))
    model.verbose_off()
    assert model.get_db().get_transition_probability().shape == (0, 0)

    model.add_param(0.4, "Flu B recovery")
    flu_b = model.get_virus(0).strain("Flu B")
    assert isinstance(flu_b, Virus)
    flu_b.use_param("Recovery rate", "Flu B recovery")
    model.add_virus(flu_b, assign.count(10))
    assert model.get_diseases() == ["Flu"]

    rsv = (Virus("RSV").add_stage("Susceptible")
           .add_stage("Infected", infectious=True).add_stage("Recovered")
           .add_infection("Susceptible", "Infected", rate=0.2)
           .add_rate("Infected", "Recovered", rate=0.1))
    model.add_virus(rsv, assign.count(20))
    assert model.get_diseases() == ["Flu", "RSV"]
    model.run(120, 331)
    db = model.get_db()

    ht = decode(db.get_hist_total())
    assert list(dict.fromkeys(ht["disease"])) == ["Flu", "RSV"]
    totals = defaultdict(int)
    for d, day, count in zip(ht["disease"], ht["dates"], ht["counts"]):
        totals[d, day] += int(count)
    assert len(totals) == 2 * 121
    assert set(totals.values()) == {n}
    assert set(decode(db.get_hist_total(disease="RSV"))["disease"]) == {"RSV"}

    stages_flu = np.array(model.get_agents_stages(disease="Flu"))
    stages_rsv = np.array(model.get_agents_stages(disease="RSV"))
    assert ((stages_flu == "Recovered") & (stages_rsv == "Recovered")).any()
    assert model.get_stages(disease="RSV") == ["Susceptible", "Infected", "Recovered"]
    agent = model.get_agent(0)
    assert model.get_stage(agent, disease="Flu") in model.get_stages("Flu")

    for call in (model.get_agents_stages, model.get_stages,
                 lambda: model.get_stage(agent),
                 lambda: model.set_initial(assign.counts({"Recovered": 0})),
                 lambda: model.fire("Infected -> Recovered", 0),
                 db.get_transition_probability,
                 lambda: db.get_transition_probability(margin="stage")):
        with pytest.raises((ValueError, RuntimeError), match="disease"):
            call()
    assert db.get_transition_probability(margin="stage", disease="RSV").shape == (3, 3)
    assert db.get_transition_probability(margin="status").shape == (1, 1)
    raw = db.get_transition_probability("stage", "RSV", normalize=False)
    np.testing.assert_allclose(raw.sum(axis=1).sum(), n * 120)
    probs = db.get_transition_probability("stage", "RSV")
    np.testing.assert_allclose(probs[raw.sum(axis=1) > 0].sum(axis=1), 1)
    with pytest.raises(ValueError, match="margin"):
        db.get_transition_probability(margin="unknown")

    tr = decode(db.get_transmissions())
    assert set(tr["disease"]) == {"Flu", "RSV"}
    assert set(tr["viruses"][tr["disease"] == "RSV"]) == {2}
    assert set(tr["viruses"][tr["disease"] == "Flu"]) == {0, 1}
    hv = decode(db.get_hist_virus(disease="Flu"))
    assert set(hv["ids"]) == {0, 1}
    vt = decode(db.get_hist_virus_transitions(disease="Flu"))
    assert set(vt["viruses"]) == {"Flu", "Flu B"}
    transitions = decode(db.get_hist_transitions(disease="Flu"))
    by_day = defaultdict(int)
    for day, name, count in zip(vt["dates"], vt["transitions"], vt["counts"]):
        by_day[day, name] += int(count)
    assert len(by_day) == len(transitions["counts"])
    for day, name, count in zip(transitions["dates"], transitions["transitions"],
                                transitions["counts"]):
        assert by_day[day, name] == count
    for virus_id, expected in ((0, 0.2), (1, 0.4)):
        recovered = vt["counts"][(vt["ids"] == virus_id) &
                                  (vt["transitions"] == "Infected -> Recovered")].sum()
        infected_days = hv["counts"][(hv["ids"] == virus_id) &
                                      (hv["stages"] == "Infected")].sum()
        assert recovered / infected_days == pytest.approx(expected, abs=0.1)

    outputs = [db.get_hist_total, db.get_hist_virus, db.get_hist_tool,
               db.get_today_total, db.get_today_virus, db.get_hist_transitions,
               db.get_hist_virus_transitions, db.get_hist_transition_matrix,
               db.get_reproductive_number, db.get_transmissions,
               db.get_generation_time, db.get_active_cases, db.get_outbreak_size,
               db.get_hospitalizations]
    for output in outputs:
        assert next(iter(output())) == "disease"
        selected = decode(output(disease="RSV"))
        assert set(selected["disease"]) <= {"RSV"}
        with pytest.raises(ValueError, match='no disease "Measles"'):
            output(disease="Measles")
        for bad in (1, True, [], Virus("bad")):
            with pytest.raises(TypeError, match="disease's name"):
                output(disease=bad)

    # Python CSV savers expand categorical columns, including disease names.
    results = extract_database_results(db, "total_hist", "transmission", "reproductive")
    write_db_results_multiple_csv(results, directory=tmp_path)
    for name, table in results.items():
        with (tmp_path / f"results-{name}.csv").open(newline="") as f:
            rows = list(csv.DictReader(f))
        assert len(rows) == len(table[next(k for k in table if k != "disease")])
        assert set(row["disease"] for row in rows) == {"Flu", "RSV"}

    # Initial placement and transition lookup resolve against the named disease.
    model.set_initial(assign.counts({"Recovered": 10}), disease="RSV")
    model.assign_virus(2, assign.counts({"Infected": 20}))
    model.run(0, 331)
    assert model.get_agents_stages("RSV").count("Recovered") == 10
    rsv_agent = model.get_agents_stages("RSV").index("Infected")
    assert model.fire("Infected -> Recovered", rsv_agent, disease="RSV")


def test_empty_model_diseases():
    model = Model(5)
    assert model.get_diseases() == []
    assert model.get_agents_stages() == [""] * 5
