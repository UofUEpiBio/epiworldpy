#ifndef EPIWORLDPY_DOCSTRINGS_MODEL_HPP
#define EPIWORLDPY_DOCSTRINGS_MODEL_HPP

// NumPy-style docstrings for epiworldpy.Model, epiworldpy.UpdateFun, and
// epiworldpy.NativeUpdateFun (see tests/test_docstrings.py).

// clang-format off
namespace epiworldpy::docstrings::model {

inline constexpr const char *cls =
	R"doc(An agent-based model: a population, its states, viruses, and tools.

``Model`` is the base class of every model in :mod:`epiworldpy.epimodels`.
Those come with their states, parameters, and virus already set up, so a
typical workflow only needs a population and a call to :meth:`run`::

    from epiworldpy import epimodels

    covid = epimodels.ModelSIR("COVID-19", prevalence=0.01,
                               transmission_rate=0.9, recovery_rate=0.1)
    covid.agents_smallworld(n=1000, k=5, d=False, p=0.01)
    covid.run(ndays=100, seed=1912)
    covid.get_db().get_hist_total()

An empty ``Model()`` can be used to build a custom model with
:meth:`add_state`, :meth:`add_virus`, and :meth:`add_tool`.
)doc";

inline constexpr const char *init =
	R"doc(Create an empty model with no states, agents, viruses, or tools.
)doc";

inline constexpr const char *add_state_native =
	R"doc(Add a state updated by a built-in epiworld function.

Parameters
----------
lab : str
    Label of the new state.
fun : NativeUpdateFun
    Update function for agents in this state, from
    :meth:`UpdateFun.susceptible` or :meth:`UpdateFun.rate`.

Returns
-------
int
    Index of the new state.
)doc";

inline constexpr const char *add_state = R"doc(Add a state to the model.

Parameters
----------
lab : str
    Label of the new state.
fun : callable or None, default None
    Update function ``fun(agent, model) -> None`` called on every agent in
    this state at each step, or ``None`` for a state agents only leave
    through events (for example, recovery). See also :class:`UpdateFun`
    for epiworld's built-in update functions, which are much faster than
    Python callbacks.

Returns
-------
int
    Index of the new state.
)doc";

inline constexpr const char *get_states =
	R"doc(Get the labels of the model's states.

Returns
-------
list of str
    State labels, in index order.
)doc";

inline constexpr const char *get_n_states = R"doc(Get the number of states.

Returns
-------
int
    Number of states.
)doc";

inline constexpr const char *state_of =
	R"doc(Get the index of a state from its label.

Parameters
----------
name : str
    Label of the state.

Returns
-------
int
    Index of the state.

Raises
------
RuntimeError
    If the model has no state with that label.
)doc";

inline constexpr const char *get_name =
	R"doc(Get the name of the model type (for example, ``"Susceptible-Infected-Recovered (SIR)"``).

Returns
-------
str
    Name of the model.
)doc";

inline constexpr const char *get_n_viruses =
	R"doc(Get the number of viruses in the model.

Returns
-------
int
    Number of viruses.
)doc";

inline constexpr const char *get_n_tools =
	R"doc(Get the number of tools in the model.

Returns
-------
int
    Number of tools.
)doc";

inline constexpr const char *get_ndays =
	R"doc(Get the number of days the model was last run for.

Returns
-------
int
    Number of days (steps).
)doc";

inline constexpr const char *get_n_replicates =
	R"doc(Get the number of replicates run so far.

Returns
-------
int
    Number of calls to :meth:`run`, including those made by
    :meth:`run_multiple`.
)doc";

inline constexpr const char *get_sim_id =
	R"doc(Get the ID of the current simulation.

Useful inside a :meth:`run_multiple` callback.

Returns
-------
int
    ID of the simulation.
)doc";

inline constexpr const char *today = R"doc(Get the current simulation day.

Returns
-------
int
    Current day (step), starting at 0.
)doc";

inline constexpr const char *agents_from_edgelist =
	R"doc(Create the population from an edge list.

Parameters
----------
source : list of int
    Source agent of each tie.
target : list of int
    Target agent of each tie, of the same length as ``source``.
size : int
    Number of agents. Agent IDs must be in ``[0, size)``.
