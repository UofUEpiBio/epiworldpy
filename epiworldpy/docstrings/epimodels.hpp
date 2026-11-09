#ifndef EPIWORLDPY_DOCSTRINGS_EPIMODELS_HPP
#define EPIWORLDPY_DOCSTRINGS_EPIMODELS_HPP

// NumPy-style docstrings for epiworldpy.epimodels (see
// tests/test_docstrings.py). The text follows epiworldR's documentation.

// clang-format off
namespace epiworldpy::docstrings::epimodels {

inline constexpr const char *module = R"doc(Ready-to-use epidemiological models, from the model registry.

Every built-in model whose arguments are numbers is built by
:func:`make_model`, from its id and its arguments' values;
:func:`list_models` lists them and :func:`get_model_info` describes their
arguments. The named constructors (:func:`ModelSIR`, :func:`ModelSEIR`, ...)
call it. Each model comes with its parameters and virus, so it only needs
contacts (a layer, see :mod:`epiworldpy.layer`) and a call to
:meth:`~epiworldpy.Model.run`. A virus added to it is a strain of the model's
own (see :meth:`~epiworldpy.Model.add_virus`).
)doc";

inline constexpr const char *make_model = R"doc(Build a model from the registry.

The model has its parameters and virus, and no layers (add them with
:meth:`~epiworldpy.Model.add_layer`).

Parameters
----------
id : str
    The model, as in :func:`list_models` (e.g., ``"SIR"``).
values : dict of str to float, default {}
    Argument values, by their names in ``get_model_info(id)["params"]``
    (``"arg"``). The arguments left out take their defaults. The values are
    checked against each argument's limits.
virus_name : str, default ""
    The name of the model's virus (``""``: the model's default).

Returns
-------
epiworldpy.Model
    The model.
)doc";

inline constexpr const char *list_models = R"doc(List the models of the registry.

Returns
-------
list of dict
    One dict per model: ``id``, ``label``, ``description``, and
    ``virus_name`` (its default virus name).
)doc";

inline constexpr const char *get_model_info = R"doc(Describe a model of the registry and its arguments.

Parameters
----------
id : str
    The model, as in :func:`list_models`.

Returns
-------
dict
    The fields of :func:`list_models`, and ``params``: one dict per
    argument, with its name (``arg``), the model parameter it sets
    (``name``, empty for ``n`` and ``prevalence``), its default
    (``value``), its limits (``min``, ``max``), whether it is a whole number
    (``integer``), and a ``description``.
)doc";

// The named constructors ---------------------------------------------------

inline constexpr const char *sir = R"doc(Susceptible-Infected-Recovered (SIR) model on a network.

Stages: ``Susceptible``, ``Infected``, ``Recovered``.

The model has ``n`` agents and no contacts until a layer (a network, a
mixing layer, or several) is added with
:meth:`~epiworldpy.Model.add_layer` (see :mod:`epiworldpy.layer`).

Parameters
----------
name : str
    Name of the virus.
n : int
    Number of agents.
prevalence : float
    Initial proportion of agents with the virus.
transmission_prob : float
    Probability that a contact with an infected agent infects, between 0 and
    1 ("Transmission probability").
recovery_rate : float
    Daily probability of recovery, between 0 and 1.

Returns
-------
epiworldpy.Model
    The model, built by :func:`make_model`.

Examples
--------
>>> from epiworldpy import epimodels
>>> from epiworldpy import graph, layer
>>> covid = epimodels.ModelSIR("COVID-19", n=1000, prevalence=0.01,
...                            transmission_prob=0.9, recovery_rate=0.1)
>>> covid.add_layer(layer.network("contacts", graph.smallworld(k=5, p=0.01)))
>>> covid.run(ndays=100, seed=1912)
)doc";

inline constexpr const char *sird = R"doc(Susceptible-Infected-Recovered-Deceased (SIRD) model on a network.

Stages: ``Susceptible``, ``Infected``, ``Recovered``, ``Deceased``.

The model has ``n`` agents and no contacts until a layer (a network, a
mixing layer, or several) is added with
:meth:`~epiworldpy.Model.add_layer` (see :mod:`epiworldpy.layer`).

Parameters
----------
name : str
    Name of the virus.
n : int
    Number of agents.
prevalence : float
    Initial proportion of agents with the virus.
transmission_prob : float
    Probability that a contact with an infected agent infects, between 0 and
    1 ("Transmission probability").
recovery_rate : float
    Daily probability of recovery, between 0 and 1.
death_rate : float
    Daily probability of death, between 0 and 1.

Returns
-------
epiworldpy.Model
    The model, built by :func:`make_model`.
)doc";

inline constexpr const char *seir = R"doc(Susceptible-Exposed-Infected-Recovered (SEIR) model on a network.

