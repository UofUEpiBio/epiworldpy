#include "model.hpp"
#include "agent-meat-state.hpp"

#include <pybind11/functional.h>
#include <pybind11/stl.h>

using namespace epiworldpy;
using namespace epiworld;
namespace py = pybind11;

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

void epiworldpy::export_update_fun(
	pybind11::class_<epiworld::UpdateFun<int>> &c) {
	// Return epiworld's own functions (not copies): the model recognizes
	// default_update_susceptible by address to enable push transmission.
	c.def_static(
		 "default",
		 []() {
			 return (std::function<void(Agent<int> *, Model<int> *)>)nullptr;
		 })
		.def_static("default_update_susceptible",
					[] {
						return std::function<void(Agent<int> *, Model<int> *)>(
							default_update_susceptible<int>);
					})
		.def_static("default_update_exposed", [] {
			return std::function<void(Agent<int> *, Model<int> *)>(
				default_update_exposed<int>);
		});
}

void epiworldpy::export_model(py::class_<epiworld::Model<int>> &c) {
	c.def(py::init<>(), "Create a new empty model.")
		.def("add_state",
			 static_cast<epiworld_fast_int (epiworld::Model<>::*)(
				 std::string, UpdateFun<int>)>(&Model<int>::add_state),
			 py::arg("lab"), py::arg("fun") = nullptr,
			 "Add a new state to the model. Returns the state index.")
		.def("get_states", &Model<int>::get_states,
			 "Get the list of state labels.")
		.def("get_n_states", &Model<int>::get_n_states,
			 "Get the number of states in the model.")
		.def("state_of",
			 static_cast<epiworld_fast_int (Model<int>::*)(std::string_view)>(
				 &Model<int>::state_of),
			 "Get the index of a state by name.", py::arg("name"))
		.def("get_name", &Model<int>::get_name,
			 "Get the name of the type of model.")
		.def("get_n_viruses", &Model<int>::get_n_viruses,
			 "Get the number of viruses in the model.")
		.def("get_n_tools", &Model<int>::get_n_tools,
			 "Get the number of tools in the model.")
		.def("get_ndays", &Model<int>::get_ndays,
			 "Get the number of days the model was run.")
		.def("get_n_replicates", &Model<int>::get_n_replicates,
			 "Get the number of replicates.")
		.def("get_sim_id", &Model<int>::get_sim_id,
			 "Get the current simulation ID (in run_multiple).")
		.def("today", &Model<int>::today, "Get the current simulation day.")
		.def("agents_from_edgelist", &Model<int>::agents_from_edgelist,
			 "Populate the model's agents from an edge list.",
			 py::arg("source"), py::arg("target"), py::arg("size"),
			 py::arg("directed"))
		.def("agents_smallworld", &Model<int>::agents_smallworld,
			 "Populate the model using a Watts-Strogatz small-world network.",
			 py::arg("n"), py::arg("k"), py::arg("d"), py::arg("p"))
		.def(
			"agents_sbm",
			[](Model<int> &self, const std::vector<size_t> &block_sizes,
			   const std::vector<double> &mixing_matrix,
			   bool row_major) -> Model<int> & {
				return self.agents_sbm(block_sizes, mixing_matrix, row_major);
			},
			py::return_value_policy::reference_internal,
			"Populate the model using a Stochastic Block Model (SBM).",
			py::arg("block_sizes"), py::arg("mixing_matrix"),
			py::arg("row_major") = true)
		.def("agents_bernoulli", &Model<int>::agents_bernoulli,
			 "Populate the model using a Bernoulli random graph.", py::arg("n"),
			 py::arg("p"), py::arg("d") = false)
		.def("agents_empty_graph", &Model<int>::agents_empty_graph,
			 "Populate the model with n agents and no connections.",
			 py::arg("n") = 1000)
		.def("add_virus", &Model<int>::add_virus, "Adds a virus to the model.",
			 py::arg("virus"))
		.def("add_tool", &Model<int>::add_tool,
			 "Adds a tool to modify the model.", py::arg("tool"))
		.def("add_entity", &Model<int>::add_entity,
			 "Adds an entity to the model.", py::arg("entity"))
		.def(
			"get_entity",
			[](Model<int> &self, size_t entity_id) -> Entity<int> & {
				return self.get_entity(entity_id);
			},
			py::return_value_policy::reference_internal,
			"Get an entity by index.", py::arg("entity_id"))
		.def("get_n_entities", &Model<int>::get_n_entities,
			 "Get the number of entities in the model.")
		.def("reset", &Model<int>::reset,
			 "Reset the model to its initial state.")
		.def(
			"print", [](const Model<int> &m, bool lite) { m.print(lite); },
			"Print a summary of the model run.", py::arg("lite") = false)
		.def("run", &Model<int>::run,
			 "Run the model for the specified number of days.",
			 py::arg("ndays"), py::arg("seed") = -1)
		.def("run_multiple", &run_multiple, "Run the model multiple times.",
			 py::arg("ndays"), py::arg("nexperiments"), py::arg("seed_") = -1,
			 py::arg("fun") = py::none(), py::arg("reset") = true,
			 py::arg("verbose") = true, py::arg("nthreads") = 1)
		.def("make_save_run", &make_save_run<int>,
			 "Create a callback function to save the model run.")
		.def("verbose_on", &Model<int>::verbose_on, "Enable verbose output.")
		.def("verbose_off", &Model<int>::verbose_off, "Disable verbose output.")
		.def("get_verbose", &Model<int>::get_verbose,
			 "Check if verbose output is enabled.")
		.def(
			"params",
			[](Model<int> &self) -> std::map<std::string, epiworld_double> {
				return self.params();
			},
			"Get the model parameters as a dictionary.")
		.def("add_param", &Model<int>::add_param,
			 "Add (or retrieve) a named parameter.", py::arg("initial_val"),
			 py::arg("pname"), py::arg("overwrite") = false)
		.def("get_param", &Model<int>::get_param,
			 "Get a named parameter value.", py::arg("pname"))
		.def("set_param", &Model<int>::set_param,
			 "Set a named parameter value.", py::arg("pname"), py::arg("val"))
		.def("par", &Model<int>::par,
			 "Get a named parameter value (short form).", py::arg("pname"))
		.def(
			"get_agent",
			[](Model<int> &self, size_t i) -> Agent<int> & {
				return self.get_agent(i);
			},
			py::return_value_policy::reference_internal,
			"Get an agent by index.", py::arg("i"))
		.def(
			"get_agents",
			[](Model<int> &self) -> std::vector<Agent<int>> & {
				return self.get_agents();
			},
			py::return_value_policy::reference_internal,
			"Get the full list of agents.")
		.def("set_rewire_prop", &Model<int>::set_rewire_prop,
			 "Set the proportion of ties to rewire each step.", py::arg("prop"))
		.def("get_rewire_prop", &Model<int>::get_rewire_prop,
			 "Get the rewire proportion.")
		.def("rewire", &Model<int>::rewire, "Rewire the network.")
		.def(
			"add_globalevent",
			[](Model<int> &self, std::function<void(Model<int> *)> fun,
			   std::string name,
			   int date) { self.add_globalevent(fun, name, date); },
			"Add a global event (called once per step).", py::arg("fun"),
			py::arg("name") = "global event", py::arg("date") = -99)
		.def("rm_globalevent",
			 py::overload_cast<std::string>(&Model<int>::rm_globalevent),
			 "Remove a global event by name.", py::arg("name"))
		.def("run_globalevents", &Model<int>::run_globalevents,
			 "Manually run all registered global events.")
		.def(
			"write_edgelist",
			[](const Model<int> &self, std::string fn) {
				self.write_edgelist(fn);
			},
			"Write the network edge list to a file.", py::arg("fn"))
		.def("set_state_function",
			 py::overload_cast<epiworld_fast_uint, UpdateFun<int>>(
				 &Model<int>::set_state_function),
			 "Replace the update function for a state by index.",
			 py::arg("state"), py::arg("fun"))
		.def(
			"set_state_function",
			[](Model<int> &self, std::string_view name, UpdateFun<int> fun)
				-> Model<int> & { return self.set_state_function(name, fun); },
			py::return_value_policy::reference_internal,
			"Replace the update function for a state by name.", py::arg("name"),
			py::arg("fun"))
		.def("get_db", py::overload_cast<>(&Model<int>::get_db),
			 py::return_value_policy::reference_internal,
			 "Get the data from the model run.")
		.def("size", &Model<int>::size, "Get the number of agents.")
		.def("__len__", &Model<int>::size, "Get the number of agents.")
		.def("seed", &Model<int>::seed,
			 "Set the seed of the model's random number generator.",
			 py::arg("s"))
		.def("set_name", &Model<int>::set_name, "Set the name of the model.",
			 py::arg("name"))
		.def("get_agents_states", &Model<int>::get_agents_states,
			 "Get the current state of every agent.")
		.def("get_agents_in_state", &get_agents_in_state,
			 "Get the IDs of the agents currently in a state (available once "
			 "the model has been run).",
			 py::arg("state"))
		.def("is_directed", &Model<int>::is_directed,
			 "Whether the network is directed.")
		.def("add_edge", &Model<int>::add_edge,
			 "Add an undirected tie between agents i and j. Safe to call "
			 "during a run (e.g., from a global event). Returns False if "
			 "the tie already existed.",
			 py::arg("i"), py::arg("j"))
		.def("rm_edge", &Model<int>::rm_edge,
			 "Remove the tie between agents i and j. Safe to call during a "
			 "run. Returns False if there was no tie.",
			 py::arg("i"), py::arg("j"))
		.def("has_edge", &Model<int>::has_edge,
			 "Whether agents i and j are tied (i -> j if directed).",
			 py::arg("i"), py::arg("j"))
		.def(
			"set_transmission_mode",
			[](Model<int> &self, std::string_view mode,
			   double kappa) -> Model<int> & {
				return self.set_transmission_mode(mode, kappa);
			},
			py::return_value_policy::reference_internal,
			"Set how network transmission is computed: 'auto' (default), "
			"'push', or 'pull'. Both give the same distribution of "
			"infections; 'pull' reproduces the random streams of "
			"epiworld <= 0.15. kappa is the cost threshold used by 'auto'.",
			py::arg("mode"), py::arg("kappa") = EPI_DEFAULT_TRANSMISSION_KAPPA)
		.def(
			"get_transmission_mode",
			[](const Model<int> &self) {
				return transmission_mode_to_str(self.get_transmission_mode());
			},
			"Get the transmission mode ('auto', 'push', or 'pull').")
		.def(
			"get_last_transmission_mode",
			[](const Model<int> &self) {
				return transmission_mode_to_str(
					self.get_last_transmission_mode());
			},
			"Get the transmission mode used in the most recent step ('push' "
			"or 'pull').")
		.def("get_transmission_kappa", &Model<int>::get_transmission_kappa,
			 "Get the threshold used by the 'auto' transmission mode.")
		.def("has_param", &Model<int>::has_param,
			 "Whether the model has a parameter with that name.",
			 py::arg("pname"))
		.def("has_globalevent", &Model<int>::has_globalevent,
			 "Whether a global event with that name exists.", py::arg("name"))
		.def("get_n_globalevents", &Model<int>::get_n_globalevents,
			 "Get the number of global events registered.")
		.def("queuing_on", &Model<int>::queuing_on,
			 "Activate the queuing system (default).")
		.def(
			"queuing_off",
			[](Model<int> &self) -> Model<int> & { return self.queuing_off(); },
			py::return_value_policy::reference_internal,
			"Deactivate the queuing system.")
		.def("is_queuing_on", &Model<int>::is_queuing_on,
			 "Whether the queuing system is on.")
		.def("print_state_codes", &Model<int>::print_state_codes,
			 "Print the state codes and labels.")
		.def("get_elapsed", &get_elapsed,
			 "Get the elapsed time of the last run and of all runs as a dict "
			 "with keys 'last', 'total', and 'unit'.",
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
			"Write the model's data to files (empty filenames are skipped).",
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
				   const char *name, Args... args) {
	c.def(py::init<typename Args::type...>(), py::arg(args.name)...,
		  py::doc((std::string("Create a ") + name + " Model").c_str()));
}

void epiworldpy::export_all_models(pybind11::module &m) {

	auto diffnet = model_of<epimodels::ModelDiffNet<int>>(
		m, "DiffNet", "A network diffusion model.");
	auto seir = model_of<epimodels::ModelSEIR<int>>(
		m, "SEIR",
		"A model with four compartments: susceptible, exposed, infectious, and "
		"recovered.");
	auto seirconn = model_of<epimodels::ModelSEIRCONN<int>>(
		m, "SEIRCONN",
		"A model with four compartments: susceptible, exposed, infectious, and "
		"recovered (connected population).");
	auto seird = model_of<epimodels::ModelSEIRD<int>>(
		m, "SEIRD",
		"A model with five compartments: susceptible, exposed, infectious, "
		"recovered, and dead.");
	auto seirdconn = model_of<epimodels::ModelSEIRDCONN<int>>(
		m, "SEIRDCONN",
		"A model with five compartments: susceptible, exposed, infectious, "
		"recovered, and dead (connected population).");
	auto seirmixing = model_of<epimodels::ModelSEIRMixing<int>>(
		m, "SEIRMixing",
		"A SEIR model with multiple population groups connected via a contact "
		"matrix.");
	auto seirmixingquarantine = model_of<
		epimodels::ModelSEIRMixingQuarantine<int>>(
		m, "SEIRMixingQuarantine",
		"A SEIR model with mixing, quarantine, isolation, hospitalization, and "
		"contact tracing.");
	auto seirnetworkquarantine =
		model_of<epimodels::ModelSEIRNetworkQuarantine<int>>(
			m, "SEIRNetworkQuarantine",
			"A SEIR model with network-based contacts, quarantine, isolation, "
			"hospitalization, and contact tracing.");
	auto sir = model_of<epimodels::ModelSIR<int>>(
		m, "SIR",
		"A model with three compartments: susceptible, infectious, and "
		"recovered.");
	auto sirconn = model_of<epimodels::ModelSIRCONN<int>>(
		m, "SIRCONN",
		"A model with three compartments: susceptible, infectious, and "
		"recovered (connected population).");
	auto sird = model_of<epimodels::ModelSIRD<int>>(
		m, "SIRD",
		"A model with four compartments: susceptible, infectious, recovered, "
		"and dead.");
	auto sirdconn = model_of<epimodels::ModelSIRDCONN<int>>(
		m, "SIRDCONN",
		"A model with four compartments: susceptible, infectious, recovered, "
		"and dead (connected population).");
	auto sirmixing = model_of<epimodels::ModelSIRMixing<int>>(
		m, "SIRMixing",
		"A SIR model with multiple population groups connected via a contact "
		"matrix.");
	auto sis = model_of<epimodels::ModelSIS<int>>(
		m, "SIS", "A model with two compartments: susceptible and infectious.");
	auto sisd = model_of<epimodels::ModelSISD<int>>(
		m, "SISD",
		"A model with three compartments: susceptible, infectious, and death.");
	auto surv = model_of<epimodels::ModelSURV<int>>(
		m, "SURV",
		"A model where agents may be isolated, even when asymptomatic.");

	export_model_<epimodels::ModelDiffNet<int>>(
		diffnet, "DiffNet", make_arg<std::string>("name"),
		make_arg<double>("prevalence"), make_arg<double>("prob_adopt"),
		make_arg<bool>("normalize_exposure"), make_arg<double *>("data"),
		make_arg<int>("data_ncols"), make_arg<std::vector<size_t>>("data_cols"),
		make_arg<std::vector<double>>("params"));

	export_model_<epimodels::ModelSEIR<int>>(
		seir, "SEIR", make_arg<std::string>("name"),
		make_arg<double>("prevalence"), make_arg<double>("transmission_rate"),
		make_arg<double>("incubation_days"), make_arg<double>("recovery_rate"));

	export_model_<epimodels::ModelSEIRCONN<int>>(
		seirconn, "SEIRCONN", make_arg<std::string>("name"), make_arg<int>("n"),
		make_arg<double>("prevalence"), make_arg<double>("contact_rate"),
		make_arg<double>("transmission_rate"),
		make_arg<double>("incubation_days"), make_arg<double>("recovery_rate"));

	export_model_<epimodels::ModelSEIRD<int>>(
		seird, "SEIRD", make_arg<std::string>("name"),
		make_arg<double>("prevalence"), make_arg<double>("transmission_rate"),
		make_arg<double>("incubation_days"), make_arg<double>("recovery_rate"),
		make_arg<double>("death_rate"));

	export_model_<epimodels::ModelSEIRDCONN<int>>(
		seirdconn, "SEIRDCONN", make_arg<std::string>("name"),
		make_arg<int>("n"), make_arg<double>("prevalence"),
		make_arg<double>("contact_rate"), make_arg<double>("transmission_rate"),
		make_arg<double>("incubation_days"), make_arg<double>("recovery_rate"),
		make_arg<double>("death_rate"));

	export_model_<epimodels::ModelSEIRMixing<int>>(
		seirmixing, "SEIRMixing", make_arg<std::string>("vname"),
		make_arg<unsigned int>("n"), make_arg<double>("prevalence"),
		make_arg<double>("transmission_rate"),
		make_arg<double>("avg_incubation_days"),
		make_arg<double>("recovery_rate"),
		make_arg<std::vector<double>>("contact_matrix"));

	seirmixingquarantine.def(
		py::init<const std::string &, unsigned int, double, double, double,
				 double, std::vector<double>, double, double, double, int,
				 double, double, int, double, unsigned int>(),
		"Create a SEIR Mixing Quarantine model.", py::arg("vname"),
		py::arg("n"), py::arg("prevalence"), py::arg("transmission_rate"),
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
		"Create a SEIR Network Quarantine model (network-based contacts).",
		py::arg("vname"), py::arg("prevalence"), py::arg("transmission_rate"),
		py::arg("avg_incubation_days"), py::arg("recovery_rate"),
		py::arg("hospitalization_rate"), py::arg("hospitalization_period"),
		py::arg("days_undetected"), py::arg("quarantine_period"),
		py::arg("quarantine_willingness"), py::arg("isolation_willingness"),
		py::arg("isolation_period"),
		py::arg("contact_tracing_success_rate") = 1.0,
		py::arg("contact_tracing_days_prior") = 4u);

	export_model_<epimodels::ModelSIR<int>>(
		sir, "SIR", make_arg<std::string>("name"),
		make_arg<double>("prevalence"), make_arg<double>("transmission_rate"),
		make_arg<double>("recovery_rate"));

	export_model_<epimodels::ModelSIRCONN<int>>(
		sirconn, "SIRCONN", make_arg<std::string>("name"), make_arg<int>("n"),
		make_arg<double>("prevalence"), make_arg<double>("contact_rate"),
		make_arg<double>("transmission_rate"),
		make_arg<double>("recovery_rate"));

	export_model_<epimodels::ModelSIRD<int>>(
		sird, "SIRD", make_arg<std::string>("name"),
		make_arg<double>("prevalence"), make_arg<double>("transmission_rate"),
		make_arg<double>("recovery_rate"), make_arg<double>("death_rate"));

	export_model_<epimodels::ModelSIRDCONN<int>>(
		sirdconn, "SIRDCONN", make_arg<std::string>("name"), make_arg<int>("n"),
		make_arg<double>("prevalence"), make_arg<double>("contact_rate"),
		make_arg<double>("transmission_rate"),
		make_arg<double>("recovery_rate"), make_arg<double>("death_rate"));

	export_model_<epimodels::ModelSIRMixing<int>>(
		sirmixing, "SIRMixing", make_arg<std::string>("vname"),
		make_arg<unsigned int>("n"), make_arg<double>("prevalence"),
		make_arg<double>("transmission_rate"),
		make_arg<double>("recovery_rate"),
		make_arg<std::vector<double>>("contact_matrix"));

	export_model_<epimodels::ModelSIS<int>>(
		sis, "SIS", make_arg<std::string>("name"),
		make_arg<double>("prevalence"), make_arg<double>("transmission_rate"),
		make_arg<double>("recovery_rate"));

	export_model_<epimodels::ModelSISD<int>>(
		sisd, "SISD", make_arg<std::string>("name"),
		make_arg<double>("prevalence"), make_arg<double>("transmission_rate"),
		make_arg<double>("recovery_rate"), make_arg<double>("death_rate"));

	export_model_<epimodels::ModelSURV<int>>(
		surv, "SURV", make_arg<std::string>("name"),
		make_arg<double>("prevalence"), make_arg<double>("efficacy_vax"),
		make_arg<double>("latent_period"), make_arg<double>("prob_symptoms"),
		make_arg<double>("prop_vaccinated"),
		make_arg<double>("prop_vax_redux_transm"),
		make_arg<double>("infect_period"),
		make_arg<double>("prop_vax_redux_infect"),
		make_arg<double>("surveillance_prob"),
		make_arg<double>("transmission_rate"), make_arg<double>("prob_death"),
		make_arg<double>("prob_noreinfect"));
}