directed : bool
    Whether ties are directed. A directed tie ``source -> target`` makes
    ``target`` a neighbor of ``source`` only, so ``target`` can infect
    ``source`` but not the other way around.
)doc";

inline constexpr const char *agents_smallworld =
	R"doc(Create the population as a Watts-Strogatz small-world network.

Agents are first placed on a ring and tied to their ``k`` nearest neighbors;
then a proportion ``p`` of the ties are rewired at random.

Parameters
----------
n : int
    Number of agents.
k : int
    Number of neighbors of each agent in the ring (half on each side if the
    network is undirected).
d : bool
    Whether the network is directed.
p : float
    Proportion of ties to rewire, between 0 and 1.

Returns
-------
Model
    The model itself.
)doc";

inline constexpr const char *agents_sbm =
	R"doc(Create the population from a stochastic block model (SBM).

Parameters
----------
block_sizes : list of int
    Number of agents in each of the ``K`` blocks (groups).
mixing_matrix : list of float
    Flattened ``K x K`` matrix; entry ``(g, h)`` is the expected number of
    ties an agent in block ``g`` has with agents in block ``h``, so the row
    sums are the expected degrees.
row_major : bool, default True
    Whether ``mixing_matrix`` is flattened by rows (NumPy's default
    ``order="C"``) or by columns (``order="F"``).

Returns
-------
Model
    The model itself.
)doc";

inline constexpr const char *agents_bernoulli =
	R"doc(Create the population as a Bernoulli (Erdos-Renyi) random graph.

Parameters
----------
n : int
    Number of agents.
p : float
    Probability that any two agents are tied.
d : bool, default False
    Whether the network is directed.

Returns
-------
Model
    The model itself.
)doc";

inline constexpr const char *agents_empty_graph =
	R"doc(Create a population with no ties.

Useful for models that do not use a network, such as the ``*CONN`` and
``*Mixing`` models.

Parameters
----------
n : int, default 1000
    Number of agents.
)doc";

inline constexpr const char *add_virus = R"doc(Add a virus to the model.

The model keeps a copy of ``virus`` and distributes it to agents when the
model is run (see :meth:`Virus.set_distribution`).

Parameters
----------
virus : Virus
    The virus to add.
)doc";

inline constexpr const char *add_tool = R"doc(Add a tool to the model.

The model keeps a copy of ``tool`` and distributes it to agents when the
model is run (see :meth:`Tool.set_distribution`).

Parameters
----------
tool : Tool
    The tool to add.
)doc";

inline constexpr const char *add_entity = R"doc(Add an entity to the model.

Agents are assigned to the entity when the model is run (see
:meth:`Entity.set_distribution`).

Parameters
----------
entity : Entity
    The entity to add.
)doc";

inline constexpr const char *get_entity = R"doc(Get an entity by ID.

Parameters
----------
entity_id : int
    ID of the entity.

Returns
-------
Entity
    The entity.
)doc";

inline constexpr const char *get_n_entities =
	R"doc(Get the number of entities in the model.

Returns
-------
int
    Number of entities.
)doc";

inline constexpr const char *reset = R"doc(Reset the model to its initial state.

Restores the population, clears the database, and redistributes viruses,
tools, and entities. :meth:`run` calls it before each simulation.
)doc";

inline constexpr const char *print =
	R"doc(Print a summary of the model and its last run.

Parameters
----------
lite : bool, default False
    If ``True``, print a shorter summary.
)doc";

inline constexpr const char *initial_states =
	R"doc(Set the initial distribution of agents across states.

The meaning of ``proportions`` is model specific. For the SEIR family, for
example, ``proportions[0]`` is the share of the initially infected agents
placed in ``Infected`` rather than ``Exposed``, and ``proportions[1]`` is the
share of the remaining agents placed in ``Recovered``. The distribution is
applied when the model is reset, which happens at the start of every run.
Models without a state-based initializer ignore the call.

Parameters
----------
proportions : list of float
    Proportions used by the model's initializer.
queue : list of int, default []
    Queuing flags used by the model's initializer. Leave empty for the
    default.

Returns
-------
Model
    The model itself.
)doc";

inline constexpr const char *run = R"doc(Run the model.

