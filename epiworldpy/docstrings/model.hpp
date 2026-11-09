#ifndef EPIWORLDPY_DOCSTRINGS_MODEL_HPP
#define EPIWORLDPY_DOCSTRINGS_MODEL_HPP

// NumPy-style docstrings for epiworldpy.Model (see tests/test_docstrings.py).

// clang-format off
namespace epiworldpy::docstrings::model {

inline constexpr const char *cls =
	R"doc(An agent-based model: a population, its viruses, tools, and contacts.

The models in :mod:`epiworldpy.epimodels` come with their parameters and
virus already set up, so a typical workflow only needs contacts (a layer)
and a call to :meth:`run`::

    from epiworldpy import epimodels, graph, layer

    covid = epimodels.ModelSIR("COVID-19", n=1000, prevalence=0.01,
                               transmission_prob=0.9, recovery_rate=0.1)
    covid.add_layer(layer.network("contacts", graph.smallworld(k=5, p=0.01)))
    covid.run(ndays=100, seed=1912)
    covid.get_db().get_hist_total()

An empty ``Model(n)`` builds a custom model: add parameters
(:meth:`add_param`), a virus that carries the stages of the disease and the
transitions between them (:class:`Virus`), placed with :mod:`epiworldpy.assign`
(:meth:`add_virus`), tools (:meth:`add_tool`), and layers (:meth:`add_layer`).
The first virus gives the model its stages.
)doc";

inline constexpr const char *init =
	R"doc(Create an empty model of ``n`` agents, with no viruses, tools, or contacts.

Parameters
----------
n : int
    Number of agents.
)doc";

inline constexpr const char *add_layer = R"doc(Add a contact layer.

Agents are exposed to the infectious agents of every layer of the model, in
one draw. The layer object stays the model's layer: editing it edits the
model.

Parameters
----------
layer : epiworldpy.layer.ContactLayer
    The layer, for example from :func:`epiworldpy.layer.network`. Its name
    must be unique within the model.
)doc";

inline constexpr const char *get_layer = R"doc(Get a contact layer by name.

Parameters
----------
name : str
    Name of the layer (the mixing models name theirs ``"mixing"``).

Returns
-------
epiworldpy.layer.ContactLayer
    The model's layer, as a :class:`~epiworldpy.layer.Network` or
    :class:`~epiworldpy.layer.Mixing`.
)doc";

inline constexpr const char *get_layer_id = R"doc(Get a contact layer by id.

Parameters
----------
id : int
    Id of the layer: layers are numbered from 0 in the order they were added,
    as in the ``layers`` column of :meth:`DataBase.get_transmissions`.

Returns
-------
epiworldpy.layer.ContactLayer
    The model's layer.
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

inline constexpr const char *add_virus = R"doc(Add a virus to the model.

The model keeps its own copy of ``virus``: only that copy gets an id (and a
lineage), so give agents and tools the model's copy (:meth:`get_virus`). The
first virus gives the model its stages; a second one is a strain of it, with
the same stages and transitions (a copy, :meth:`Virus.clone`, whose
transitions may read other parameters, :meth:`Virus.use_param`).

Parameters
----------
virus : Virus
    The virus, with its natural history.
assign : epiworldpy.assign.Assign or None, default None
    Who has the virus at the start of each run (see :mod:`epiworldpy.assign`;
    stage names are the virus's stages). The seeds start in the stage the
    infection leads to. ``None`` places it on nobody.
)doc";

inline constexpr const char *add_tool = R"doc(Add a tool to the model.

The model keeps its own copy of ``tool``: only that copy gets an id, so give
agents the model's copy (:meth:`get_tool`).

Parameters
----------
tool : Tool
    The tool.
assign : epiworldpy.assign.Assign or None, default None
    Who has the tool at the start of each run (see :mod:`epiworldpy.assign`).
    ``None`` gives it to nobody.
)doc";

inline constexpr const char *reset = R"doc(Reset the model to its initial state.

Restores the population, clears the database, and places the viruses, tools,
and initial stages again. :meth:`run` calls it before each simulation.
)doc";

inline constexpr const char *print =
	R"doc(Print a summary of the model and its last run.

Parameters
----------
lite : bool, default False
    If ``True``, print a shorter summary.
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
    Save the totals per status, stage, and day (see :meth:`DataBase.get_hist_total`).
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

inline constexpr const char *add_globalevent =
	R"doc(Add a global event: a function called at the end of a simulation day.

Global events can, for example, implement interventions that change the
network (see :meth:`epiworldpy.layer.Network.add_edge`) or model
parameters.

Parameters
----------
fun : callable
    Function ``fun(model) -> None``.
name : str, default "global event"
    Name of the event, used by :meth:`rm_globalevent`.
date : int, default -99
    Day on which to call ``fun``. A negative value calls it every day.
)doc";

inline constexpr const char *add_globalevent_bubbles =
	R"doc(Add a social-bubble policy (see :class:`Bubbles`).

The model keeps its own copy of the policy: changes to the Python object
after it is added do not affect the model. Read the model's copy, e.g., its
bubbles after a run, with :meth:`Bubbles.get_from`. With
``ties="complete"``, also add the household layer
(:meth:`Bubbles.make_layer`).