Stages: ``Susceptible``, ``Exposed``, ``Infected``, ``Removed``. Exposed
agents carry the virus but cannot transmit it yet.

The model has ``n`` agents and no contacts until a layer (a network, a
mixing layer, or several) is added with
:meth:`~epiworldpy.Model.add_layer` (see :mod:`epiworldpy.layer`).

Parameters
----------
name : str
    Name of the virus.
n : int
    Number of agents.
prevalence : float
    Initial proportion of agents with the virus.
transmission_prob : float
    Probability that a contact with an infected agent infects, between 0 and
    1 ("Transmission probability").
latent_period : float
    Mean days in the exposed stage (infected, not yet infectious); greater
    than 0 ("Latent period").
recovery_rate : float
    Daily probability of recovery, between 0 and 1.

Returns
-------
epiworldpy.Model
    The model, built by :func:`make_model`.

Examples
--------
>>> from epiworldpy import epimodels
>>> from epiworldpy import graph, layer
>>> covid = epimodels.ModelSEIR("COVID-19", n=1000, prevalence=0.01,
...                             transmission_prob=0.9, latent_period=4,
...                             recovery_rate=0.1)
>>> covid.add_layer(layer.network("contacts", graph.smallworld(k=5, p=0.01)))
>>> covid.run(ndays=100, seed=1912)
)doc";

inline constexpr const char *seird = R"doc(Susceptible-Exposed-Infected-Recovered-Deceased (SEIRD) model on a network.

Stages: ``Susceptible``, ``Exposed``, ``Infected``, ``Removed``,
``Deceased``.

The model has ``n`` agents and no contacts until a layer (a network, a
mixing layer, or several) is added with
:meth:`~epiworldpy.Model.add_layer` (see :mod:`epiworldpy.layer`).

Parameters
----------
name : str
    Name of the virus.
n : int
    Number of agents.
prevalence : float
    Initial proportion of agents with the virus.
transmission_prob : float
    Probability that a contact with an infected agent infects, between 0 and
    1 ("Transmission probability").
latent_period : float
    Mean days in the exposed stage (infected, not yet infectious); greater
    than 0 ("Latent period").
recovery_rate : float
    Daily probability of recovery, between 0 and 1.
death_rate : float
    Daily probability of death, between 0 and 1.

Returns
-------
epiworldpy.Model
    The model, built by :func:`make_model`.
)doc";

inline constexpr const char *seirquarantine = R"doc(SEIR model with quarantine, isolation, hospitalization, and contact tracing.

Its contacts come from its layers, of any kind (networks, mixing by group, or
both). Infected agents are isolated once detected (if willing), and their
contacts in every layer are traced and quarantined (if willing): at the start
of every run the model turns contact tracing on for each layer that does not
have it. Only Infected agents transmit; isolated, quarantined, and
hospitalized agents are out of the mixing layers altogether.

Stages: ``Susceptible``, ``Exposed``, ``Infected``, ``Severe``,
``Recovered``. Statuses: ``Free``, ``Quarantined``, ``Isolated``,
``Hospitalized`` (see :meth:`~epiworldpy.Model.get_statuses`).

The model has ``n`` agents and no contacts until a layer (a network, a
mixing layer, or several) is added with
:meth:`~epiworldpy.Model.add_layer` (see :mod:`epiworldpy.layer`).

Parameters
----------
name : str
    Name of the virus.
n : int
    Number of agents.
prevalence : float
    Initial proportion of agents with the virus.
transmission_prob : float
    Probability that a contact with an infected agent infects, between 0 and
    1 ("Transmission probability").
latent_period : float
    Mean days in the exposed stage (infected, not yet infectious); greater
    than 0 ("Latent period").
recovery_rate : float
    Daily probability of recovery, between 0 and 1.
hospitalization_rate : float
    Daily probability that an infected agent is hospitalized.
hospitalization_period : float
    Average number of days in the hospital.
detection_delay : float
    Mean days infected before detection; 0 detects at once, and a
    negative value never ("Detection delay").
quarantine_period : int
    Number of days in quarantine. A negative value turns quarantine off.
quarantine_willingness : float
    Proportion of agents willing to quarantine.
isolation_willingness : float
    Proportion of agents willing to isolate.
isolation_period : int
    Number of days in isolation. A negative value turns isolation off.
contact_tracing_prob : float, default 1.0
    Probability that a contact is traced ("Contact tracing probability").
contact_tracing_window : int, default 4
    Days before detection whose contacts are traced ("Contact tracing
    window").

Returns
-------
epiworldpy.Model
    The model, built by :func:`make_model`.
)doc";

inline constexpr const char *sis = R"doc(Susceptible-Infected-Susceptible (SIS) model on a network.

Stages: ``Susceptible``, ``Infected``. Recovered agents become susceptible
again.