Parameters
----------
ndays : int
    Number of days (steps) to simulate.
seed : int, default -1
    Seed for the random number generator. A negative value keeps the
    current generator state.

Returns
-------
Model
    The model itself.
)doc";

inline constexpr const char *run_multiple = R"doc(Run the model several times.

Each experiment resets the model (if ``reset`` is ``True``), runs it for
``ndays`` days, and then calls ``fun``.

Parameters
----------
ndays : int
    Number of days (steps) of each experiment.
nexperiments : int
    Number of experiments.
seed_ : int, default -1
    Seed for the random number generator. A negative value keeps the
    current generator state.
fun : callable or None, default None
    Function ``fun(sim_id, model) -> None`` called after each experiment,
    for example to collect results. With ``None``, the results are saved
    with :meth:`make_save_run`'s defaults.
reset : bool, default True
    Whether to reset the model before each experiment.
verbose : bool, default True
    Whether to show a progress bar.
nthreads : int, default 1
    Number of threads. With more than one, experiments run in parallel on
    copies of the model, so ``fun`` must not rely on shared state.
)doc";

inline constexpr const char *make_save_run =
	R"doc(Create a :meth:`run_multiple` callback that writes each run's results to files.

Parameters
----------
fmt : str, default "%03lu-episimulation.csv"
    File name pattern, which must contain exactly one ``%`` format (the
    simulation ID). The type of data is appended to each file name.
total_hist : bool, default True
    Save the totals per state and day (see :meth:`DataBase.get_hist_total`).
virus_info : bool, default False
    Save information about the viruses.
virus_hist : bool, default False
    Save the history of each virus (see :meth:`DataBase.get_hist_virus`).
tool_info : bool, default False
    Save information about the tools.
tool_hist : bool, default False
    Save the history of each tool (see :meth:`DataBase.get_hist_tool`).
transmission : bool, default False
    Save the transmission events (see :meth:`DataBase.get_transmissions`).
transition : bool, default False
    Save the transition counts (see
    :meth:`DataBase.get_hist_transition_matrix`).
reproductive : bool, default False
    Save the reproductive numbers (see
    :meth:`DataBase.get_reproductive_number`).
generation : bool, default False
    Save the generation times (see :meth:`DataBase.get_generation_time`).
active_cases : bool, default False
    Save the active cases (see :meth:`DataBase.get_active_cases`).
outbreak_size : bool, default False
    Save the outbreak sizes (see :meth:`DataBase.get_outbreak_size`).
hospitalizations : bool, default False
    Save the hospitalizations (see :meth:`DataBase.get_hospitalizations`).

Returns
-------
callable
    A function ``fun(sim_id, model) -> None`` to pass to
    :meth:`run_multiple`.
)doc";

inline constexpr const char *verbose_on =
	R"doc(Turn on verbose output (progress bars and messages).

Returns
-------
Model
    The model itself.
)doc";

inline constexpr const char *verbose_off = R"doc(Turn off verbose output.

Returns
-------
Model
    The model itself.
)doc";

inline constexpr const char *get_verbose =
	R"doc(Check whether verbose output is on.

Returns
-------
bool
    ``True`` if verbose output is on.
)doc";

inline constexpr const char *params = R"doc(Get the model's parameters.

Returns
-------
dict of str to float
    Parameter values by name.
)doc";

inline constexpr const char *add_param =
	R"doc(Add a named parameter, or get it if it already exists.

Viruses and tools can read parameters by name (see, for example,
:meth:`Virus.set_prob_infecting`), so changing a parameter changes every
object that uses it.

Parameters
----------
initial_val : float
    Value of the new parameter.
pname : str
    Name of the parameter. It cannot contain a colon.
overwrite : bool, default False
    If the parameter exists, whether to replace its value with
    ``initial_val``.

Returns
-------
float
    The parameter's current value.
)doc";

inline constexpr const char *get_param =
	R"doc(Get the value of a named parameter.

Parameters
----------
pname : str
    Name of the parameter.

Returns
-------
float
    The parameter's value.
)doc";

inline constexpr const char *set_param =
	R"doc(Set the value of a named parameter.

Parameters
----------
pname : str
    Name of the parameter.
val : float
    New value.
)doc";

