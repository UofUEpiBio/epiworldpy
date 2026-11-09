# epiworldPy


[![](https://github.com/UofUEpiBio/epiworldpy/actions/workflows/pip.yaml/badge.svg)](https://github.com/UofUEpiBio/epiworldpy/actions/workflows/pip.yaml)
[![](https://img.shields.io/pypi/v/epiworldpy.svg)](https://pypi.org/project/epiworldpy)
[![ForeSITE
Group](https://github.com/EpiForeSITE/software/blob/e82ed88f75e0fe5c0a1a3b38c2b94509f122019c/docs/assets/foresite-software-badge.svg)](https://github.com/EpiForeSITE)

This Python package is a wrapper of the C++ library
[epiworld](https://github.com/UofUEpiBio/epiworld). It provides a
general framework for modeling disease transmission using agent-based
models. Some of the main features include:

- Fast simulation with an average of 30 million agents/day per second.
- One model can include multiple diseases.
- Policies (tools) can be multiple and user-defined.
- Transmission can be a function of agents’ features.
- Out-of-the-box parallelization for multiple simulations.

An evolving [cross-framework
benchmark](https://github.com/UofUEpiBio/epiworld-benchmark) compares
epiworldpy, epiworld, epiworldR, and other epidemic ABM engines. In the
SEIRH network scenarios currently covered, the epiworld family is among
the fastest, while native epiworld is among the lower-memory
implementations. These results are a snapshot of the tested versions,
workloads, and environment rather than a universal ranking; the
benchmark welcomes additional scenarios, engines, corrections, and
implementation improvements through
[issues](https://github.com/UofUEpiBio/epiworld-benchmark/issues) and
[pull requests](https://github.com/UofUEpiBio/epiworld-benchmark/pulls).

*See the [intro](docs/intro.md) for demos!*

## Installation

Installation can be performed through pip (*pip installs packages*), or
through your favorite Python package manager (`uv` is preferable).

    pip install epiworldpy

If there’s a feature that’s only available on the repository, and hasn’t
yet been published to PyPi, please create an issue so we know to get on
publishing. In the meantime, you can clone the repository though Git,
and install locally in your virtual environment.

``` bash
git clone https://github.com/uofUEpiBio/epiworldpy
cd epiworldpy

git checkout $COMMIT
uv install .
pip install .
```

## Quickstart

For a quickstart, please see the [intro](docs/intro.md).

## Migrating to 1.0.0 (development)

epiworldpy 1.0.0 follows epiworld 1.0.0, where contacts come from contact
layers. Code written for 0.x needs these changes; removed methods raise an
error that names their replacement.

- The population size goes to the model constructor, and only there:
  `epimodels.ModelSIR(name, n, ...)`, `Model(n)`.
- Networks are layers: `model.add_layer(layer.network("contacts",
  graph.smallworld(k=5, p=0.01)))`, with `graph.bernoulli()`, `graph.sbm()` or
  a fixed `graph.edgelist()`. `agents_smallworld()` and the other `agents_*()`
  methods are gone.
- Network edits and queries are on the layer (`model.get_layer(name)`):
  `add_edge()`, `rm_edge()`, `has_edge()`, `get_degree()`, `get_edgelist()`,
  `set_rewiring(graph.rewire_degseq(param))`. The contact matrix of a mixing
  model is on `model.get_layer("mixing")`.
- `get_transmissions()` gains `layers`: the layer each infection came
  through. Layers have a weight (`set_weight(param)`) and virus targets
  (`add_virus_target(virus)`).
- Entities are gone; groups live in mixing layers
  ([epiworld#349](https://github.com/UofUEpiBio/epiworld/pull/349)).
  `Entity`, `Model.add_entity()`, `get_entity()`, `get_n_entities()`, and
  `Agent.add_entity()`, `rm_entity()`, `has_entity()`, `get_entities()`,
  `get_n_entities()` are defunct. Use `layer.mixing(name, groups, ...)` with a
  list of groups or a placement (`assign.counts(sizes)`), and read groups with
  the layer's `members_of()` and `get_group()`.
- Natural histories replace states
  ([epiworld#363](https://github.com/UofUEpiBio/epiworld/pull/363)). A virus
  carries the stages of the disease and the transitions between them, each
  naming the model parameters it reads:

  ```python
  flu = (Virus("Flu")
         .add_stage("Susceptible")
         .add_stage("Infected", infectious=True)
         .add_stage("Recovered")
         .add_infection("Susceptible", "Infected", rate="Transmission probability")
         .add_rate("Infected", "Recovered", rate="Recovery rate", effect="recovery"))
  model.add_virus(flu, assign.share(0.01))
  ```

  `Duration` (for `add_after()`) and `Value` (`Value.inverse()`,
  `Value.complement()`, `Value.fun()`) give the transitions their timing and
  values; `add_external()` declares a transition that `Model.fire()` applies.
  `Virus(name)` takes only a name, and `Tool(name, ...)` takes its effects as
  keywords. The state API is defunct: `Model.add_state()`,
  `set_state_function()`, `get_states()`, `state_of()`, `initial_states()`,
  `set_infectious_states()`, `get_agents_states()`, `UpdateFun`,
  `NativeUpdateFun`, `Agent.get_state()`, `change_state()`, `rm_virus()`, and
  the probabilities, states and distributions of `Virus` and `Tool`
  (`set_prob_infecting()`, `set_state()`, `set_distribution()`, ...). Use
  `Model.get_stages()`, `get_statuses()`, `get_stage(agent)`,
  `get_agents_stages()`, `fire()`, and `draw_natural_history()`.
- Placement is one vocabulary, `epiworldpy.assign`: `share()`, `count()`,
  `shares()`, `counts()`, `group_shares()`, `group_counts()`, `agents()`,
  `labels()`, `by()`. It places viruses (`Model.add_virus(virus, assign)`,
  `assign_virus()`), tools (`add_tool(tool, assign)`, `assign_tool()`),
  agents in stages without the virus (`set_initial(assign.shares({"Recovered":
  0.3}))`), and the groups of a mixing layer. `grouping.random()` is defunct.
- The outputs report a status and a stage: `get_hist_total()` returns
  `statuses` and `stages` (in place of `states`), and so do
  `get_today_total()`, `get_hist_virus()`, `get_hist_tool()` and
  `get_today_virus()`; the transition matrix has `status_from`, `stage_from`,
  `status_to` and `stage_to`. `get_hist_transitions()` counts how many times
  each transition fired.
- Models come from the registry
  ([epiworld#356](https://github.com/UofUEpiBio/epiworld/pull/356)):
  `epimodels.make_model(id, values, virus_name)`, `list_models()` and
  `get_model_info(id)`. `ModelSIR()` and the other named constructors keep
  their arguments and return a `Model`. A second virus is a strain of the
  model's: a copy (`model.get_virus(0).clone()`) with its own name. The model
  keeps its own copy of each virus and tool, and only that copy has an id:
  give agents and tools `model.get_virus(i)` and `model.get_tool(i)`.
- Social bubbles: `Bubbles` is new in epiworldpy
  ([epiworld#353](https://github.com/UofUEpiBio/epiworld/pull/353)). With
  `ties="complete"`, bubblemates meet in a household layer (a within-group
  mixing layer, one group per household) that `make_layer()` builds and you
  add; the bubbles are larger groups of it while the policy is in force:

  ```python
  b = epiworldpy.Bubbles(hh, "household", 3, ties="complete",
                         layer="household", bubble_contact_prob=0.5)
  model.add_layer(b.make_layer(1.0))  # household_contact_prob
  model.add_globalevent(b)            # the model keeps a copy
  ```

  `flavor` decides who bubbles together; `ties` decides who then meets.
  These are the closest settings for two styles of rule used during
  COVID-19, not exact matches:

  | Policy style | Settings |
  |---|---|
  | Households join one or a few others (New Zealand; UK support bubbles; Belgium, May 2020) | `"household"`, `group_size` = households per bubble, `ties="complete"` |
  | One close contact per person (Belgium, October 2020) | `"peer"`, `max_households=2` (the default), `ties="existing"` |

  The second is an approximation: `"peer"` commits whole households, so one
  accepted choice pairs two households and their other members choose no one
  else, and `"existing"` spares every tie between the two households. A larger
  `max_households` lets more members choose, with larger bubbles.
  `flavor="peer"` with `ties="complete"` makes both households meet in full.

## Documentation

This is a short introduction to epiworldPy; for complete documentation,
see the API documentation page [on the
website](https://uofuepibio.github.io/epiworldpy).

## Existing Alternatives

Several alternatives to epiworldPy exist and provide researchers with a
range of options, each with its own unique features and strengths,
enabling the exploration and analysis of infectious disease dynamics
through agent-based modeling. For convenience, the below table is
orthogonal to the table on the epiworldR repository.

| Package / Framework | Multiple Viruses (support \>1 pathogen) | Multiple Tools (e.g. interventions, contact tracing, NPIs, behavior etc.) | Multiple Runs (i.e. ability to run many replicates, parameter sweeps) | Global Actions (e.g. policies that affect everyone / global interventions) | Built-in Epidemiological Models (SIR, SEIR, etc.) | Dependencies / Notes | Activity / Maturity |
|----|----|----|----|----|----|----|----|
| [**epiworldPy**](https://github.com/UofUEpiBio/epiworldpy) | yes | yes | yes | yes | yes | Python ≥ 3.7; available on PyPI; supports parallel simulations | Active; wrapper of epiworld C++ library |
| [**Mesa**](https://mesa.readthedocs.io/) | no (by default; you’d implement multiple pathogens yourself) | yes (you can code interventions etc.) | yes (you can script many runs) | yes (you can apply global interventions) | no built-in epidemic models per se; it’s a general ABM toolkit | Python, uses standard scientific stack (NumPy, etc.); has visualization components | Active, widely used; good documentation |
| [**AgentPy**](https://agentpy.readthedocs.io/) | not clearly documented multiple virus support | yes | yes | yes | no default epidemic models; more general ABM framework | Python, integrates with Jupyter etc. | Relatively newer but seems stable |
| [**Epiabm**](https://github.com/SABS-R3-Epidemiology/epiabm) | probably no multiple viruses (CovidSim / SEIR base is for one pathogen) | yes (modular scenarios, age, spatial heterogeneity, interventions) | yes | yes (global NPIs etc.) | yes (SEIR, more compartments) | Python & C++ backends; modular; has docs, tests | Recently published, growing |
| [**PyRoss**](https://arxiv.org/abs/2005.09625) | not clearly multiple viruses (focus seems single disease with structure) | yes (structured contact matrices, NPIs etc.) | yes | yes | yes (age structured ODE/compartmental etc.) | Python, requires numpy, scipy etc. | Relatively mature; used in COVID work |
| [**BESSIE**](https://arxiv.org/abs/2203.11414) | ambiguous / not clearly multiple viruses; seems built for one epidemic at a time | yes (behavior, synthetic population, interventions) | yes | yes (policies / behavior modules) | yes (standard epidemic spread etc.) | Python; synthetic populations, behavior plugins | Active, though specific to research contexts |
| [**Epydemix**](https://www.medrxiv.org/content/10.1101/2025.05.07.25327151v1.full.pdf) | unclear; seems more about epidemic modelling tasks than full ABM with multiple pathogens | yes (data analysis, simulation, visualization) | yes | likely supports global interventions, but unclear | yes (epidemic models) | Python package, uses scientific stack | Newer; preprint stage; less ABM complexity |

# Code of Conduct

The epiworldPy project is released with a Contributor Code of Conduct.
By contributing to this project, you agree to abide by its terms.
