#ifndef EPIWORLDPY_DOCSTRINGS_ASSIGN_HPP
#define EPIWORLDPY_DOCSTRINGS_ASSIGN_HPP

// NumPy-style docstrings for epiworldpy.assign (see tests/test_docstrings.py;
// the functions are in epiworldpy/assign/__init__.py).

// clang-format off
namespace epiworldpy::docstrings::assign {

inline constexpr const char *module =
	R"doc(Placement: who gets a virus, a tool, an initial stage, or a group.

One vocabulary places everything at the start of each run:

- a virus: :meth:`epiworldpy.Model.add_virus` (labels are its stages);
- a tool: :meth:`epiworldpy.Model.add_tool`;
- agents in stages without the virus: :meth:`epiworldpy.Model.set_initial`;
- the groups of a mixing layer: :func:`epiworldpy.layer.mixing`.

Agent ids are 0-based.
)doc";

inline constexpr const char *cls =
	R"doc(A placement, from one of the :mod:`epiworldpy.assign` functions.
)doc";

inline constexpr const char *get_form = R"doc(Get the kind of placement.

Returns
-------
str
    ``"share"``, ``"count"``, ``"shares"``, ``"counts"``,
    ``"group_shares"``, ``"group_counts"``, ``"agents"``, ``"labels"``, or
    ``"by"``.
)doc";

} // namespace epiworldpy::docstrings::assign

// clang-format on

#endif /* EPIWORLDPY_DOCSTRINGS_ASSIGN_HPP */
