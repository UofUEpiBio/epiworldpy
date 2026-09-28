#include <sys/stat.h>

#include <pybind11/iostream.h>
#include <pybind11/numpy.h>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h> // silently fails when removed.

#include "agent.hpp"
#include "database.hpp"
#include "diagram.hpp"
#include "docstrings/agent.hpp"
#include "docstrings/database.hpp"
#include "docstrings/diagram.hpp"
#include "docstrings/entity.hpp"
#include "docstrings/epimodels.hpp"
#include "docstrings/model.hpp"
#include "docstrings/tool.hpp"
#include "docstrings/virus.hpp"
#include "entity.hpp"
#include "misc.hpp"
#include "model.hpp"
#include "tool.hpp"
#include "virus.hpp"

namespace py = pybind11;
using namespace epiworld;
namespace docs = epiworldpy::docstrings;

#define STRINGIFY(x) #x
#define MACRO_STRINGIFY(x) STRINGIFY(x)

PYBIND11_MODULE(_core, m) {
	auto agent = py::class_<Agent<int>>(m, "Agent", docs::agent::cls);
	auto native_update_fun = py::class_<epiworldpy::NativeUpdateFun>(
		m, "NativeUpdateFun", docs::model::native_update_fun_cls);
	auto update_fun = py::class_<UpdateFun<int>>(m, "UpdateFun",
												 docs::model::update_fun_cls);
	auto model = py::class_<Model<int>>(m, "Model", docs::model::cls);
	auto database = py::class_<DataBase<int>, std::shared_ptr<DataBase<int>>>(
		m, "DataBase", docs::database::cls);
	auto diagram_type = py::enum_<DiagramType>(m, "DiagramType",
											   docs::diagram::diagram_type);
	auto diagram = py::class_<ModelDiagram, std::shared_ptr<ModelDiagram>>(
		m, "ModelDiagram", docs::diagram::cls);
	auto entity = py::class_<Entity<int>, std::shared_ptr<Entity<int>>>(
		m, "Entity", docs::entity::cls);
	auto tool = py::class_<Tool<int>>(m, "Tool", docs::tool::cls);
	auto virus = py::class_<Virus<int>>(m, "Virus", docs::virus::cls);

	epiworldpy::export_agent(agent);
	epiworldpy::export_native_update_fun(native_update_fun);
	epiworldpy::export_update_fun(update_fun);
	epiworldpy::export_model(model);
	epiworldpy::export_database(database);
	epiworldpy::export_diagram_type(diagram_type);
	epiworldpy::export_diagram(diagram);
	epiworldpy::export_entity(entity);
	epiworldpy::export_tool(tool);
	epiworldpy::export_virus(virus);

	auto m_epimodels = m.def_submodule("epimodels", docs::epimodels::module);
	epiworldpy::export_all_models(m_epimodels);

	m.attr("__epiworld_version__") = epiworld_version();

#ifdef VERSION_INFO
	/* Give the real version. */
	m.attr("__version__") = MACRO_STRINGIFY(VERSION_INFO);
#else
	/* Also give the real version, but prefix with 'dev'. */
	m.attr("__version__") = "dev-" MACRO_STRINGIFY(VERSION_INFO);
#endif
}
