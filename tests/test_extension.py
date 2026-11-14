"""C++ extensions: an extension reaches a model built in Python and adds a
global event to it. Compiling takes minutes, so the compiling tests run only
with EPIWORLDPY_TEST_EXTENSIONS=1 (and setuptools installed)."""

import os
from pathlib import Path

import numpy as np
import pytest

import epiworldpy as epiworld
import epiworldpy.epimodels as epimodels
from epiworldpy import graph, layer

compiles = pytest.mark.skipif(
    os.environ.get("EPIWORLDPY_TEST_EXTENSIONS") != "1",
    reason="C++ extensions compile only with EPIWORLDPY_TEST_EXTENSIONS=1",
)

VACCINATE_ALL = """
#include <epiworldpy/extension.hpp>

// Gives the model's vaccine to every susceptible agent on day 0
class VaccinateAll : public epiworld::GlobalEvent {
public:
  VaccinateAll() { set_name("Vaccinate all"); set_day(0); }
  void operator()(epiworld::Model * m, int) override {
    auto & vaccine = m->get_tool("Vaccine");
    for (auto & agent : m->get_agents())
      if (m->get_stage(agent) == 0)
        agent.add_tool(*m, vaccine);
  }
  std::unique_ptr< epiworld::GlobalEvent > clone_ptr() const override {
    return std::make_unique< VaccinateAll >(*this);
  }
};

PYBIND11_MODULE(EPIWORLDPY_EXTENSION, m) {
  m.def("vaccinate_all", [](pybind11::object model) {
    VaccinateAll event;
    auto & mod = epiworldpy::as_model(model);
    mod.add_globalevent(event);
    return mod.size();
  });
}
"""

# Built for epiworld 99: epiworld first, then the major version redefined,
# then the extension header (whose as_model() uses it)
FUTURE = """
#include <epiworld/epiworld.hpp>
#undef EPIWORLD_VERSION_MAJOR
#define EPIWORLD_VERSION_MAJOR 99
#include <epiworldpy/extension.hpp>

PYBIND11_MODULE(EPIWORLDPY_EXTENSION, m) {
  m.def("size", [](pybind11::object model) {
    return epiworldpy::as_model(model).size();
  });
}
"""


def new_model():
    m = epimodels.ModelSIR(
        "flu", n=1000, prevalence=0.01, transmission_prob=0.5, recovery_rate=0.1
    )
    m.add_layer(layer.network("contacts", graph.smallworld(k=10, p=0.1)))
    m.add_tool(epiworld.Tool("Vaccine", susceptibility_reduction=1.0),
               epiworld.assign.count(0))
    m.verbose_off()
    return m


def susceptible_on_day_30(m):
    m.run(30, 1)
    hist = m.get_db().get_hist_total()
    day = np.asarray(hist["dates"]) == 30
    susceptible = hist["stages"]["indexes"] == 0
    return int(hist["counts"][day & susceptible].sum())


def test_extension_helpers():
    assert (Path(epiworld.get_include()) / "epiworldpy" / "extension.hpp").exists()
    assert (Path(epiworld.get_include()) / "epiworld" / "epiworld.hpp").exists()
    with pytest.raises(ValueError, match="either"):
        epiworld.cpp_source()


@compiles
def test_extension_global_event():
    ext = epiworld.cpp_source(VACCINATE_ALL)
    model = new_model()
    assert ext.vaccinate_all(model) == 1000

    # The event gives full protection at the end of day 0 to everyone still
    # susceptible, so only day 0's infections spread; without it, the
    # epidemic runs its course
    vaccinated = susceptible_on_day_30(model)
    unvaccinated = susceptible_on_day_30(new_model())
    assert vaccinated > 900
    assert unvaccinated < vaccinated - 300

    # Compiled once per session
    assert epiworld.cpp_source(VACCINATE_ALL) is ext

    with pytest.raises(TypeError, match="not an epiworldpy model"):
        ext.vaccinate_all(1)


@compiles
def test_extension_version_check():
    ext = epiworld.cpp_source(FUTURE)
    with pytest.raises(RuntimeError, match="built for epiworld 99"):
        ext.size(new_model())
