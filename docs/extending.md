# Extending epiworldpy in C++

Everything in epiworldpy runs in C++, in the
[epiworld](https://github.com/UofUEpiBio/epiworld) library. When Python
functions are not enough (an intervention that runs every day, a rate that
depends on each agent), you can write that part in C++ and add it to a model
built in Python. It then runs at the speed of the rest of the model.

The C++ reaches a model with one function, `epiworldpy::as_model()`, in the
header `<epiworldpy/extension.hpp>`. It returns the `epiworld::Model` behind a
Python model. From there, the whole epiworld C++ API is available: global
events, tools, viruses with their natural histories, layers, parameters...
What the code adds belongs to the model, as if added from Python.

There are two ways to compile it:

- **In a session**, with {func}`epiworldpy.cpp_source` (this page). It needs
  setuptools and a C++17 compiler: `pip install epiworldpy[ext]`.
- **In a package**: build a pybind11 module as C++17, with
  {func}`epiworldpy.get_include` among the include directories (next to
  pybind11's), and `#include <epiworldpy/extension.hpp>`.

The C++ must be built against the same major version of epiworld as the
installed epiworldpy: `as_model()` checks this, and raises an error saying so
if they differ. Compiling it again after updating epiworldpy is safest.
Compiling takes a while (the epiworld headers are large); in a session, the
same code is compiled once.

## Two extensions

The code below defines a module with two functions:

1. `add_campaign()`: a global event, run at the end of every day, that gives
   the model's vaccine to a share of the susceptible agents without it.
2. `add_waning_sirs()`: an SIRS disease whose immunity starts to wane only a
   month after recovery, a rate that reads how long each agent has been
   recovered.

The module is defined with `PYBIND11_MODULE(EPIWORLDPY_EXTENSION, m)`:
{func}`~epiworldpy.cpp_source` sets the name `EPIWORLDPY_EXTENSION`.

```python
import epiworldpy as epiworld
from epiworldpy import assign, epimodels, graph, layer

ext = epiworld.cpp_source(r"""
#include <epiworldpy/extension.hpp>

// A daily vaccination campaign
class Campaign : public epiworld::GlobalEvent {
  double p_;
public:
  explicit Campaign(double p) : p_(p) { set_name("Campaign"); }

  void operator()(epiworld::Model * m, int) override {
    auto & vaccine = m->get_tool("Vaccine");
    for (auto & agent : m->get_agents())
      if ((m->get_stage(agent) == 0) && (agent.get_n_tools() == 0u) &&
          (m->runif() < p_))
        agent.add_tool(*m, vaccine);
  }

  std::unique_ptr< epiworld::GlobalEvent > clone_ptr() const override {
    return std::make_unique< Campaign >(*this);
  }
};

// SIRS, with immunity that wanes only after 30 days
void add_waning_sirs(pybind11::object model, double prevalence) {
  epiworld::Virus flu("Flu");
  flu.add_stage("Susceptible");
  flu.add_stage("Infected", epiworld::stage::infectious);
  flu.add_stage("Recovered");
  flu.infection(0, 1, "Transmission rate");
  flu.rate(1, 2, "Recovery rate").effect(epiworld::Effect::recovery);
  flu.rate(2, 0, epiworld::ValueFun(
    [](epiworld::Agent & agent, epiworld::Model & m) -> epiworld_double {
      const int recovered = m.get_stage_entered(agent.get_id(), 2);
      return (m.today() - recovered > 30) ? 0.05 : 0.0;
    }
  ));
  epiworldpy::as_model(model).add_virus(flu, epiworld::assign::share(prevalence));
}

PYBIND11_MODULE(EPIWORLDPY_EXTENSION, m) {
  m.def("add_campaign", [](pybind11::object model, double p) {
    Campaign campaign(p);
    epiworldpy::as_model(model).add_globalevent(campaign);
  }, pybind11::arg("model"), pybind11::arg("p"));
  m.def("add_waning_sirs", &add_waning_sirs,
        pybind11::arg("model"), pybind11::arg("prevalence"));
}
""")
```

### A global event

The campaign needs a tool named `"Vaccine"` in the model, added from Python
and given to no one at the start:

```python
model = epimodels.ModelSIR(
    "flu", n=5000, prevalence=0.01, transmission_prob=0.3, recovery_rate=0.1
)
model.add_layer(layer.network("contacts", graph.smallworld(k=10, p=0.1)))
model.add_tool(epiworld.Tool("Vaccine", susceptibility_reduction=0.9),
               assign.count(0))
ext.add_campaign(model, 0.05)
model.verbose_off()
model.run(60, 1)
model.get_db().get_hist_total()
```

### A disease with a rate written in C++

The model starts empty: it gets its parameters and its layer in Python, and
its disease from C++. The transitions read the parameters by name.

```python
model = epiworld.Model(5000)
model.add_param(0.15, "Transmission rate")
model.add_param(1 / 7, "Recovery rate")
model.add_layer(layer.network("contacts", graph.smallworld(k=10, p=0.1)))
ext.add_waning_sirs(model, 0.01)
model.verbose_off()
model.run(300, 1)
model.get_db().get_hist_total()
```

The infections come in waves: an agent becomes susceptible again only after
its first month as recovered.

## In a package

With scikit-build-core and CMake, for example:

```cmake
find_package(pybind11 CONFIG REQUIRED)
execute_process(
  COMMAND ${Python_EXECUTABLE} -c "import epiworldpy; print(epiworldpy.get_include())"
  OUTPUT_VARIABLE EPIWORLDPY_INCLUDE OUTPUT_STRIP_TRAILING_WHITESPACE)
pybind11_add_module(my_extension my_extension.cpp)
target_include_directories(my_extension PRIVATE ${EPIWORLDPY_INCLUDE})
target_compile_features(my_extension PRIVATE cxx_std_17)
```

The C++ is the same as above, with the module's own name in
`PYBIND11_MODULE(my_extension, m)`; epiworldpy goes in the build requirements.

## Going further

The functions an extension can call are epiworld's C++ API: see the
[epiworld documentation](https://uofuepibio.github.io/epiworld/) and its
headers, in {func}`epiworldpy.get_include`.
