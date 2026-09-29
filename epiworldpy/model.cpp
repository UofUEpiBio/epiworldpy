#include "model.hpp"
#include "docstrings/epimodels.hpp"
#include "docstrings/model.hpp"
#include "agent-meat-state.hpp"

#include <pybind11/functional.h>
#include <pybind11/stl.h>

using namespace epiworldpy;
using namespace epiworld;
namespace py = pybind11;
namespace doc = epiworldpy::docstrings::model;
namespace mdoc = epiworldpy::docstrings::epimodels;

template <typename T, typename... Args>
void export_model_with_init(py::class_<T, Model<int>> &c, const char *doc,
							Args &&...args) {
	c.def(py::init<Args...>(), doc, std::forward<Args>(args)...);
}

template <typename T>
auto model_of(py::module &m, const char *name, const char *doc)
	-> py::class_<T, Model<int>> {
	return py::class_<T, Model<int>>(m, (std::string("Model") + name).c_str(),
									 doc);
}

template <typename ModelT, typename... Args>
void bind_model(py::module &m, const char *pyname, const char *doc,
				const char *ctor_doc, Args &&...args) {
	auto cls = model_of<ModelT>(m, pyname, doc);
	cls.def(py::init<std::decay_t<Args>...>(), ctor_doc,
			std::forward<Args>(args)...);
}

static auto transmission_mode_to_str(TransmissionMode mode) -> std::string {
	switch (mode) {
	case TransmissionMode::automatic:
		return "auto";
	case TransmissionMode::push:
		return "push";
	case TransmissionMode::pull:
		return "pull";
	}
	throw std::logic_error("Unknown transmission mode.");
}

static auto get_agents_in_state(const Model<int> &self,
								epiworld_fast_uint state)
	-> py::array_t<size_t> {
	auto ids = self.get_agents_in_state(state);
	return py::array_t<size_t>(ids.size(), ids.begin());
}

static auto get_elapsed(const Model<int> &self, std::string unit)
	-> py::dict {
	epiworld_double last, total;
	std::string abbr;
	self.get_elapsed(std::move(unit), &last, &total, &abbr, false);
	return py::dict(py::arg("last") = last, py::arg("total") = total,
					py::arg("unit") = abbr);
}

static void run_multiple(Model<int> &m, int ndays, int nexperiments, int seed,
						 const py::object &fun, bool reset, bool verbose,
						 int nthreads) {
	std::function<void(size_t, Model<int> *)> cb;
	if (fun.is_none()) {
		cb = make_save_run<int>();
	} else {
		cb = fun.cast<std::function<void(size_t, Model<int> *)>>();
	}

	m.run_multiple(ndays, nexperiments, seed, cb, reset, verbose, nthreads);
}

void epiworldpy::export_native_update_fun(
	pybind11::class_<NativeUpdateFun> &c) {
	c.def("__repr__",
		  [](const NativeUpdateFun &) { return "<epiworldpy.NativeUpdateFun>"; });
}

void epiworldpy::export_update_fun(
	pybind11::class_<epiworld::UpdateFun<int>> &c) {
	// Return epiworld's own functions (not copies): the model recognizes
	// default_update_susceptible by address to enable push transmission.
	c.def_static(
		 "default",
		 []() {
			 return (std::function<void(Agent<int> *, Model<int> *)>)nullptr;
		 },
		 doc::update_fun_default)
		.def_static("default_update_susceptible",
					[] {
						return std::function<void(Agent<int> *, Model<int> *)>(
							default_update_susceptible<int>);
					},
					doc::default_update_susceptible)
		.def_static("default_update_exposed",
					[] {
						return std::function<void(Agent<int> *, Model<int> *)>(
							default_update_exposed<int>);
					},
					doc::default_update_exposed)
		.def_static(
			"susceptible",
			[](std::vector<epiworld_fast_uint> exclude) {
				return NativeUpdateFun{
					sampler::make_update_susceptible<int>(std::move(exclude))};
			},
			py::arg("exclude") = std::vector<epiworld_fast_uint>{},
			doc::update_fun_susceptible)
		.def_static(
			"rate",
			[](std::vector<std::string> param_names,
			   std::vector<epiworld_fast_uint> target_states) {
				return NativeUpdateFun{new_state_update_transition<int>(
					std::move(param_names), std::move(target_states))};
			},
			py::arg("param_names"), py::arg("target_states"),
			doc::update_fun_rate);
}