inline constexpr const char *par =
	R"doc(Get the value of a named parameter (alias of :meth:`get_param`).

Parameters
----------
pname : str
    Name of the parameter.

Returns
-------
float
    The parameter's value.
)doc";

inline constexpr const char *get_agent = R"doc(Get an agent by ID.

Parameters
----------
i : int
    ID of the agent.

Returns
-------
Agent
    The agent.
)doc";

inline constexpr const char *get_agents = R"doc(Get the population.

Returns
-------
list of Agent
    Every agent in the model, in ID order.
)doc";

inline constexpr const char *set_rewire_prop =
	R"doc(Set the proportion of ties rewired at each step.

Only used if the model has a rewiring function.

Parameters
----------
prop : float
    Proportion between 0 and 1.
)doc";

inline constexpr const char *get_rewire_prop =
	R"doc(Get the proportion of ties rewired at each step.

Returns
-------
float
    Proportion between 0 and 1.
)doc";

inline constexpr const char *rewire =
	R"doc(Rewire the network with the model's rewiring function, if it has one.
)doc";

inline constexpr const char *add_globalevent =
	R"doc(Add a global event: a function called at the end of a simulation day.

Global events can, for example, implement interventions that change the
network (see :meth:`add_edge`) or model parameters.

Parameters
----------
fun : callable
    Function ``fun(model) -> None``.
name : str, default "global event"
    Name of the event, used by :meth:`rm_globalevent`.
date : int, default -99
    Day on which to call ``fun``. A negative value calls it every day.
)doc";

inline constexpr const char *rm_globalevent = R"doc(Remove a global event.

Parameters
----------
name : str
    Name of the event (see :meth:`add_globalevent`).
)doc";

inline constexpr const char *run_globalevents =
	R"doc(Run the global events scheduled for today.
)doc";

inline constexpr const char *write_edgelist =
	R"doc(Write the network to a file as an edge list.

Parameters
----------
fn : str
    Path of the output file.
)doc";

inline constexpr const char *set_state_function_native_id =
	R"doc(Replace a state's update function with a built-in epiworld function.

Parameters
----------
state : int
    Index of the state.
fun : NativeUpdateFun
    New update function, from :meth:`UpdateFun.susceptible` or
    :meth:`UpdateFun.rate`.

Returns
-------
Model
    The model itself.
)doc";

inline constexpr const char *set_state_function_native_name =
	R"doc(Replace a state's update function with a built-in epiworld function.

Parameters
----------
name : str
    Label of the state.
fun : NativeUpdateFun
    New update function, from :meth:`UpdateFun.susceptible` or
    :meth:`UpdateFun.rate`.

Returns
-------
Model
    The model itself.
)doc";

inline constexpr const char *set_state_function_id =
	R"doc(Replace a state's update function.

Parameters
----------
state : int
    Index of the state.
fun : callable or None
    New update function ``fun(agent, model) -> None``, or ``None`` to
    remove it.

Returns
-------
Model
    The model itself.
)doc";

inline constexpr const char *set_state_function_name =
	R"doc(Replace a state's update function.

Parameters
----------
name : str
    Label of the state.
fun : callable or None
    New update function ``fun(agent, model) -> None``, or ``None`` to
    remove it.

Returns
-------
Model
    The model itself.
)doc";

inline constexpr const char *get_db =
	R"doc(Get the database with the results of the last run.

Returns
-------
DataBase
    The model's database.
)doc";

inline constexpr const char *size = R"doc(Get the number of agents.

Returns
-------
int
    Population size.
)doc";

inline constexpr const char *len =
	R"doc(Get the number of agents (same as :meth:`size`).

Returns
-------
int
    Population size.
)doc";

inline constexpr const char *seed =
	R"doc(Seed the model's random number generator.

Parameters
----------
s : int
    Seed.
)doc";

inline constexpr const char *set_name = R"doc(Set the name of the model.

Parameters
----------
name : str
    New name.
)doc";

inline constexpr const char *get_agents_states =
	R"doc(Get the current state of every agent.

Returns
-------
list of int
    State index of each agent, in ID order.
)doc";

inline constexpr const char *get_agents_in_state =
	R"doc(Get the agents currently in a state.

