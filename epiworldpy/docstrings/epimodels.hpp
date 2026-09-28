#ifndef EPIWORLDPY_DOCSTRINGS_EPIMODELS_HPP
#define EPIWORLDPY_DOCSTRINGS_EPIMODELS_HPP

// NumPy-style docstrings for the models in epiworldpy.epimodels (see
// tests/test_docstrings.py). The text follows epiworldR's documentation.

// clang-format off
namespace epiworldpy::docstrings::epimodels {

// Shared pieces, spliced in with string literal concatenation.
#define EPIWORLDPY_DOC_NETWORK                                                 \
    "The model has no agents until a population is added, for example with\n"  \
    ":meth:`~epiworldpy.Model.agents_smallworld`.\n"

#define EPIWORLDPY_DOC_CONNECTED                                                 \
    "Every agent can contact every other agent (a well-mixed population), so\n"  \
    "no network is needed: the model creates its ``n`` agents itself. This is\n" \
    "equivalent to a compartmental model.\n"

#define EPIWORLDPY_DOC_MIXING                                                    \
    "Agents are split into groups with :class:`~epiworldpy.Entity` objects\n"    \
    "(add them with :meth:`~epiworldpy.Model.add_entity` before running), and\n" \
    "contacts between groups follow ``contact_matrix``. The model creates its\n" \
    "``n`` agents itself.\n"

#define EPIWORLDPY_DOC_CONTACT_MATRIX                                             \
    "contact_matrix : list of float\n"                                            \
    "    Flattened ``G x G`` matrix, where ``G`` is the number of entities;\n"    \
    "    entry ``(i, j)`` is the expected number of daily contacts an agent in\n" \
    "    group ``i`` has with agents in group ``j``, so the row sums are the\n"   \
    "    expected contacts per day of each group. The matrix is stored by\n"      \
    "    columns: flatten a NumPy array with ``m.flatten(order=\"F\")``.\n"

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
>>> covid = epimodels.ModelSIR("COVID-19", prevalence=0.01,
...                            transmission_rate=0.9, recovery_rate=0.1)
>>> covid.agents_smallworld(n=1000, k=5, d=False, p=0.01)
>>> covid.run(ndays=100, seed=1912)
)doc";

inline constexpr const char *sir_init = R"doc(Create a SIR model.

Parameters
----------
name : str
    Name of the virus.
prevalence : float
    Initial proportion of agents with the virus.
transmission_rate : float
    Probability of transmission per contact, between 0 and 1.
recovery_rate : float
    Daily probability of recovery, between 0 and 1.
)doc";

inline constexpr const char *sirconn =
	R"doc(Susceptible-Infected-Recovered (SIR) model in a connected population.

States: ``Susceptible``, ``Infected``, ``Recovered``.

)doc" EPIWORLDPY_DOC_CONNECTED R"doc(
Examples
--------
>>> from epiworldpy import epimodels
>>> covid = epimodels.ModelSIRCONN("COVID-19", n=10000, prevalence=0.01,
...                                contact_rate=5, transmission_rate=0.4,
...                                recovery_rate=0.95)
>>> covid.run(ndays=100, seed=1912)
)doc";

inline constexpr const char *sirconn_init = R"doc(Create a connected SIR model.

Parameters
----------
name : str
    Name of the virus.
n : int
    Number of agents.
prevalence : float
    Initial proportion of agents with the virus.
contact_rate : float
    Average number of contacts per agent per day.
transmission_rate : float
    Probability of transmission per contact, between 0 and 1.
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
prevalence : float
    Initial proportion of agents with the virus.
transmission_rate : float
    Probability of transmission per contact, between 0 and 1.
recovery_rate : float
    Daily probability of recovery, between 0 and 1.
death_rate : float
    Daily probability of death, between 0 and 1.
)doc";

inline constexpr const char *sirdconn =
	R"doc(Susceptible-Infected-Recovered-Deceased (SIRD) model in a connected population.

States: ``Susceptible``, ``Infected``, ``Recovered``, ``Deceased``.

)doc" EPIWORLDPY_DOC_CONNECTED;

inline constexpr const char *sirdconn_init =
	R"doc(Create a connected SIRD model.

Parameters
----------
name : str
    Name of the virus.
n : int
    Number of agents.
prevalence : float
    Initial proportion of agents with the virus.
contact_rate : float
    Average number of contacts per agent per day.
transmission_rate : float
    Probability of transmission per contact, between 0 and 1.
recovery_rate : float
    Daily probability of recovery, between 0 and 1.
death_rate : float
    Daily probability of death, between 0 and 1.
)doc";