void epiworldpy::export_model(py::class_<epiworld::Model<int>> &c) {
	c.def(py::init<>(), doc::init)
		.def(
			"add_state",
			[](Model<int> &self, std::string lab, const NativeUpdateFun &fun) {
				return self.add_state(std::move(lab), fun.fun);
			},
			py::arg("lab"), py::arg("fun"),
			doc::add_state_native)
		.def("add_state",
			 static_cast<epiworld_fast_int (epiworld::Model<>::*)(
				 std::string, UpdateFun<int>)>(&Model<int>::add_state),
			 py::arg("lab"), py::arg("fun") = nullptr,
			 doc::add_state)
		.def("get_states", &Model<int>::get_states,
			 doc::get_states)
		.def("get_n_states", &Model<int>::get_n_states,
			 doc::get_n_states)
		.def("state_of",
			 static_cast<epiworld_fast_int (Model<int>::*)(std::string_view)>(
				 &Model<int>::state_of),
			 doc::state_of, py::arg("name"))
		.def("get_name", &Model<int>::get_name,
			 doc::get_name)
		.def("get_n_viruses", &Model<int>::get_n_viruses,
			 doc::get_n_viruses)
		.def("get_n_tools", &Model<int>::get_n_tools,
			 doc::get_n_tools)
		.def("get_ndays", &Model<int>::get_ndays,
			 doc::get_ndays)
		.def("get_n_replicates", &Model<int>::get_n_replicates,
			 doc::get_n_replicates)
		.def("get_sim_id", &Model<int>::get_sim_id,
			 doc::get_sim_id)
		.def("today", &Model<int>::today, doc::today)
		.def("agents_from_edgelist", &Model<int>::agents_from_edgelist,
			 doc::agents_from_edgelist,
			 py::arg("source"), py::arg("target"), py::arg("size"),
			 py::arg("directed"))
		.def("agents_smallworld", &Model<int>::agents_smallworld,
			 doc::agents_smallworld,
			 py::arg("n"), py::arg("k"), py::arg("d"), py::arg("p"))
		.def(
			"agents_sbm",
			[](Model<int> &self, const std::vector<size_t> &block_sizes,
			   const std::vector<double> &mixing_matrix,
			   bool row_major) -> Model<int> & {
				return self.agents_sbm(block_sizes, mixing_matrix, row_major);
			},
			py::return_value_policy::reference_internal,
			doc::agents_sbm,
			py::arg("block_sizes"), py::arg("mixing_matrix"),
			py::arg("row_major") = true)
		.def("agents_bernoulli", &Model<int>::agents_bernoulli,
			 doc::agents_bernoulli, py::arg("n"),
			 py::arg("p"), py::arg("d") = false)
		.def("agents_empty_graph", &Model<int>::agents_empty_graph,
			 doc::agents_empty_graph,
			 py::arg("n") = 1000)
		.def("add_virus", &Model<int>::add_virus, doc::add_virus,
			 py::arg("virus"))
		.def("add_tool", &Model<int>::add_tool,
			 doc::add_tool, py::arg("tool"))
		.def("add_entity", &Model<int>::add_entity,
			 doc::add_entity, py::arg("entity"))
		.def(
			"get_entity",
			[](Model<int> &self, size_t entity_id) -> Entity<int> & {
				return self.get_entity(entity_id);
			},
			py::return_value_policy::reference_internal,
			doc::get_entity, py::arg("entity_id"))
		.def("get_n_entities", &Model<int>::get_n_entities,
			 doc::get_n_entities)
		.def("reset", &Model<int>::reset,
			 doc::reset)
		.def(
			"print", [](const Model<int> &m, bool lite) { m.print(lite); },
			doc::print, py::arg("lite") = false)
		.def("initial_states", &Model<int>::initial_states,
			 doc::initial_states, py::arg("proportions"),
			 py::arg("queue") = std::vector<int>{})
		.def("run", &Model<int>::run,
			 doc::run,
			 py::arg("ndays"), py::arg("seed") = -1)
		.def("run_multiple", &run_multiple, doc::run_multiple,
			 py::arg("ndays"), py::arg("nexperiments"), py::arg("seed_") = -1,
			 py::arg("fun") = py::none(), py::arg("reset") = true,
			 py::arg("verbose") = true, py::arg("nthreads") = 1)
		.def_static("make_save_run", &make_save_run<int>, doc::make_save_run,
					py::arg("fmt") = "%03lu-episimulation.csv",
					py::arg("total_hist") = true, py::arg("virus_info") = false,
					py::arg("virus_hist") = false, py::arg("tool_info") = false,
					py::arg("tool_hist") = false, py::arg("transmission") = false,
					py::arg("transition") = false,
					py::arg("reproductive") = false,
					py::arg("generation") = false,
					py::arg("active_cases") = false,
					py::arg("outbreak_size") = false,
					py::arg("hospitalizations") = false)
		.def("verbose_on", &Model<int>::verbose_on, doc::verbose_on)
		.def("verbose_off", &Model<int>::verbose_off, doc::verbose_off)
		.def("get_verbose", &Model<int>::get_verbose,
			 doc::get_verbose)
		.def(
			"params",
			[](Model<int> &self) -> std::map<std::string, epiworld_double> {
				return self.params();
			},
			doc::params)
		.def("add_param", &Model<int>::add_param,
			 doc::add_param, py::arg("initial_val"),
			 py::arg("pname"), py::arg("overwrite") = false)
		.def("get_param", &Model<int>::get_param,
			 doc::get_param, py::arg("pname"))
		.def("set_param", &Model<int>::set_param,
			 doc::set_param, py::arg("pname"), py::arg("val"))
		.def("par", &Model<int>::par,
			 doc::par, py::arg("pname"))
		.def(
			"get_agent",
			[](Model<int> &self, size_t i) -> Agent<int> & {
				return self.get_agent(i);
			},
			py::return_value_policy::reference_internal,
			doc::get_agent, py::arg("i"))
		.def(
			"get_agents",
			[](Model<int> &self) -> std::vector<Agent<int>> & {
				return self.get_agents();
			},
			py::return_value_policy::reference_internal,
			doc::get_agents)
		.def("set_rewire_prop", &Model<int>::set_rewire_prop,
			 doc::set_rewire_prop, py::arg("prop"))
		.def("get_rewire_prop", &Model<int>::get_rewire_prop,
			 doc::get_rewire_prop)
		.def("rewire", &Model<int>::rewire, doc::rewire)
		.def(
			"add_globalevent",
			[](Model<int> &self, std::function<void(Model<int> *)> fun,
			   std::string name,
			   int date) { self.add_globalevent(fun, name, date); },
			doc::add_globalevent, py::arg("fun"),
			py::arg("name") = "global event", py::arg("date") = -99)
		.def("rm_globalevent",
			 py::overload_cast<std::string>(&Model<int>::rm_globalevent),
			 doc::rm_globalevent, py::arg("name"))
		.def("run_globalevents", &Model<int>::run_globalevents,
			 doc::run_globalevents)
		.def(
			"write_edgelist",
			[](const Model<int> &self, std::string fn) {
				self.write_edgelist(fn);
			},
			doc::write_edgelist, py::arg("fn"))
		.def(
			"set_state_function",
			[](Model<int> &self, epiworld_fast_uint state,
			   const NativeUpdateFun &fun) -> Model<int> & {
				return self.set_state_function(state, fun.fun);
			},
			py::return_value_policy::reference_internal,
			doc::set_state_function_native_id,
			py::arg("state"), py::arg("fun"))
		.def(
			"set_state_function",
			[](Model<int> &self, std::string_view name,
			   const NativeUpdateFun &fun) -> Model<int> & {
				return self.set_state_function(name, fun.fun);
			},
			py::return_value_policy::reference_internal,
			doc::set_state_function_native_name, py::arg("name"),
			py::arg("fun"))
		.def("set_state_function",
			 py::overload_cast<epiworld_fast_uint, UpdateFun<int>>(
				 &Model<int>::set_state_function),
			 doc::set_state_function_id,
			 py::arg("state"), py::arg("fun"))
		.def(
			"set_state_function",
			[](Model<int> &self, std::string_view name, UpdateFun<int> fun)
				-> Model<int> & { return self.set_state_function(name, fun); },
			py::return_value_policy::reference_internal,
			doc::set_state_function_name, py::arg("name"),
			py::arg("fun"))
		.def("get_db", py::overload_cast<>(&Model<int>::get_db),
			 py::return_value_policy::reference_internal,
			 doc::get_db)
		.def("size", &Model<int>::size, doc::size)
		.def("__len__", &Model<int>::size, doc::len)
		.def("seed", &Model<int>::seed,
			 doc::seed,
			 py::arg("s"))
		.def("set_name", &Model<int>::set_name, doc::set_name,
			 py::arg("name"))
		.def("get_agents_states", &Model<int>::get_agents_states,
			 doc::get_agents_states)
		.def("get_agents_in_state", &get_agents_in_state,
			 doc::get_agents_in_state,
			 py::arg("state"))
		.def("is_directed", &Model<int>::is_directed,
			 doc::is_directed)
		.def("add_edge", &Model<int>::add_edge,
			 doc::add_edge,
			 py::arg("i"), py::arg("j"))
		.def("rm_edge", &Model<int>::rm_edge,
			 doc::rm_edge,
			 py::arg("i"), py::arg("j"))
		.def("has_edge", &Model<int>::has_edge,
			 doc::has_edge,
			 py::arg("i"), py::arg("j"))
		.def(
			"set_transmission_mode",
			[](Model<int> &self, std::string_view mode,
			   double kappa) -> Model<int> & {
				return self.set_transmission_mode(mode, kappa);
			},
			py::return_value_policy::reference_internal,
			doc::set_transmission_mode,
			py::arg("mode"), py::arg("kappa") = EPI_DEFAULT_TRANSMISSION_KAPPA)
		.def(
			"get_transmission_mode",
			[](const Model<int> &self) {
				return transmission_mode_to_str(self.get_transmission_mode());
			},
			doc::get_transmission_mode)
		.def(
			"get_last_transmission_mode",
			[](const Model<int> &self) {
				return transmission_mode_to_str(
					self.get_last_transmission_mode());
			},
			doc::get_last_transmission_mode)
		.def("get_transmission_kappa", &Model<int>::get_transmission_kappa,
			 doc::get_transmission_kappa)
		.def("has_param", &Model<int>::has_param,
			 doc::has_param,
			 py::arg("pname"))
		.def("has_globalevent", &Model<int>::has_globalevent,
			 doc::has_globalevent, py::arg("name"))
		.def("get_n_globalevents", &Model<int>::get_n_globalevents,
			 doc::get_n_globalevents)
		.def("queuing_on", &Model<int>::queuing_on,
			 doc::queuing_on)
		.def(
			"queuing_off",
			[](Model<int> &self) -> Model<int> & { return self.queuing_off(); },
			py::return_value_policy::reference_internal,
			doc::queuing_off)
		.def("is_queuing_on", &Model<int>::is_queuing_on,
			 doc::is_queuing_on)
		.def("print_state_codes", &Model<int>::print_state_codes,
			 doc::print_state_codes)
		.def("get_elapsed", &get_elapsed,
			 doc::get_elapsed,
			 py::arg("unit") = "auto")
		.def(
			"write_data",
			[](const Model<int> &self, std::string fn_virus_info,
			   std::string fn_virus_hist, std::string fn_tool_info,
			   std::string fn_tool_hist, std::string fn_total_hist,
			   std::string fn_transmission, std::string fn_transition,
			   std::string fn_reproductive_number,
			   std::string fn_generation_time, std::string fn_active_cases,
			   std::string fn_outbreak_size, std::string fn_hospitalizations) {
				self.write_data(fn_virus_info, fn_virus_hist, fn_tool_info,
								fn_tool_hist, fn_total_hist, fn_transmission,
								fn_transition, fn_reproductive_number,
								fn_generation_time, fn_active_cases,
								fn_outbreak_size, fn_hospitalizations);
			},
			doc::write_data,
			py::arg("fn_virus_info") = std::string(""),
			py::arg("fn_virus_hist") = std::string(""),
			py::arg("fn_tool_info") = std::string(""),
			py::arg("fn_tool_hist") = std::string(""),
			py::arg("fn_total_hist") = std::string(""),
			py::arg("fn_transmission") = std::string(""),
			py::arg("fn_transition") = std::string(""),
			py::arg("fn_reproductive_number") = std::string(""),
			py::arg("fn_generation_time") = std::string(""),
			py::arg("fn_active_cases") = std::string(""),
			py::arg("fn_outbreak_size") = std::string(""),
			py::arg("fn_hospitalizations") = std::string(""));
}

