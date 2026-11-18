#ifndef EPIWORLDPY_DOCSTRINGS_DATABASE_HPP
#define EPIWORLDPY_DOCSTRINGS_DATABASE_HPP

// NumPy-style docstrings for epiworldpy.DataBase (see
// tests/test_docstrings.py).

// clang-format off
namespace epiworldpy::docstrings::database {

inline constexpr const char *cls = R"doc(The results of a model run.

Get it with :meth:`Model.get_db` after running the model. The ``get_*``
methods return the results as dictionaries of equal-length lists, one entry
per row. String columns (including the first column, ``disease``) are
encoded as ``{"values": labels, "indexes": indices}``; decode them with
``column["values"][column["indexes"]]`` before making a data frame.
Every table defaults to all diseases; pass ``disease="Flu"`` to select one::

    import pandas as pd

    model.run(ndays=100, seed=1912)
    data = model.get_db().get_hist_total()
    totals = pd.DataFrame({k: v["values"][v["indexes"]]
                           if isinstance(v, dict) else v
                           for k, v in data.items()})
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
layer : int, default -1
    Id of the contact layer the infection came through (``-1``: none).
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
    Totals per status, stage, and day (see :meth:`get_hist_total`).
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
	R"doc(Get the number of agents with each virus, by status, stage, and day.

Parameters
----------
disease : str or None, default None
    Disease name (see Model.get_diseases()). None selects every disease.

Returns
-------
dict
    First column ``"disease"`` (names), followed by ``"dates"``, ``"ids"`` (virus IDs), ``"statuses"`` and ``"stages"``
    (labels), and ``"counts"``, one entry per row.
)doc";

inline constexpr const char *get_hist_tool =
	R"doc(Get the number of agents with each tool, by status, stage, and day.

Parameters
----------
disease : str or None, default None
    Disease name (see Model.get_diseases()). None selects every disease.

Returns
-------
dict
    First column ``"disease"`` (names), followed by ``"dates"``, ``"ids"`` (tool IDs), ``"statuses"`` and ``"stages"``
    (labels), and ``"counts"``, one entry per row.
)doc";

inline constexpr const char *get_today_virus =
	R"doc(Get the number of agents with each virus, by status and stage, today.

Parameters
----------
disease : str or None, default None
    Disease name (see Model.get_diseases()). None selects every disease.

Returns
-------
dict
    First column ``"disease"`` (names), followed by ``"statuses"`` and ``"stages"`` (labels), ``"ids"`` (virus IDs), and
    ``"counts"``, one entry per row.
)doc";

inline constexpr const char *get_today_total =
	R"doc(Get the number of agents in each status and stage today.

Parameters
----------
disease : str or None, default None
    Disease name (see Model.get_diseases()). None selects every disease.

Returns
-------
dict
    First column ``"disease"`` (names), followed by ``"statuses"`` and ``"stages"`` (labels) and ``"counts"``, one entry per
    pair.
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
founder : bool, default False
    Whether this is a founder added to the model, rather than a mutation.
)doc";

inline constexpr const char *get_hist_total =
	R"doc(Get the number of agents in each status and stage, by day.

Without statuses, the status is ``"Free"``.

Parameters
----------
disease : str or None, default None
    Disease name (see Model.get_diseases()). None selects every disease.

Returns
-------
dict
    First column ``"disease"`` (names), followed by ``"dates"``, ``"statuses"`` and ``"stages"`` (labels), and ``"counts"``,
    one entry per row.
)doc";

inline constexpr const char *get_reproductive_number =
	R"doc(Get the number of agents each infected agent went on to infect.

Parameters
----------
disease : str or None, default None
    Disease name (see Model.get_diseases()). None selects every disease.

Returns
-------
dict
    First column ``"disease"`` (names), then ``"virus_id"``, ``"sources"``,
    ``"source_exposure_dates"``, and ``"rt"`` (secondary infections).
)doc";

inline constexpr const char *get_transmissions =
	R"doc(Get every transmission event.

Parameters
----------
disease : str or None, default None
    Disease name (see Model.get_diseases()). None selects every disease.

Returns
-------
dict
    First column ``"disease"`` (names), followed by ``"dates"``, ``"sources"`` (IDs of the infecting agents, ``-1`` for the
    initial cases), ``"targets"`` (IDs of the infected agents),
    ``"viruses"`` (virus IDs), ``"source_exposure_dates"`` (days the
    infecting agents were infected), and ``"layers"`` (ids of the contact
    layers the infections came through, ``-1`` for the initial cases; see
    :meth:`Model.get_layer`), one entry per event.
)doc";

