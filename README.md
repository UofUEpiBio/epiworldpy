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
