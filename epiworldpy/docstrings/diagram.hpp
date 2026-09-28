#ifndef EPIWORLDPY_DOCSTRINGS_DIAGRAM_HPP
#define EPIWORLDPY_DOCSTRINGS_DIAGRAM_HPP

// NumPy-style docstrings for epiworldpy.ModelDiagram and
// epiworldpy.DiagramType (see tests/test_docstrings.py).

// clang-format off
namespace epiworldpy::docstrings::diagram {

inline constexpr const char *diagram_type =
	R"doc(Output format of a :class:`ModelDiagram`: ``Mermaid`` or ``DOT`` (Graphviz).
)doc";

inline constexpr const char *cls =
	R"doc(Draws a model's states and transitions as a flowchart.

Examples
--------
>>> from epiworldpy import DiagramType, ModelDiagram, epimodels
>>> model = epimodels.ModelSIRCONN("COVID-19", n=10000, prevalence=0.01,
...                                contact_rate=5, transmission_rate=0.4,
...                                recovery_rate=0.95)
>>> model.run(ndays=100, seed=1912)
>>> ModelDiagram().draw_from_data(
...     DiagramType.Mermaid, model.get_states(),
...     model.get_db().get_transition_probability())
)doc";

inline constexpr const char *init = R"doc(Create a diagram.
)doc";

#define EPIWORLDPY_DOC_OUTPUT                                                  \
    "fn_output : str, default \"\"\n"                                          \
    "    Path of the output file. With an empty string, the diagram is\n"      \
    "    printed instead.\n"                                                   \
    "self_loops : bool, default False\n"                                       \
    "    Whether to draw transitions from a state to itself.\n"

inline constexpr const char *draw_from_data =
	R"doc(Draw a diagram from a transition probability matrix.

Parameters
----------
diagram_type : DiagramType
    Output format.
states : list of str
    State labels.
tprob : list of float
    Flattened ``S x S`` matrix of transition probabilities, where ``S`` is
    the number of states, stored by columns: entry ``i + j * S`` is the
    probability of moving from state ``i`` to state ``j``. This is what
    :meth:`DataBase.get_transition_probability` returns.
)doc" EPIWORLDPY_DOC_OUTPUT;

inline constexpr const char *draw_from_file =
	R"doc(Draw a diagram from a file of transition counts.

Parameters
----------
diagram_type : DiagramType
    Output format.
fn_transition : str
    Path of a file written with the ``fn_transition`` argument of
    :meth:`Model.write_data`.
)doc" EPIWORLDPY_DOC_OUTPUT;

inline constexpr const char *draw_from_files =
	R"doc(Draw a diagram from several files of transition counts, such as those saved by :meth:`Model.run_multiple`.

Parameters
----------
diagram_type : DiagramType
    Output format.
fns_transition : list of str
    Paths of files written with the ``fn_transition`` argument of
    :meth:`Model.write_data`.
)doc" EPIWORLDPY_DOC_OUTPUT;

#undef EPIWORLDPY_DOC_OUTPUT

} // namespace epiworldpy::docstrings::diagram
// clang-format on

#endif
