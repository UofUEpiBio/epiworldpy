#ifndef EPIWORLDPY_DOCSTRINGS_EPIMODELS_HPP
#define EPIWORLDPY_DOCSTRINGS_EPIMODELS_HPP

// NumPy-style docstrings for the models in epiworldpy.epimodels (see
// tests/test_docstrings.py). The text follows epiworldR's documentation.

// clang-format off
namespace epiworldpy::docstrings::epimodels {

// Shared pieces, spliced in with string literal concatenation.
#define EPIWORLDPY_DOC_NETWORK                                                 \
    "The model has ``n`` agents and no contacts until a layer (a network, a\n" \
    "mixing layer, or several) is added with\n"                              \
    ":meth:`~epiworldpy.Model.add_layer` (see :mod:`epiworldpy.layer`).\n"

inline constexpr const char *module = R"doc(Ready-to-use epidemiological models.

Each class sets up its states, parameters, and virus, so it only needs a
population (for network models) and a call to :meth:`~epiworldpy.Model.run`.
All of them are :class:`~epiworldpy.Model` subclasses.
)doc";

// SIR family ---------------------------------------------------------------

inline constexpr const char *sir =
	R"doc(Susceptible-Infected-Recovered (SIR) model on a network.

States: ``Susceptible``, ``Infected``, ``Recovered``.

)doc" EPIWORLDPY_DOC_NETWORK R"doc(
Examples
--------
>>> from epiworldpy import epimodels
>>> from epiworldpy import graph, layer
>>> covid = epimodels.ModelSIR("COVID-19", n=1000, prevalence=0.01,
...                            transmission_prob=0.9, recovery_rate=0.1)
>>> covid.add_layer(layer.network("contacts", graph.smallworld(k=5, p=0.01)))
>>> covid.run(ndays=100, seed=1912)
)doc";

inline constexpr const char *sir_init = R"doc(Create a SIR model.

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
)doc";

inline constexpr const char *sird =
	R"doc(Susceptible-Infected-Recovered-Deceased (SIRD) model on a network.

States: ``Susceptible``, ``Infected``, ``Recovered``, ``Deceased``.

)doc" EPIWORLDPY_DOC_NETWORK;

inline constexpr const char *sird_init = R"doc(Create a SIRD model.

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
)doc";

// SEIR family --------------------------------------------------------------

inline constexpr const char *seir =
	R"doc(Susceptible-Exposed-Infected-Recovered (SEIR) model on a network.

States: ``Susceptible``, ``Exposed``, ``Infected``, ``Removed``. Exposed
agents carry the virus but cannot transmit it yet.

)doc" EPIWORLDPY_DOC_NETWORK R"doc(
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

inline constexpr const char *seir_init = R"doc(Create a SEIR model.

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
    Mean days in the exposed state (infected, not yet infectious); greater
    than 0 ("Latent period").
recovery_rate : float
    Daily probability of recovery, between 0 and 1.
)doc";

inline constexpr const char *seird =
	R"doc(Susceptible-Exposed-Infected-Recovered-Deceased (SEIRD) model on a network.

States: ``Susceptible``, ``Exposed``, ``Infected``, ``Removed``,
``Deceased``.

)doc" EPIWORLDPY_DOC_NETWORK;

inline constexpr const char *seird_init = R"doc(Create a SEIRD model.

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
    Mean days in the exposed state (infected, not yet infectious); greater
    than 0 ("Latent period").
recovery_rate : float
    Daily probability of recovery, between 0 and 1.
death_rate : float
    Daily probability of death, between 0 and 1.
)doc";