Parameters
----------
bubbles : Bubbles
    The policy.
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

inline constexpr const char *set_transmission_mode =
	R"doc(Set how network transmission is computed.

In a *pull* step, each susceptible agent looks at its infected neighbors; in
a *push* step, each infected agent adds its infection odds to its susceptible
neighbors. Both give the same distribution of infections and differ only in
speed and in the random numbers drawn. Directed networks always pull.

Parameters
----------
mode : {"auto", "push", "pull"}
    ``"auto"`` (the default) chooses each day from the cost of each step:
    it pushes when ``D_c + 4 N_c <= kappa (D_s + 4 N_s)``, where ``D`` is
    the sum of degrees and ``N`` the number of agents, over the infected
    agents that can transmit (``c``) and the susceptible ones (``s``), and
    pulls otherwise. ``"pull"`` reproduces the random streams of
    epiworld 0.15 and earlier.
kappa : float, default 0.5
    Threshold used by ``"auto"``; a finite, non-negative number. Smaller
    values pull more often.

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

With queuing, each step only updates agents that may change stage (for
example, susceptible agents with an infected contact), which is much faster
in large populations. It does not change the results.
)doc";

inline constexpr const char *queuing_off =
	R"doc(Turn off the queuing system, so every agent is updated at every step.

Every agent is then visited at every step, whether or not it may change
stage. It does not change the results.

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
    Totals per status, stage, and day (see :meth:`DataBase.get_hist_total`).
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

// Viruses, tools, and the natural history.

inline constexpr const char *assign_virus = R"doc(Change where a virus of the model is placed.

Parameters
----------
virus_pos : int
    Position of the virus in the model (starting at 0).
assign : epiworldpy.assign.Assign
    Who has the virus at the start of each run (stage names are the model's
    stages).
)doc";

inline constexpr const char *rm_virus = R"doc(Remove a virus from the model.

Parameters
----------
virus_pos : int
    Position of the virus in the model (starting at 0).
)doc";

inline constexpr const char *get_virus = R"doc(Get the model's copy of a virus.

Parameters
----------
virus_pos : int
    Position of the virus in the model (starting at 0).

Returns
-------
Virus
    The model's virus (not a copy): its id and lineage are the model's.
)doc";

inline constexpr const char *assign_tool = R"doc(Change where a tool of the model is placed.

Parameters
----------
tool_pos : int
    Position of the tool in the model (starting at 0).
assign : epiworldpy.assign.Assign
    Who has the tool at the start of each run.
)doc";

inline constexpr const char *rm_tool = R"doc(Remove a tool from the model.

Parameters
----------
tool_pos : int
    Position of the tool in the model (starting at 0).
)doc";

inline constexpr const char *get_tool = R"doc(Get the model's copy of a tool.

Parameters
----------
tool_pos : int
    Position of the tool in the model (starting at 0).

Returns
-------
Tool
    The model's tool (not a copy): its id is the model's.
)doc";

inline constexpr const char *set_initial =
	R"doc(Place agents in stages without the virus at the start of each run.

For example, ``assign.shares({"Recovered": 0.3})`` starts 30% of the agents
already recovered. The virus's seeds are placed first.

Parameters
----------
assign : epiworldpy.assign.Assign
    The placement; its labels are the model's stages.
)doc";

inline constexpr const char *get_stages = R"doc(Get the model's stages.

Returns
-------
list of str
    The labels of the stages, in order (from the model's first virus).
)doc";

inline constexpr const char *get_statuses = R"doc(Get the model's statuses.

Returns
-------
list of str
    The labels of the statuses (e.g., quarantine), or ``["Free"]`` for a
    model without statuses.
)doc";

inline constexpr const char *get_stage = R"doc(Get the stage of an agent.

Parameters
----------
agent : Agent
    The agent (from :meth:`get_agent`).

Returns
-------
str
    The label of the agent's stage.
)doc";

inline constexpr const char *get_status = R"doc(Get the status of an agent.

Parameters
----------
agent : Agent
    The agent (from :meth:`get_agent`).

Returns
-------
str
    The label of the agent's status (``"Free"`` without statuses).
)doc";

inline constexpr const char *get_agents_stages =
	R"doc(Get the stage of every agent.

Returns
-------
list of str
    The label of each agent's stage, by agent id.
)doc";

inline constexpr const char *get_agents_statuses =
	R"doc(Get the status of every agent.

Returns
-------
list of str
    The label of each agent's status, by agent id (``"Free"`` without
    statuses).
)doc";

inline constexpr const char *fire = R"doc(Apply a transition to an agent now.

An agent moves only along a declared transition; an external one
(:meth:`Virus.add_external`) moves only when fired.

Parameters
----------
transition : str
    Name of the transition (e.g., ``"Vaccination"``, or
    ``"Infected -> Recovered"`` for an unnamed one).
agent_id : int
    ID of the agent.

Returns
-------
bool
    ``True`` if the agent was in a stage the transition leaves, and moved.
)doc";

inline constexpr const char *draw_natural_history =
	R"doc(Draw the model's natural history as a Mermaid diagram.

Returns
-------
str
    The diagram's Mermaid source: the stages, and the transitions between
    them.
)doc";

} // namespace epiworldpy::docstrings::model
// clang-format on

#endif