inline constexpr const char *sirmixing =
	R"doc(Susceptible-Infected-Recovered (SIR) model with mixing between groups.

States: ``Susceptible``, ``Infected``, ``Recovered``.

)doc" EPIWORLDPY_DOC_MIXING;

inline constexpr const char *sirmixing_init =
	R"doc(Create a SIR model with mixing.

Parameters
----------
vname : str
    Name of the virus.
n : int
    Number of agents.
prevalence : float
    Initial proportion of agents with the virus.
transmission_rate : float
    Probability of transmission per contact, between 0 and 1.
recovery_rate : float
    Daily probability of recovery, between 0 and 1.
)doc" EPIWORLDPY_DOC_CONTACT_MATRIX;

// SEIR family --------------------------------------------------------------

inline constexpr const char *seir =
	R"doc(Susceptible-Exposed-Infected-Recovered (SEIR) model on a network.

States: ``Susceptible``, ``Exposed``, ``Infected``, ``Removed``. Exposed
agents carry the virus but cannot transmit it yet.

)doc" EPIWORLDPY_DOC_NETWORK R"doc(
Examples
--------
>>> from epiworldpy import epimodels
>>> covid = epimodels.ModelSEIR("COVID-19", prevalence=0.01,
...                             transmission_rate=0.9, incubation_days=4,
...                             recovery_rate=0.1)
>>> covid.agents_smallworld(n=1000, k=5, d=False, p=0.01)
>>> covid.run(ndays=100, seed=1912)
)doc";

inline constexpr const char *seir_init = R"doc(Create a SEIR model.

Parameters
----------
name : str
    Name of the virus.
prevalence : float
    Initial proportion of agents with the virus.
transmission_rate : float
    Probability of transmission per contact, between 0 and 1.
incubation_days : float
    Average number of days in the exposed state; greater than 0.
recovery_rate : float
    Daily probability of recovery, between 0 and 1.
)doc";

inline constexpr const char *seirconn =
	R"doc(Susceptible-Exposed-Infected-Recovered (SEIR) model in a connected population.

States: ``Susceptible``, ``Exposed``, ``Infected``, ``Recovered``.

)doc" EPIWORLDPY_DOC_CONNECTED R"doc(
Examples
--------
>>> from epiworldpy import epimodels
>>> covid = epimodels.ModelSEIRCONN("COVID-19", n=10000, prevalence=0.01,
...                                 contact_rate=2, transmission_rate=0.5,
...                                 incubation_days=7, recovery_rate=0.3)
>>> covid.run(ndays=100, seed=1912)
)doc";

inline constexpr const char *seirconn_init =
	R"doc(Create a connected SEIR model.

Parameters
----------
name : str
    Name of the virus.
n : int
    Number of agents.
prevalence : float
    Initial proportion of agents with the virus.
contact_rate : float
    Average number of contacts per agent per day.
transmission_rate : float
    Probability of transmission per contact, between 0 and 1.
incubation_days : float
    Average number of days in the exposed state; greater than 0.
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
prevalence : float
    Initial proportion of agents with the virus.
transmission_rate : float
    Probability of transmission per contact, between 0 and 1.
incubation_days : float
    Average number of days in the exposed state; greater than 0.
recovery_rate : float
    Daily probability of recovery, between 0 and 1.
death_rate : float
    Daily probability of death, between 0 and 1.
)doc";

inline constexpr const char *seirdconn =
	R"doc(Susceptible-Exposed-Infected-Recovered-Deceased (SEIRD) model in a connected population.

States: ``Susceptible``, ``Exposed``, ``Infected``, ``Removed``,
``Deceased``.

)doc" EPIWORLDPY_DOC_CONNECTED;

inline constexpr const char *seirdconn_init =
	R"doc(Create a connected SEIRD model.

Parameters
----------
name : str
    Name of the virus.
n : int
    Number of agents.
prevalence : float
    Initial proportion of agents with the virus.
contact_rate : float
    Average number of contacts per agent per day.
transmission_rate : float
    Probability of transmission per contact, between 0 and 1.
incubation_days : float
    Average number of days in the exposed state; greater than 0.
recovery_rate : float
    Daily probability of recovery, between 0 and 1.
death_rate : float
    Daily probability of death, between 0 and 1.
)doc";

inline constexpr const char *seirmixing =
	R"doc(Susceptible-Exposed-Infected-Recovered (SEIR) model with mixing between groups.

States: ``Susceptible``, ``Exposed``, ``Infected``, ``Recovered``.

)doc" EPIWORLDPY_DOC_MIXING;