#define EPIWORLDPY_DOC_QUARANTINE_PARAMS                                         \
    "hospitalization_rate : float\n"                                             \
    "    Daily probability that an infected agent is hospitalized.\n"            \
    "hospitalization_period : float\n"                                           \
    "    Average number of days in the hospital.\n"                              \
    "detection_delay : float\n"                                                  \
    "    Mean days infected before detection; 0 detects at once, and a\n"        \
    "    negative value never (\"Detection delay\").\n"                          \
    "quarantine_period : int\n"                                                  \
    "    Number of days in quarantine. A negative value turns quarantine off.\n" \
    "quarantine_willingness : float\n"                                           \
    "    Proportion of agents willing to quarantine.\n"                          \
    "isolation_willingness : float\n"                                            \
    "    Proportion of agents willing to isolate.\n"                             \
    "isolation_period : int\n"                                                   \
    "    Number of days in isolation. A negative value turns isolation off.\n"   \
    "contact_tracing_prob : float, default 1.0\n"                                \
    "    Probability that a contact is traced (\"Contact tracing probability\").\n" \
    "contact_tracing_window : int, default 4\n"                                  \
    "    Days before detection whose contacts are traced (\"Contact tracing\n"     \
    "    window\").\n"

inline constexpr const char *seirquarantine =
	R"doc(SEIR model with quarantine, isolation, hospitalization, and contact tracing.

Its contacts come from its layers, of any kind (networks, mixing by group, or
both). Infected agents are isolated once detected (if willing), and their
contacts in every layer are traced and quarantined (if willing): at the start
of every run the model turns contact tracing on for each layer that does not
have it. Only Infected agents transmit; isolated, quarantined, and
hospitalized agents are out of the mixing layers altogether.

States: ``Susceptible``, ``Exposed``, ``Infected``, ``Isolated``,
``Detected Hospitalized``, ``Quarantined Susceptible``,
``Quarantined Exposed``, ``Isolated Recovered``, ``Hospitalized``,
``Recovered``.

)doc" EPIWORLDPY_DOC_NETWORK;

inline constexpr const char *seirquarantine_init =
	R"doc(Create a SEIR model with quarantine.

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
    Mean days in the exposed state (infected, not yet infectious); greater
    than 0 ("Latent period").
recovery_rate : float
    Daily probability of recovery, between 0 and 1.
)doc" EPIWORLDPY_DOC_QUARANTINE_PARAMS;

// SIS family ---------------------------------------------------------------

inline constexpr const char *sis =
	R"doc(Susceptible-Infected-Susceptible (SIS) model on a network.

States: ``Susceptible``, ``Infected``. Recovered agents become susceptible
again.

)doc" EPIWORLDPY_DOC_NETWORK;

inline constexpr const char *sis_init = R"doc(Create a SIS model.

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
)doc";

inline constexpr const char *sisd =
	R"doc(Susceptible-Infected-Susceptible-Deceased (SISD) model on a network.

States: ``Susceptible``, ``Infected``, ``Deceased``. Recovered agents become
susceptible again.

)doc" EPIWORLDPY_DOC_NETWORK;

inline constexpr const char *sisd_init = R"doc(Create a SISD model.

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
)doc";

// Other models -------------------------------------------------------------

inline constexpr const char *surv =
	R"doc(Surveillance (SURV) model, where agents may be tested and isolated even without symptoms.

A share of the population is vaccinated, infected agents may or may not
develop symptoms, and agents are tested at random.

States: ``Susceptible``, ``Latent``, ``Symptomatic``,
``Symptomatic isolated``, ``Asymptomatic``, ``Asymptomatic isolated``,
``Recovered``, ``Removed``.

)doc" EPIWORLDPY_DOC_NETWORK;

inline constexpr const char *surv_init = R"doc(Create a SURV model.

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
)doc";

inline constexpr const char *diffnet =
	R"doc(Network diffusion model: adoption of a behavior spread through a network.

Unlike an epidemic model, the probability that an agent adopts depends on how
many of its neighbors have adopted (its exposure), not on transmission from a
single neighbor:

    P(adopt) = logit^-1(prob_adopt + params * data + exposure)

States: ``Non adopters``, ``Adopters``.

)doc" EPIWORLDPY_DOC_NETWORK;

inline constexpr const char *diffnet_init =
	R"doc(Create a network diffusion model.

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

#undef EPIWORLDPY_DOC_NETWORK
#undef EPIWORLDPY_DOC_QUARANTINE_PARAMS

} // namespace epiworldpy::docstrings::epimodels
// clang-format on

#endif