template <typename T> struct ModelNamedArg {
	using type = T;
	const char *name;
};

template <typename T>
constexpr auto make_arg(const char *name) -> ModelNamedArg<T> {
	return {name};
}

template <typename ModelT, typename... Args>
void export_model_(pybind11::class_<ModelT, epiworld::Model<int>> &c,
				   const char *doc, Args... args) {
	c.def(py::init<typename Args::type...>(), py::arg(args.name)...,
		  py::doc(doc));
}

void epiworldpy::export_all_models(pybind11::module &m) {

	auto diffnet = model_of<epimodels::ModelDiffNet<int>>(m, "DiffNet",
														   mdoc::diffnet);
	auto seir = model_of<epimodels::ModelSEIR<int>>(m, "SEIR", mdoc::seir);
	auto seirconn =
		model_of<epimodels::ModelSEIRCONN<int>>(m, "SEIRCONN", mdoc::seirconn);
	auto seird = model_of<epimodels::ModelSEIRD<int>>(m, "SEIRD", mdoc::seird);
	auto seirdconn = model_of<epimodels::ModelSEIRDCONN<int>>(
		m, "SEIRDCONN", mdoc::seirdconn);
	auto seirmixing = model_of<epimodels::ModelSEIRMixing<int>>(
		m, "SEIRMixing", mdoc::seirmixing);
	auto seirmixingquarantine =
		model_of<epimodels::ModelSEIRMixingQuarantine<int>>(
			m, "SEIRMixingQuarantine", mdoc::seirmixingquarantine);
	auto seirnetworkquarantine =
		model_of<epimodels::ModelSEIRNetworkQuarantine<int>>(
			m, "SEIRNetworkQuarantine", mdoc::seirnetworkquarantine);
	auto sir = model_of<epimodels::ModelSIR<int>>(m, "SIR", mdoc::sir);
	auto sirconn =
		model_of<epimodels::ModelSIRCONN<int>>(m, "SIRCONN", mdoc::sirconn);
	auto sird = model_of<epimodels::ModelSIRD<int>>(m, "SIRD", mdoc::sird);
	auto sirdconn =
		model_of<epimodels::ModelSIRDCONN<int>>(m, "SIRDCONN", mdoc::sirdconn);
	auto sirmixing = model_of<epimodels::ModelSIRMixing<int>>(
		m, "SIRMixing", mdoc::sirmixing);
	auto sis = model_of<epimodels::ModelSIS<int>>(m, "SIS", mdoc::sis);
	auto sisd = model_of<epimodels::ModelSISD<int>>(m, "SISD", mdoc::sisd);
	auto surv = model_of<epimodels::ModelSURV<int>>(m, "SURV", mdoc::surv);

	export_model_<epimodels::ModelDiffNet<int>>(
		diffnet, mdoc::diffnet_init, make_arg<std::string>("name"),
		make_arg<double>("prevalence"), make_arg<double>("prob_adopt"),
		make_arg<bool>("normalize_exposure"), make_arg<double *>("data"),
		make_arg<int>("data_ncols"), make_arg<std::vector<size_t>>("data_cols"),
		make_arg<std::vector<double>>("params"));

	export_model_<epimodels::ModelSEIR<int>>(
		seir, mdoc::seir_init, make_arg<std::string>("name"),
		make_arg<double>("prevalence"), make_arg<double>("transmission_rate"),
		make_arg<double>("incubation_days"), make_arg<double>("recovery_rate"));

	export_model_<epimodels::ModelSEIRCONN<int>>(
		seirconn, mdoc::seirconn_init, make_arg<std::string>("name"),
		make_arg<int>("n"), make_arg<double>("prevalence"),
		make_arg<double>("contact_rate"), make_arg<double>("transmission_rate"),
		make_arg<double>("incubation_days"), make_arg<double>("recovery_rate"));

	export_model_<epimodels::ModelSEIRD<int>>(
		seird, mdoc::seird_init, make_arg<std::string>("name"),
		make_arg<double>("prevalence"), make_arg<double>("transmission_rate"),
		make_arg<double>("incubation_days"), make_arg<double>("recovery_rate"),
		make_arg<double>("death_rate"));

	export_model_<epimodels::ModelSEIRDCONN<int>>(
		seirdconn, mdoc::seirdconn_init, make_arg<std::string>("name"),
		make_arg<int>("n"), make_arg<double>("prevalence"),
		make_arg<double>("contact_rate"), make_arg<double>("transmission_rate"),
		make_arg<double>("incubation_days"), make_arg<double>("recovery_rate"),
		make_arg<double>("death_rate"));

	export_model_<epimodels::ModelSEIRMixing<int>>(
		seirmixing, mdoc::seirmixing_init, make_arg<std::string>("vname"),
		make_arg<unsigned int>("n"), make_arg<double>("prevalence"),
		make_arg<double>("transmission_rate"),
		make_arg<double>("avg_incubation_days"),
		make_arg<double>("recovery_rate"),
		make_arg<std::vector<double>>("contact_matrix"));

	seirmixingquarantine.def(
		py::init<const std::string &, unsigned int, double, double, double,
				 double, std::vector<double>, double, double, double, int,
				 double, double, int, double, unsigned int>(),
		mdoc::seirmixingquarantine_init, py::arg("vname"), py::arg("n"),
		py::arg("prevalence"), py::arg("transmission_rate"),
		py::arg("avg_incubation_days"), py::arg("recovery_rate"),
		py::arg("contact_matrix"), py::arg("hospitalization_rate"),
		py::arg("hospitalization_period"), py::arg("days_undetected"),
		py::arg("quarantine_period"), py::arg("quarantine_willingness"),
		py::arg("isolation_willingness"), py::arg("isolation_period"),
		py::arg("contact_tracing_success_rate") = 1.0,
		py::arg("contact_tracing_days_prior") = 4u);

	seirnetworkquarantine.def(
		py::init<const std::string &, double, double, double, double, double,
				 double, double, int, double, double, int, double,
				 unsigned int>(),
		mdoc::seirnetworkquarantine_init, py::arg("vname"),
		py::arg("prevalence"), py::arg("transmission_rate"),
		py::arg("avg_incubation_days"), py::arg("recovery_rate"),
		py::arg("hospitalization_rate"), py::arg("hospitalization_period"),
		py::arg("days_undetected"), py::arg("quarantine_period"),
		py::arg("quarantine_willingness"), py::arg("isolation_willingness"),
		py::arg("isolation_period"),
		py::arg("contact_tracing_success_rate") = 1.0,
		py::arg("contact_tracing_days_prior") = 4u);

	export_model_<epimodels::ModelSIR<int>>(
		sir, mdoc::sir_init, make_arg<std::string>("name"),
		make_arg<double>("prevalence"), make_arg<double>("transmission_rate"),
		make_arg<double>("recovery_rate"));

	export_model_<epimodels::ModelSIRCONN<int>>(
		sirconn, mdoc::sirconn_init, make_arg<std::string>("name"),
		make_arg<int>("n"), make_arg<double>("prevalence"),
		make_arg<double>("contact_rate"), make_arg<double>("transmission_rate"),
		make_arg<double>("recovery_rate"));

	export_model_<epimodels::ModelSIRD<int>>(
		sird, mdoc::sird_init, make_arg<std::string>("name"),
		make_arg<double>("prevalence"), make_arg<double>("transmission_rate"),
		make_arg<double>("recovery_rate"), make_arg<double>("death_rate"));

	export_model_<epimodels::ModelSIRDCONN<int>>(
		sirdconn, mdoc::sirdconn_init, make_arg<std::string>("name"),
		make_arg<int>("n"), make_arg<double>("prevalence"),
		make_arg<double>("contact_rate"), make_arg<double>("transmission_rate"),
		make_arg<double>("recovery_rate"), make_arg<double>("death_rate"));

	export_model_<epimodels::ModelSIRMixing<int>>(
		sirmixing, mdoc::sirmixing_init, make_arg<std::string>("vname"),
		make_arg<unsigned int>("n"), make_arg<double>("prevalence"),
		make_arg<double>("transmission_rate"),
		make_arg<double>("recovery_rate"),
		make_arg<std::vector<double>>("contact_matrix"));

	export_model_<epimodels::ModelSIS<int>>(
		sis, mdoc::sis_init, make_arg<std::string>("name"),
		make_arg<double>("prevalence"), make_arg<double>("transmission_rate"),
		make_arg<double>("recovery_rate"));

	export_model_<epimodels::ModelSISD<int>>(
		sisd, mdoc::sisd_init, make_arg<std::string>("name"),
		make_arg<double>("prevalence"), make_arg<double>("transmission_rate"),
		make_arg<double>("recovery_rate"), make_arg<double>("death_rate"));

	// The argument names follow ModelSURV's constructor, and `prevalence` is a
	// number of agents there, not a proportion.
	export_model_<epimodels::ModelSURV<int>>(
		surv, mdoc::surv_init, make_arg<std::string>("name"),
		make_arg<epiworld_fast_uint>("prevalence"),
		make_arg<double>("efficacy_vax"), make_arg<double>("latent_period"),
		make_arg<double>("infect_period"), make_arg<double>("prob_symptoms"),
		make_arg<double>("prop_vaccinated"),
		make_arg<double>("prop_vax_redux_transm"),
		make_arg<double>("prop_vax_redux_infect"),
		make_arg<double>("surveillance_prob"),
		make_arg<double>("transmission_rate"), make_arg<double>("prob_death"),
		make_arg<double>("prob_noreinfect"));
}