Only available once the model has been run.

Parameters
----------
state : int
    Index of the state.

Returns
-------
numpy.ndarray
    IDs of the agents in that state.
)doc";

inline constexpr const char *is_directed =
	R"doc(Check whether the network is directed.

Returns
-------
bool
    ``True`` if the network is directed.
)doc";

inline constexpr const char *add_edge = R"doc(Tie two agents.

Safe to call during a run, for example from a global event. Only works on
undirected networks.

Parameters
----------
i : int
    ID of the first agent.
j : int
    ID of the second agent.

Returns
-------
bool
    ``True`` if the tie was created, ``False`` if the agents were already
    tied.
)doc";

inline constexpr const char *rm_edge = R"doc(Remove the tie between two agents.

Safe to call during a run, for example from a global event. Only works on
undirected networks.

Parameters
----------
i : int
    ID of the first agent.
j : int
    ID of the second agent.

Returns
-------
bool
    ``True`` if a tie was removed, ``False`` if there was none.
)doc";

inline constexpr const char *has_edge = R"doc(Check whether two agents are tied.

Parameters
----------
i : int
    ID of the first agent.
j : int
    ID of the second agent.

Returns
-------
bool
    ``True`` if the agents are tied (``i -> j`` in a directed network).
)doc";

inline constexpr const char *set_transmission_mode =
	R"doc(Set how network transmission is computed.

In a *pull* step, each susceptible agent looks at its infected neighbors; in
a *push* step, each infected agent adds its infection odds to its susceptible
neighbors. Both give the same distribution of infections and differ only in
speed and in the random numbers drawn. Directed networks always pull.

Parameters
----------
mode : {"auto", "push", "pull"}
    ``"auto"`` (the default) pushes when the infected agents have at most
    ``kappa`` times as many ties as the susceptible ones and pulls
    otherwise. ``"pull"`` reproduces the random streams of
    epiworld 0.15 and earlier.
kappa : float, default 0.25
    Threshold used by ``"auto"``; a finite, non-negative number.

Returns
-------
Model
    The model itself.
)doc";

inline constexpr const char *get_transmission_mode =
	R"doc(Get the transmission mode set with :meth:`set_transmission_mode`.

Returns
-------
str
    ``"auto"``, ``"push"``, or ``"pull"``.
)doc";

inline constexpr const char *get_last_transmission_mode =
	R"doc(Get the transmission mode used in the most recent step.

Returns
-------
str
    ``"push"`` or ``"pull"``.
)doc";

inline constexpr const char *get_transmission_kappa =
	R"doc(Get the threshold used by the ``"auto"`` transmission mode.

Returns
-------
float
    The ``kappa`` threshold (see :meth:`set_transmission_mode`).
)doc";

inline constexpr const char *has_param =
	R"doc(Check whether the model has a named parameter.

Parameters
----------
pname : str
    Name of the parameter.

Returns
-------
bool
    ``True`` if the parameter exists.
)doc";

inline constexpr const char *has_globalevent =
	R"doc(Check whether the model has a global event.

Parameters
----------
name : str
    Name of the event.

Returns
-------
bool
    ``True`` if the event exists.
)doc";

inline constexpr const char *get_n_globalevents =
	R"doc(Get the number of global events.

Returns
-------
int
    Number of global events.
)doc";

inline constexpr const char *queuing_on =
	R"doc(Turn on the queuing system (the default).

With queuing, each step only updates agents that may change state (for
example, susceptible agents with an infected neighbor), which is much faster
in large populations. It does not change the results.
)doc";

inline constexpr const char *queuing_off =
	R"doc(Turn off the queuing system, so every agent is updated at every step.

Needed when an update function may change the state of agents that have no
infected neighbors.

Returns
-------
Model
    The model itself.
)doc";

inline constexpr const char *is_queuing_on =
	R"doc(Check whether the queuing system is on.

Returns
-------
bool
    ``True`` if queuing is on.
)doc";

inline constexpr const char *print_state_codes =
	R"doc(Print the index and label of each state.
)doc";

inline constexpr const char *get_elapsed =
	R"doc(Get the time the simulations took.

