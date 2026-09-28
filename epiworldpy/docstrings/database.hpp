#ifndef EPIWORLDPY_DOCSTRINGS_DATABASE_HPP
#define EPIWORLDPY_DOCSTRINGS_DATABASE_HPP

// NumPy-style docstrings for epiworldpy.DataBase (see
// tests/test_docstrings.py).

// clang-format off
namespace epiworldpy::docstrings::database {

inline constexpr const char *cls = R"doc(The results of a model run.

Get it with :meth:`Model.get_db` after running the model. The ``get_*``
methods return the results as dictionaries of equal-length lists, one entry
per row, which convert directly to data frames::

    import pandas as pd

    model.run(ndays=100, seed=1912)
    totals = pd.DataFrame(model.get_db().get_hist_total())
)doc";

inline constexpr const char *add_user_data_row =
	R"doc(Add a row of user-defined data for the current day.

Parameters
----------
x : list of float
    One value per user-defined variable.
)doc";

inline constexpr const char *add_user_data_value =
	R"doc(Set one user-defined variable for the current day.

Starts a new row (filled with zeros) if the current day has none yet.

Parameters
----------
j : int
    Index of the variable.
x : float
    Value.
)doc";

inline constexpr const char *get_n_tools =
	R"doc(Get the number of tools recorded.

Returns
-------
int
    Number of tools.
)doc";

inline constexpr const char *get_n_viruses =
	R"doc(Get the number of viruses (including variants) recorded.

Returns
-------
int
    Number of viruses.
)doc";

inline constexpr const char *record_transmission =
	R"doc(Record a transmission event on the current day.

Called by the model; only needed by custom update functions that infect
agents themselves.

Parameters
----------
i : int
    ID of the agent who transmitted the virus, or ``-1`` if unknown.
j : int
    ID of the agent who was infected.
virus : int
    ID of the virus.
i_expo_date : int
    Day the transmitting agent was infected.
)doc";

inline constexpr const char *write_data = R"doc(Write the results to files.

Each argument is the path of a file to write; files with an empty path are
skipped. See :meth:`Model.write_data`, which has defaults for all of them.

Parameters
----------
fn_virus_info : str
    Information about each virus.
fn_virus_hist : str
    History of each virus (see :meth:`get_hist_virus`).
fn_tool_info : str
    Information about each tool.
fn_tool_hist : str
    History of each tool (see :meth:`get_hist_tool`).
fn_total_hist : str
    Totals per state and day (see :meth:`get_hist_total`).
fn_transmission : str
    Transmission events (see :meth:`get_transmissions`).
fn_transition : str
    Transition counts (see :meth:`get_hist_transition_matrix`).
fn_reproductive_number : str
    Reproductive numbers (see :meth:`get_reproductive_number`).
fn_generation_time : str
    Generation times (see :meth:`get_generation_time`).
fn_active_cases : str, default ""
    Active cases (see :meth:`get_active_cases`).
fn_outbreak_size : str, default ""
    Outbreak sizes (see :meth:`get_outbreak_size`).
fn_hospitalizations : str, default ""
    Hospitalizations (see :meth:`get_hospitalizations`).
)doc";

inline constexpr const char *get_hist_virus =
	R"doc(Get the number of agents with each virus, by state and day.

Returns
-------
dict
    ``"dates"``, ``"ids"`` (virus IDs), ``"states"`` (state labels), and
    ``"counts"``, one entry per row.
)doc";

inline constexpr const char *get_hist_tool =
	R"doc(Get the number of agents with each tool, by state and day.

Returns
-------
dict
    ``"dates"``, ``"ids"`` (tool IDs), ``"states"`` (state labels), and
    ``"counts"``, one entry per row.
)doc";

inline constexpr const char *get_today_transition_matrix =
	R"doc(Get the number of agents who moved between each pair of states today.

Returns
-------
dict
    ``"counts"``: a flattened ``S x S`` matrix, where ``S`` is the number of
    states, stored by columns: entry ``i + j * S`` counts the agents who
    moved from state ``i`` to state ``j``.
)doc";

inline constexpr const char *get_today_virus =
	R"doc(Get the number of agents with each virus, by state, today.

Returns
-------
dict
    ``"states"`` (state labels), ``"ids"`` (virus IDs), and ``"counts"``,
    one entry per row.
)doc";