inline constexpr const char *seirmixing_init =
	R"doc(Create a SEIR model with mixing.

Parameters
----------
vname : str
    Name of the virus.
n : int
    Number of agents.
prevalence : float
    Initial proportion of agents with the virus.
transmission_rate : float
    Probability of transmission per contact, between 0 and 1.
avg_incubation_days : float
    Average number of days in the exposed state; greater than 0.
recovery_rate : float
    Daily probability of recovery, between 0 and 1.
)doc" EPIWORLDPY_DOC_CONTACT_MATRIX;

#define EPIWORLDPY_DOC_QUARANTINE_PARAMS                                         \
    "hospitalization_rate : float\n"                                             \
    "    Daily probability that an infected agent is hospitalized.\n"            \
    "hospitalization_period : float\n"                                           \
    "    Average number of days in the hospital.\n"                              \
    "days_undetected : float\n"                                                  \
    "    Average number of days an infection goes undetected.\n"                 \
    "quarantine_period : int\n"                                                  \
    "    Number of days in quarantine. A negative value turns quarantine off.\n" \
    "quarantine_willingness : float\n"                                           \
    "    Proportion of agents willing to quarantine.\n"                          \
    "isolation_willingness : float\n"                                            \
    "    Proportion of agents willing to isolate.\n"                             \
    "isolation_period : int\n"                                                   \
    "    Number of days in isolation. A negative value turns isolation off.\n"   \
    "contact_tracing_success_rate : float, default 1.0\n"                        \
    "    Probability that a contact is traced.\n"                                \
    "contact_tracing_days_prior : int, default 4\n"                              \
    "    Number of days before detection during which contacts are traced.\n"

inline constexpr const char *seirmixingquarantine =
	R"doc(SEIR model with mixing, quarantine, isolation, hospitalization, and contact tracing.

Infected agents are isolated once detected, and the agents they were in
contact with are traced and quarantined.

States: ``Susceptible``, ``Exposed``, ``Infected``, ``Isolated``,
``Quarantined Susceptible``, ``Quarantined Exposed``, ``Isolated Recovered``,
``Hospitalized``, ``Recovered``.

)doc" EPIWORLDPY_DOC_MIXING;

inline constexpr const char *seirmixingquarantine_init =
	R"doc(Create a SEIR model with mixing and quarantine.

Parameters
----------
vname : str
    Name of the virus.
n : int
    Number of agents.
prevalence : float
    Initial proportion of agents with the virus.
transmission_rate : float
    Probability of transmission per contact, between 0 and 1.
avg_incubation_days : float
    Average number of days in the exposed state; greater than 0.
recovery_rate : float
    Daily probability of recovery, between 0 and 1.
)doc" EPIWORLDPY_DOC_CONTACT_MATRIX EPIWORLDPY_DOC_QUARANTINE_PARAMS;

inline constexpr const char *seirnetworkquarantine =
	R"doc(SEIR model on a network, with quarantine, isolation, hospitalization, and contact tracing.

Like :class:`ModelSEIRMixingQuarantine`, but contacts are the agents'
neighbors in the network (for example, one built with
:meth:`~epiworldpy.Model.agents_sbm`) instead of a contact matrix.

States: ``Susceptible``, ``Exposed``, ``Infected``, ``Isolated``,
``Detected Hospitalized``, ``Quarantined Susceptible``,
``Quarantined Exposed``, ``Isolated Recovered``, ``Hospitalized``,
``Recovered``.

)doc" EPIWORLDPY_DOC_NETWORK;

inline constexpr const char *seirnetworkquarantine_init =
	R"doc(Create a SEIR network model with quarantine.

Parameters
----------
vname : str
    Name of the virus.
prevalence : float
    Initial proportion of agents with the virus.
transmission_rate : float
    Probability of transmission per contact, between 0 and 1.
avg_incubation_days : float
    Average number of days in the exposed state; greater than 0.
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
prevalence : float
    Initial proportion of agents with the virus.
transmission_rate : float
    Probability of transmission per contact, between 0 and 1.
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
prevalence : float
    Initial proportion of agents with the virus.
transmission_rate : float
    Probability of transmission per contact, between 0 and 1.
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
transmission_rate : float
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
#undef EPIWORLDPY_DOC_CONNECTED
#undef EPIWORLDPY_DOC_MIXING
#undef EPIWORLDPY_DOC_CONTACT_MATRIX
#undef EPIWORLDPY_DOC_QUARANTINE_PARAMS

} // namespace epiworldpy::docstrings::epimodels
// clang-format on

#endif