The model has ``n`` agents and no contacts until a layer (a network, a
mixing layer, or several) is added with
:meth:`~epiworldpy.Model.add_layer` (see :mod:`epiworldpy.layer`).

Parameters
----------
name : str
    Name of the virus.
n : int
    Number of agents.
prevalence : float
    Initial proportion of agents with the virus.
transmission_prob : float
    Probability that a contact with an infected agent infects, between 0 and
    1 ("Transmission probability").
recovery_rate : float
    Daily probability of recovery, between 0 and 1.

Returns
-------
epiworldpy.Model
    The model, built by :func:`make_model`.
)doc";

inline constexpr const char *sisd = R"doc(Susceptible-Infected-Susceptible-Deceased (SISD) model on a network.

Stages: ``Susceptible``, ``Infected``, ``Deceased``. Recovered agents become
susceptible again.

The model has ``n`` agents and no contacts until a layer (a network, a
mixing layer, or several) is added with
:meth:`~epiworldpy.Model.add_layer` (see :mod:`epiworldpy.layer`).

Parameters
----------
name : str
    Name of the virus.
n : int
    Number of agents.
prevalence : float
    Initial proportion of agents with the virus.
transmission_prob : float
    Probability that a contact with an infected agent infects, between 0 and
    1 ("Transmission probability").
recovery_rate : float
    Daily probability of recovery, between 0 and 1.
death_rate : float
    Daily probability of death, between 0 and 1.

Returns
-------
epiworldpy.Model
    The model, built by :func:`make_model`.
)doc";

inline constexpr const char *surv = R"doc(Surveillance (SURV) model, where agents may be tested and isolated even without symptoms.

A share of the population is vaccinated, infected agents may or may not
develop symptoms, and agents are tested at random.

Stages: ``Susceptible``, ``Latent``, ``Symptomatic``,
``Symptomatic isolated``, ``Asymptomatic``, ``Asymptomatic isolated``,
``Recovered``, ``Removed``.

The model has ``n`` agents and no contacts until a layer (a network, a
mixing layer, or several) is added with
:meth:`~epiworldpy.Model.add_layer` (see :mod:`epiworldpy.layer`).

Parameters
----------
name : str
    Name of the virus.
n : int
    Number of agents.
prevalence : int
    Initial number of agents with the virus.
efficacy_vax : float
    Efficacy of the vaccine: 1 minus the probability that a vaccinated agent
    acquires the disease.
latent_period : float
    Shape of a Gamma(latent_period, 1) distribution: the expected number of
    latent days.
infect_period : float
    Shape of a Gamma(infect_period, 1) distribution: the expected number of
    infectious days.
prob_symptoms : float
    Probability of developing symptoms.
prop_vaccinated : float
    Proportion of agents vaccinated at the start.
prop_vax_redux_transm : float
    Factor by which the vaccine reduces transmissibility.
prop_vax_redux_infect : float
    Factor by which the vaccine reduces the chance of becoming infected.
surveillance_prob : float
    Daily probability of testing an agent.
transmission_prob : float
    Probability of transmission per contact.
prob_death : float
    Daily probability of death for symptomatic agents.
prob_noreinfect : float
    Probability that a recovered agent cannot be infected again.

Returns
-------
epiworldpy.Model
    The model, built by :func:`make_model`.
)doc";

// Models with their own constructor -----------------------------------------

inline constexpr const char *diffnet = R"doc(Network diffusion model: adoption of a behavior spread through a network.

Unlike an epidemic model, the probability that an agent adopts depends on how
many of its neighbors have adopted (its exposure), not on transmission from a
single neighbor:

    P(adopt) = logit^-1(prob_adopt + params * data + exposure)

Stages: ``Non adopters``, ``Adopters``.

The model has ``n`` agents and no contacts until a layer (a network, a
mixing layer, or several) is added with
:meth:`~epiworldpy.Model.add_layer` (see :mod:`epiworldpy.layer`).
)doc";

inline constexpr const char *diffnet_init = R"doc(Create a network diffusion model.

Parameters
----------
name : str
    Name of the behavior (innovation).
n : int
    Number of agents.
prevalence : float
    Initial proportion of adopters.
prob_adopt : float
    Baseline probability of adoption.
normalize_exposure : bool
    Whether to divide the exposure by the number of neighbors.
data : float
    Agent covariates. Not supported from Python yet: the model keeps a
    pointer to this value, which does not outlive the call. Pass ``0.0``
    with ``data_ncols=0``.
data_ncols : int
    Number of covariate columns in ``data``.
data_cols : list of int
    Indices of the covariates used in the adoption probability.
params : list of float
    Coefficient of each covariate in ``data_cols``.
)doc";

} // namespace epiworldpy::docstrings::epimodels

// clang-format on

#endif /* EPIWORLDPY_DOCSTRINGS_EPIMODELS_HPP */
