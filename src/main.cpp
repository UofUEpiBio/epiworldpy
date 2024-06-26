#include <vector>
#include <pybind11/pybind11.h>
#include <pybind11/iostream.h>
#include <pybind11/stl.h>
#include <pybind11/numpy.h> // for py::array_t
#include "epiworld-common.hpp"
#include "misc.hpp"

using namespace epiworld;
using namespace pybind11::literals;

namespace py = pybind11;

#define STRINGIFY(x) #x
#define MACRO_STRINGIFY(x) STRINGIFY(x)


std::shared_ptr< epimodels::ModelSEIRCONN<int> > ModelSEIR(
    std::string name,
    int n,
    double prevalence,
    double contact_rate,
    double transmission_rate,
    double incubation_days,
    double recovery_rate
    ) {

    std::shared_ptr< epimodels::ModelSEIRCONN<int> > object(
        new epimodels::ModelSEIRCONN<int>(
            name,
            n,
            prevalence,
            contact_rate,
            transmission_rate,
            incubation_days,
            recovery_rate
        )
        );

    return object;
}


PYBIND11_MODULE(_core, m) {
    m.doc() = R"pbdoc(
        epiworldpy wrapper
        -----------------------

        .. currentmodule:: epiworldpy

        .. autosummary::
           :toctree: _generate

           ModelSEIR
    )pbdoc";

    // Only this is necessary to expose the class
    py::class_<Model<int>, std::shared_ptr<Model<int>>>(m, "Model")
        .def("get_name", &Model<int>::get_name);

    // Only this is necessary to expose the class
    py::class_<DataBase<int>, std::shared_ptr<DataBase<int>>>(m, "DataBase")
        .def("get_hist_total", [](DataBase<int> &self) {
            /* Lo, one of the times in modern C++ where the 'new' keyword isn't out of place. */
            std::vector<std::string> states;
            std::vector<int>* dates = new std::vector<int>();
            std::vector<int>* counts = new std::vector<int>();

            self.get_hist_total(dates, &states, counts);

            /* Return to Python. */
            py::capsule pyc_dates(dates, [](void* x) { delete reinterpret_cast<std::vector<int>*>(x); });
            py::capsule pyc_counts(counts, [](void* x) { delete reinterpret_cast<std::vector<int>*>(x); });

            /* TODO: Find a way to do a no-copy of a string vector. */
            py::array py_states = py::array(py::cast(states));
            py::array_t<int> py_dates(dates->size(), dates->data(), pyc_dates);
            py::array_t<int> py_counts(counts->size(), counts->data(), pyc_counts);

            py::dict ret(
                "dates"_a=py_dates,
                "states"_a=py_states,
                "counts"_a=py_counts);

            return ret;
        })
        .def("get_reproductive_number", [](DataBase<int> &self) {
            MapVec_type<int, int> raw_rt = self.reproductive_number();
            // viruses | dates     | pairs
            // C       | C         | V
            std::vector<std::vector<std::map<int, int>>> viruses;

            /* Reserve our spaces for our elements so we don't have to worry about it later. */
            for (int virus_id = 0; virus_id < self.get_n_viruses(); virus_id++) {
                std::vector<std::map<int, int>> dates(self.get_model()->today() + 1);
                viruses.push_back(dates);
            }

            /* Load into pre-return. */
            for (const auto& keyValue : raw_rt) {
                const std::vector<int>& key = keyValue.first;
                const int virus_id = key.at(0);
                const int source = key.at(1);
                const int exposure_date = key.at(2);
                const int effective_rn = keyValue.second;

                viruses[virus_id][exposure_date].insert({source, effective_rn});
            }

            /* TODO: There's lots room for optimization here, namely returning an array instead
             *  of a bunch of lists. */
            return viruses;
        })
        .def("get_transmissions", [](DataBase<int> &self) {
            /* Lo, one of the times in modern C++ where the 'new' keyword isn't out of place. */
            std::vector<int>* dates = new std::vector<int>();
            std::vector<int>* sources = new std::vector<int>();
            std::vector<int>* targets = new std::vector<int>();
            std::vector<int>* viruses = new std::vector<int>();
            std::vector<int>* source_exposure_dates = new std::vector<int>();

            self.get_transmissions(*dates, *sources, *targets, *viruses, *source_exposure_dates);

            /* Return to Python. */
            py::capsule pyc_dates(dates, [](void* x) { delete reinterpret_cast<std::vector<int>*>(x); });
            py::capsule pyc_sources(sources, [](void* x) { delete reinterpret_cast<std::vector<int>*>(x); });
            py::capsule pyc_targets(targets, [](void* x) { delete reinterpret_cast<std::vector<int>*>(x); });
            py::capsule pyc_viruses(viruses, [](void* x) { delete reinterpret_cast<std::vector<int>*>(x); });
            py::capsule pyc_source_exposure_dates(source_exposure_dates, [](void* x) { delete reinterpret_cast<std::vector<int>*>(x); });

            py::array py_dates(dates->size(), dates->data(), pyc_dates);
            py::array py_sources(sources->size(), sources->data(), pyc_sources);
            py::array py_targets(targets->size(), targets->data(), pyc_targets);
            py::array py_viruses(viruses->size(), viruses->data(), pyc_viruses);
            py::array py_source_exposure_dates(source_exposure_dates->size(), source_exposure_dates->data(), pyc_source_exposure_dates);

            py::dict ret(
                "dates"_a=py_dates,
                "sources"_a=py_sources,
                "targets"_a=py_targets,
                "viruses"_a=py_viruses,
                "source_exposure_dates"_a=py_source_exposure_dates);

            return ret;
        })
        .def("get_generation_time", [](DataBase<int> &self) {
            std::vector<int>* agents = new std::vector<int>();
            std::vector<int>* viruses = new std::vector<int>();
            std::vector<int>* times = new std::vector<int>();
            std::vector<int>* gentimes = new std::vector<int>();

            self.generation_time(*agents, *viruses, *times, *gentimes);

            /* Return to Python. */
            py::capsule pyc_agents(agents, [](void* x) { delete reinterpret_cast<std::vector<int>*>(x); });
            py::capsule pyc_viruses(viruses, [](void* x) { delete reinterpret_cast<std::vector<int>*>(x); });
            py::capsule pyc_times(times, [](void* x) { delete reinterpret_cast<std::vector<int>*>(x); });
            py::capsule pyc_gentimes(gentimes, [](void* x) { delete reinterpret_cast<std::vector<int>*>(x); });

            py::array py_agents(agents->size(), agents->data(), pyc_agents);
            py::array py_viruses(viruses->size(), viruses->data(), pyc_viruses);
            py::array py_times(times->size(), times->data(), pyc_times);
            py::array py_gentimes(gentimes->size(), gentimes->data(), pyc_gentimes);

            py::dict ret(
                "agents"_a=py_agents,
                "viruses"_a=py_viruses,
                "times"_a=py_times,
                "gentimes"_a=py_gentimes);

            return ret;
        });

    // Only this is necessary to expose the class
    py::class_<epimodels::ModelSEIRCONN<int>, std::shared_ptr<epimodels::ModelSEIRCONN<int>>>(m, "ModelSEIRCONN")
        // .def(py::init<>())
        .def("print", &epimodels::ModelSEIRCONN<int>::print,
            // py::call_guard<py::scoped_ostream_redirect, py::scoped_estream_redirect>(),
            R"pbdoc(
            Print the object

            Some other explanation about the print function.)
        )pbdoc",
            py::arg("summary") = true
            )
        .def("run", &epimodels::ModelSEIRCONN<int>::run, R"pbdoc(
            Run the model
        )pbdoc",
            py::arg("ndays"),
            py::arg("seed") = 1u
            )
        .def("get_db", [](epimodels::ModelSEIRCONN<int> &self) {
            //std::cout << "!!! " << self.get_db().get_model()->get_name() << std::endl;
            return std::shared_ptr<DataBase<int>>(&self.get_db(), [](DataBase<int>*){ /* do nothing, no delete */ });
        }, py::return_value_policy::reference);

    // Example with shared_ptr
    m.def("ModelSEIR", &ModelSEIR, R"pbdoc(
        Create an SEIR model
    )pbdoc",
        py::arg("name"),
        py::arg("n"),
        py::arg("prevalence"),
        py::arg("contact_rate"),
        py::arg("transmission_rate"),
        py::arg("incubation_days"),
        py::arg("recovery_rate")
        );

#ifdef VERSION_INFO
    m.attr("__version__") = MACRO_STRINGIFY(VERSION_INFO);
#else
    m.attr("__version__") = "dev";
#endif
}