inline constexpr const char *get_generation_time =
	R"doc(Get the generation time of each infected agent.

The generation time is the number of days between an agent's infection and
the first infection it caused.

Parameters
----------
disease : str or None, default None
    Disease name (see Model.get_diseases()). None selects every disease.

Returns
-------
dict
    First column ``"disease"`` (names), followed by ``"agents"`` (agent IDs), ``"viruses"`` (virus IDs), ``"times"`` (days
    the agents were infected), and ``"generation_times"`` (``-1`` for agents
    who infected no one), one entry per infected agent.
)doc";

inline constexpr const char *get_hist_transition_matrix =
	R"doc(Get the number of agents who moved between each pair of cells, by day.

A cell is a pair of a status and a stage.

Parameters
----------
disease : str or None, default None
    Disease name (see Model.get_diseases()). None selects every disease.
skip_zeros : bool, default False
    Whether to leave out the rows with a zero count.

Returns
-------
dict
    First column ``"disease"`` (names), followed by ``"status_from"``, ``"stage_from"``, ``"status_to"``, ``"stage_to"``
    (labels), ``"dates"``, and ``"counts"``, one entry per row.
)doc";

inline constexpr const char *get_active_cases =
	R"doc(Get the number of agents with each virus, by day.

Parameters
----------
disease : str or None, default None
    Disease name (see Model.get_diseases()). None selects every disease.

Returns
-------
dict
    First column ``"disease"`` (names), followed by ``"dates"``, ``"virus_id"``, and ``"counts"``, one entry per row.
)doc";

inline constexpr const char *get_outbreak_size =
	R"doc(Get the cumulative number of agents infected with each virus, by day.

Parameters
----------
disease : str or None, default None
    Disease name (see Model.get_diseases()). None selects every disease.

Returns
-------
dict
    First column ``"disease"`` (names), followed by ``"dates"``, ``"virus_id"``, and ``"size"``, one entry per row.
)doc";

inline constexpr const char *get_hospitalizations =
	R"doc(Get the number of hospitalized agents, by day, virus, and tool.

Only models that track hospitalizations (for example,
:func:`~epiworldpy.epimodels.ModelSEIRQuarantine`) record them.

Parameters
----------
disease : str or None, default None
    Disease name (see Model.get_diseases()). None selects every disease.

Returns
-------
dict
    First column ``"disease"`` (names), followed by ``"dates"``, ``"virus_id"``, ``"tool_id"``, ``"count"``, and
    ``"weight"``, one entry per row.
)doc";

inline constexpr const char *get_transition_probability = R"doc(Get daily transition probabilities or counts over a margin.

Parameters
----------
margin : {"pair", "stage", "status"}, default "pair"
    Matrix over status and stage pairs, stages, or statuses.
disease : str or None, default None
    Disease name; required for pair and stage margins with several diseases.
normalize : bool, default True
    Divide each origin's counts by their sum to obtain probabilities.
print : bool, default False
    Also print the matrix.

Returns
-------
numpy.ndarray
    Square matrix with origins in rows and destinations in columns.
    Before a run, an empty matrix of shape (0, 0).
)doc";

inline constexpr const char *get_hist_virus_transitions = R"doc(Get declared stage transitions by virus (strain) and day.

Infections are counted under the strain caught; transitions leaving a carrier
stage under the strain carried. Over a disease's strains these sum to its
transition counts, except external firings from stages without the virus.
Seeding and imports are not transitions. Status transitions have no strain.

Parameters
----------
disease : str or None, default None
    Disease name; None selects every disease.

Returns
-------
dict
    ``"disease"``, ``"dates"``, ``"ids"`` (virus IDs), ``"viruses"``
    (strain names), ``"transitions"`` (names), and ``"counts"``.
)doc";

inline constexpr const char *get_hist_transitions =
	R"doc(Get how many times each transition fired, by day.

Parameters
----------
disease : str or None, default None
    Disease name (see Model.get_diseases()). None selects every disease.

Returns
-------
dict
    First column ``"disease"`` (names), followed by ``"dates"``, ``"transitions"`` (names: ``"<from> -> <to>"`` unless
    named), and ``"counts"``, one entry per row.
)doc";

} // namespace epiworldpy::docstrings::database
// clang-format on

#endif