Parameters
----------
unit : str, default "auto"
    Time unit: ``"auto"`` (chosen from the magnitude), ``"nanoseconds"``,
    ``"microseconds"``, ``"milliseconds"``, ``"seconds"``, ``"minutes"``,
    or ``"hours"``.

Returns
-------
dict
    ``"last"``: duration of the last run; ``"total"``: duration of all runs;
    ``"unit"``: abbreviation of the unit used.
)doc";

inline constexpr const char *write_data =
	R"doc(Write the results of the last run to files.

Each argument is the path of a file to write; files with an empty path are
skipped.

Parameters
----------
fn_virus_info : str, default ""
    Information about each virus.
fn_virus_hist : str, default ""
    History of each virus (see :meth:`DataBase.get_hist_virus`).
fn_tool_info : str, default ""
    Information about each tool.
fn_tool_hist : str, default ""
    History of each tool (see :meth:`DataBase.get_hist_tool`).
fn_total_hist : str, default ""
    Totals per state and day (see :meth:`DataBase.get_hist_total`).
fn_transmission : str, default ""
    Transmission events (see :meth:`DataBase.get_transmissions`).
fn_transition : str, default ""
    Transition counts (see :meth:`DataBase.get_hist_transition_matrix`).
fn_reproductive_number : str, default ""
    Reproductive numbers (see :meth:`DataBase.get_reproductive_number`).
fn_generation_time : str, default ""
    Generation times (see :meth:`DataBase.get_generation_time`).
fn_active_cases : str, default ""
    Active cases (see :meth:`DataBase.get_active_cases`).
fn_outbreak_size : str, default ""
    Outbreak sizes (see :meth:`DataBase.get_outbreak_size`).
fn_hospitalizations : str, default ""
    Hospitalizations (see :meth:`DataBase.get_hospitalizations`).
)doc";

// UpdateFun and NativeUpdateFun.

inline constexpr const char *update_fun_cls =
	R"doc(Factories for state update functions.

An update function is called on each agent in a state at every step and
decides whether the agent moves to another state. Pass the functions returned
here to :meth:`Model.add_state` or :meth:`Model.set_state_function`.
)doc";

inline constexpr const char *update_fun_default =
	R"doc(Get an empty update function: agents in the state never change on their own.

Returns
-------
None
    ``None``, which :meth:`Model.add_state` treats as no update function.
)doc";

inline constexpr const char *default_update_susceptible =
	R"doc(Get epiworld's default update function for susceptible agents.

Each susceptible agent can be infected by the viruses of its neighbors, with
probabilities that account for the viruses and the agents' tools.

Returns
-------
callable
    The update function.
)doc";

inline constexpr const char *default_update_exposed =
	R"doc(Get epiworld's default update function for infected agents.

Each infected agent can recover or die, with the probabilities set by its
virus and tools.

Returns
-------
callable
    The update function.
)doc";

inline constexpr const char *update_fun_susceptible =
	R"doc(Get a native update function for susceptible agents.

Each susceptible agent can be infected by the viruses of its neighbors, except
neighbors in the states listed in ``exclude``. Like
:meth:`default_update_susceptible`, the model may push infections from the
infected agents instead when that is cheaper (see
:meth:`Model.set_transmission_mode`).

Parameters
----------
exclude : list of int, default []
    States whose agents cannot infect others (for example, latent or
    hospitalized).

Returns
-------
NativeUpdateFun
    The update function.
)doc";

inline constexpr const char *update_fun_rate =
	R"doc(Get a native update function that moves agents at fixed daily rates.

Each day, an agent moves to ``target_states[i]`` with the probability given by
the model parameter ``param_names[i]``. With several targets, at most one
transition happens per day.

Parameters
----------
param_names : list of str
    Names of the model parameters holding the daily probabilities.
target_states : list of int
    Index of the state each probability leads to, of the same length as
    ``param_names``.

Returns
-------
NativeUpdateFun
    The update function.
)doc";

inline constexpr const char *native_update_fun_cls =
	R"doc(A state update function implemented in C++.

Create one with :meth:`UpdateFun.susceptible` or :meth:`UpdateFun.rate` and
pass it to :meth:`Model.add_state` or :meth:`Model.set_state_function`.
)doc";

} // namespace epiworldpy::docstrings::model
// clang-format on

#endif