inline constexpr const char *get_today_total =
	R"doc(Get the number of agents in each state today.

Returns
-------
dict
    ``"states"`` (state labels) and ``"counts"``, one entry per state.
)doc";

inline constexpr const char *size =
	R"doc(Get the number of viruses (including variants) recorded.

Returns
-------
int
    Number of viruses.
)doc";

inline constexpr const char *record =
	R"doc(Record the model's current counts. Called by the model at every step.
)doc";

inline constexpr const char *reset =
	R"doc(Clear the recorded results. Called by :meth:`Model.reset`.
)doc";

inline constexpr const char *record_tool =
	R"doc(Register a tool, or a new version of one, in the database.

Parameters
----------
t : Tool
    The tool.
)doc";

inline constexpr const char *record_virus =
	R"doc(Register a virus, or a new variant of one, in the database.

Parameters
----------
v : Virus
    The virus.
)doc";

inline constexpr const char *get_hist_total =
	R"doc(Get the number of agents in each state, by day.

Returns
-------
dict
    ``"dates"``, ``"states"`` (state labels), and ``"counts"``, one entry
    per row.
)doc";

inline constexpr const char *get_reproductive_number =
	R"doc(Get the number of agents each infected agent went on to infect.

Returns
-------
numpy.ndarray
    Integer array with one row per infected agent and four columns: the
    virus ID, the day the agent was infected, the agent's ID, and the number
    of agents it infected (its reproductive number).
)doc";

inline constexpr const char *get_transmissions =
	R"doc(Get every transmission event.

Returns
-------
dict
    ``"dates"``, ``"sources"`` (IDs of the infecting agents, ``-1`` for the
    initial cases), ``"targets"`` (IDs of the infected agents),
    ``"viruses"`` (virus IDs), and ``"source_exposure_dates"`` (days the
    infecting agents were infected), one entry per event.
)doc";

inline constexpr const char *get_generation_time =
	R"doc(Get the generation time of each infected agent.

The generation time is the number of days between an agent's infection and
the first infection it caused.

Returns
-------
dict
    ``"agents"`` (agent IDs), ``"viruses"`` (virus IDs), ``"times"`` (days
    the agents were infected), and ``"generation_times"`` (``-1`` for agents
    who infected no one), one entry per infected agent.
)doc";

inline constexpr const char *get_hist_transition_matrix =
	R"doc(Get the number of agents who moved between each pair of states, by day.

Parameters
----------
skip_zeros : bool, default False
    Whether to leave out the rows with a zero count.

Returns
-------
dict
    ``"state_from"``, ``"state_to"`` (state labels), ``"dates"``, and
    ``"counts"``, one entry per row.
)doc";

inline constexpr const char *get_active_cases =
	R"doc(Get the number of agents with each virus, by day.

Returns
-------
dict
    ``"dates"``, ``"virus_id"``, and ``"counts"``, one entry per row.
)doc";

inline constexpr const char *get_outbreak_size =
	R"doc(Get the cumulative number of agents infected with each virus, by day.

Returns
-------
dict
    ``"dates"``, ``"virus_id"``, and ``"size"``, one entry per row.
)doc";

inline constexpr const char *get_hospitalizations =
	R"doc(Get the number of hospitalized agents, by day, virus, and tool.

Only models that track hospitalizations (for example,
:class:`~epiworldpy.epimodels.ModelSEIRNetworkQuarantine`) record them.

Returns
-------
dict
    ``"dates"``, ``"virus_id"``, ``"tool_id"``, ``"count"``, and
    ``"weight"``, one entry per row.
)doc";

inline constexpr const char *get_transition_probability =
	R"doc(Estimate the daily probability of moving between each pair of states.

Parameters
----------
print : bool, default False
    Whether to also print the matrix.
normalize : bool, default True
    Whether to divide the counts by the number of agents in each origin
    state, giving probabilities; otherwise, return the counts.

Returns
-------
list of float
    A flattened ``S x S`` matrix, where ``S`` is the number of states,
    stored by columns: entry ``i + j * S`` is the probability of moving
    from state ``i`` to state ``j``.
)doc";

} // namespace epiworldpy::docstrings::database
// clang-format on

#endif
