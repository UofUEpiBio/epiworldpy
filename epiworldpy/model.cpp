#include "model.hpp"
#include "docstrings/epimodels.hpp"
#include "docstrings/model.hpp"
#include "natural_history.hpp"

#include <pybind11/functional.h>
#include <pybind11/stl.h>

using namespace epiworldpy;
using namespace epiworld;
namespace py = pybind11;
namespace doc = epiworldpy::docstrings::model;
namespace mdoc = epiworldpy::docstrings::epimodels;

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

static auto get_elapsed(const Model &self, std::string unit) -> py::dict {
	epiworld_double last, total;
	std::string abbr;
	self.get_elapsed(std::move(unit), &last, &total, &abbr, false);
	return py::dict(py::arg("last") = last, py::arg("total") = total,
					py::arg("unit") = abbr);
}

static void run_multiple(Model &m, int ndays, int nexperiments, int seed,
						 const py::object &fun, bool reset, bool verbose,
						 int nthreads) {
	std::function<void(size_t, Model *)> cb;
	if (fun.is_none()) {
		cb = make_save_run();
	} else {
		cb = fun.cast<std::function<void(size_t, Model *)>>();
	}

	m.run_multiple(ndays, nexperiments, seed, cb, reset, verbose, nthreads);
}

static auto get_statuses(const Model &self) -> std::vector<std::string> {
	const Statuses *st = self.get_statuses();
	if (st == nullptr)
		return {"Free"};
	return st->get_stages();
}

/* The label of each agent's stage (or status; "Free" without statuses) */
static auto agents_labels(Model &self, bool status)
	-> std::vector<std::string> {
	if (status && (self.get_statuses() == nullptr))
		return std::vector<std::string>(self.size(), "Free");
	const std::vector<std::string> &labels =
		status ? self.get_statuses()->get_stages() : model_nh(self).get_stages();
	std::vector<std::string> out;
	out.reserve(self.size());
	for (const auto &a : self.get_agents())
		out.push_back(labels[static_cast<size_t>(status ? self.get_status(a)
														: self.get_stage(a))]);
	return out;
}

void epiworldpy::export_model(py::class_<epiworld::Model> &c) {
	c.def(py::init<size_t>(), doc::init, py::arg("n"))
		.def(
			"add_layer",
			[](Model &self, std::shared_ptr<ContactLayer> layer) {
				self.add_layer(std::move(layer));
			},
			doc::add_layer, py::arg("layer"))
		.def(
			"get_layer",
			[](Model &self, std::string_view name) -> ContactLayer & {
				return self.get_layer(name);
			},
			py::return_value_policy::reference_internal, doc::get_layer,
			py::arg("name"))
		.def(
			"get_layer",
			[](Model &self, size_t id) -> ContactLayer & {
				return self.get_layer(id);
			},
			py::return_value_policy::reference_internal, doc::get_layer_id,
			py::arg("id"))
		.def("get_name", &Model::get_name, doc::get_name)
		.def("get_n_viruses", &Model::get_n_viruses, doc::get_n_viruses)
		.def("get_n_tools", &Model::get_n_tools, doc::get_n_tools)
		.def("get_ndays", &Model::get_ndays, doc::get_ndays)
		.def("get_n_replicates", &Model::get_n_replicates,
			 doc::get_n_replicates)
		.def("get_sim_id", &Model::get_sim_id, doc::get_sim_id)
		.def("today", &Model::today, doc::today)
		.def(
			"add_virus",
			[](Model &self, const Virus &virus, const PyAssign *assign) {
				// A placement's labels are the virus's stages
				self.add_virus(
					virus,
					assign_from_py(assign, assign ? stage_resolver(virus_nh(virus))
												  : LabelOf()));
			},
			doc::add_virus, py::arg("virus"), py::arg("assign") = py::none())
		.def(
			"assign_virus",
			[](Model &self, size_t virus_pos, const PyAssign &assign) {
				self.assign_virus(virus_pos,
								  assign_from_py(&assign,
												 stage_resolver(model_nh(self))));
			},
			doc::assign_virus, py::arg("virus_pos"), py::arg("assign"))
		.def("rm_virus", &Model::rm_virus, doc::rm_virus,
			 py::arg("virus_pos"))
		.def(
			"get_virus",
			[](Model &self, size_t virus_pos) -> Virus & {
				return self.get_virus(virus_pos);
			},
			py::return_value_policy::reference_internal, doc::get_virus,
			py::arg("virus_pos"))
		.def(
			"add_tool",
			[](Model &self, const Tool &tool, const PyAssign *assign) {
				// A tool's placement takes no stage names
				self.add_tool(tool, assign_from_py(assign, LabelOf()));
			},
			doc::add_tool, py::arg("tool"), py::arg("assign") = py::none())
		.def(
			"assign_tool",
			[](Model &self, size_t tool_pos, const PyAssign &assign) {
				self.assign_tool(tool_pos, assign_from_py(&assign, LabelOf()));
			},
			doc::assign_tool, py::arg("tool_pos"), py::arg("assign"))
		.def("rm_tool", &Model::rm_tool, doc::rm_tool, py::arg("tool_pos"))
		.def(
			"get_tool",
			[](Model &self, size_t tool_pos) -> Tool & {
				return self.get_tool(tool_pos);
			},
			py::return_value_policy::reference_internal, doc::get_tool,
			py::arg("tool_pos"))
		.def(
			"set_initial",
			[](Model &self, const PyAssign &assign) {
				// The labels are the model's stages
				self.set_initial(
					assign_from_py(&assign, stage_resolver(model_nh(self))));
			},
			doc::set_initial, py::arg("assign"))
		.def(
			"get_stages",
			[](const Model &self) { return model_nh(self).get_stages(); },
			doc::get_stages)
		.def("get_statuses", &get_statuses, doc::get_statuses)
		.def(
			"get_stage",
			[](const Model &self, const Agent &agent) {
				return model_nh(self).get_stages()[static_cast<size_t>(
					self.get_stage(agent))];
			},
			doc::get_stage, py::arg("agent"))
		.def(
			"get_status",
			[](const Model &self, const Agent &agent) -> std::string {
				const Statuses *st = self.get_statuses();
				if (st == nullptr)
					return "Free";
				return st->get_stages()[static_cast<size_t>(
					self.get_status(agent))];
			},
			doc::get_status, py::arg("agent"))
		.def(
			"get_agents_stages",
			[](Model &self) { return agents_labels(self, false); },
			doc::get_agents_stages)
		.def(
			"get_agents_statuses",
			[](Model &self) { return agents_labels(self, true); },
			doc::get_agents_statuses)
		.def(
			"fire",
			[](Model &self, std::string_view transition, size_t agent_id) {
				return self.fire(self.transition_id(transition),
								 self.get_agent(agent_id));
			},
			doc::fire, py::arg("transition"), py::arg("agent_id"))
		.def("draw_natural_history", &Model::draw_natural_history,
			 doc::draw_natural_history)
		.def("reset", &Model::reset, doc::reset)
		.def(
			"print", [](const Model &m, bool lite) { m.print(lite); },
			doc::print, py::arg("lite") = false)
		.def("run", &Model::run, doc::run, py::arg("ndays"),
			 py::arg("seed") = -1)
		.def("run_multiple", &run_multiple, doc::run_multiple,
			 py::arg("ndays"), py::arg("nexperiments"), py::arg("seed_") = -1,
			 py::arg("fun") = py::none(), py::arg("reset") = true,
			 py::arg("verbose") = true, py::arg("nthreads") = 1)
		.def_static("make_save_run", &make_save_run, doc::make_save_run,
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
		.def("verbose_on", &Model::verbose_on, doc::verbose_on)
		.def("verbose_off", &Model::verbose_off, doc::verbose_off)
		.def("get_verbose", &Model::get_verbose, doc::get_verbose)
		.def(
			"params",
			[](Model &self) -> std::map<std::string, epiworld_double> {
				return self.params();
			},
			doc::params)
		.def("add_param", &Model::add_param, doc::add_param,
			 py::arg("initial_val"), py::arg("pname"),
			 py::arg("overwrite") = false)
		.def("get_param", &Model::get_param, doc::get_param, py::arg("pname"))
		.def("set_param", &Model::set_param, doc::set_param, py::arg("pname"),
			 py::arg("val"))
		.def("par", &Model::par, doc::par, py::arg("pname"))
		.def(
			"get_agent",
			[](Model &self, size_t i) -> Agent & { return self.get_agent(i); },
			py::return_value_policy::reference_internal, doc::get_agent,
			py::arg("i"))
		.def(
			"get_agents",
			[](Model &self) -> std::vector<Agent> & {
				return self.get_agents();
			},
			py::return_value_policy::reference_internal, doc::get_agents)
		.def(
			"add_globalevent",
			[](Model &self, Bubbles &bubbles) { self.add_globalevent(bubbles); },
			doc::add_globalevent_bubbles, py::arg("bubbles"))
		.def(
			"add_globalevent",
			[](Model &self, std::function<void(Model *)> fun, std::string name,
			   int date) { self.add_globalevent(fun, name, date); },
			doc::add_globalevent, py::arg("fun"),
			py::arg("name") = "global event", py::arg("date") = -99)
		.def("rm_globalevent",
			 py::overload_cast<std::string>(&Model::rm_globalevent),
			 doc::rm_globalevent, py::arg("name"))
		.def("run_globalevents", &Model::run_globalevents,
			 doc::run_globalevents)
		.def("get_db", py::overload_cast<>(&Model::get_db),
			 py::return_value_policy::reference_internal, doc::get_db)
		.def("size", &Model::size, doc::size)
		.def("__len__", &Model::size, doc::len)
		.def("seed", &Model::seed, doc::seed, py::arg("s"))
		.def("set_name", &Model::set_name, doc::set_name, py::arg("name"))
		.def(
			"set_transmission_mode",
			[](Model &self, std::string_view mode, double kappa) -> Model & {
				return self.set_transmission_mode(mode, kappa);
			},
			py::return_value_policy::reference_internal,
			doc::set_transmission_mode, py::arg("mode"),
			py::arg("kappa") = EPI_DEFAULT_TRANSMISSION_KAPPA)
		.def(
			"get_transmission_mode",
			[](const Model &self) {
				return transmission_mode_to_str(self.get_transmission_mode());
			},
			doc::get_transmission_mode)
		.def(
			"get_last_transmission_mode",
			[](const Model &self) {
				return transmission_mode_to_str(
					self.get_last_transmission_mode());
			},
			doc::get_last_transmission_mode)
		.def("get_transmission_kappa", &Model::get_transmission_kappa,
			 doc::get_transmission_kappa)
		.def("has_param", &Model::has_param, doc::has_param, py::arg("pname"))
		.def("has_globalevent", &Model::has_globalevent, doc::has_globalevent,
			 py::arg("name"))
		.def("get_n_globalevents", &Model::get_n_globalevents,
			 doc::get_n_globalevents)
		.def("queuing_on", &Model::queuing_on, doc::queuing_on)
		.def(
			"queuing_off",
			[](Model &self) -> Model & { return self.queuing_off(); },
			py::return_value_policy::reference_internal, doc::queuing_off)
		.def("is_queuing_on", &Model::is_queuing_on, doc::is_queuing_on)
		.def("get_elapsed", &get_elapsed, doc::get_elapsed,
			 py::arg("unit") = "auto")
		.def(
			"write_data",
			[](const Model &self, std::string fn_virus_info,
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
			doc::write_data, py::arg("fn_virus_info") = std::string(""),
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

/* The registry: every built-in model whose arguments are numbers is built by
 * make_model() (the named constructors in epiworldpy.epimodels call it). */
static const ModelRegistry &registry() {
	static const ModelRegistry reg = [] {
		ModelRegistry r;
		epimodels::register_models(r);
		return r;
	}();
	return reg;
}

static auto model_info_dict(const ModelInfo &info, bool with_params)
	-> py::dict {
	py::dict d(py::arg("id") = info.id, py::arg("label") = info.label,
			   py::arg("description") = info.description,
			   py::arg("virus_name") = info.virus_name);
	if (with_params) {
		py::list params;
		for (const auto &p : info.params)
			params.append(py::dict(
				py::arg("arg") = p.arg, py::arg("name") = p.name,
				py::arg("value") = p.value, py::arg("min") = p.min,
				py::arg("max") = p.max, py::arg("integer") = p.integer,
				py::arg("description") = p.description));
		d["params"] = params;
	}
	return d;
}

/* A named constructor: `pyname(name, <args>)` is make_model(id, {args}, name),
 * with every argument required. */
template <size_t N, size_t... I>
static void def_shim(py::module &m, const char *pyname, std::string id,
					 const char *doc, std::array<const char *, N> args,
					 std::index_sequence<I...>) {
	m.def(
		pyname,
		[id, args](const std::string &name, decltype((void)I, double())... v) {
			return registry().make_model(id, {{args[I], v}...}, name);
		},
		doc, py::arg("name"), py::arg(args[I])...);
}

template <size_t N>
static void def_shim(py::module &m, const char *pyname, std::string id,
					 const char *doc, std::array<const char *, N> args) {
	def_shim(m, pyname, std::move(id), doc, args, std::make_index_sequence<N>{});
}

void epiworldpy::export_all_models(pybind11::module &m) {

	// The named constructors (the argument names are the registry's)
	def_shim<4>(m, "ModelSIR", "SIR", mdoc::sir,
				{"n", "prevalence", "transmission_prob", "recovery_rate"});
	def_shim<5>(m, "ModelSIRD", "SIRD", mdoc::sird,
				{"n", "prevalence", "transmission_prob", "recovery_rate",
				 "death_rate"});
	def_shim<5>(m, "ModelSEIR", "SEIR", mdoc::seir,
				{"n", "prevalence", "transmission_prob", "latent_period",
				 "recovery_rate"});
	def_shim<6>(m, "ModelSEIRD", "SEIRD", mdoc::seird,
				{"n", "prevalence", "transmission_prob", "latent_period",
				 "recovery_rate", "death_rate"});
	def_shim<4>(m, "ModelSIS", "SIS", mdoc::sis,
				{"n", "prevalence", "transmission_prob", "recovery_rate"});
	def_shim<5>(m, "ModelSISD", "SISD", mdoc::sisd,
				{"n", "prevalence", "transmission_prob", "recovery_rate",
				 "death_rate"});
	def_shim<13>(m, "ModelSURV", "SURV", mdoc::surv,
				 {"n", "prevalence", "efficacy_vax", "latent_period",
				  "infect_period", "prob_symptoms", "prop_vaccinated",
				  "prop_vax_redux_transm", "prop_vax_redux_infect",
				  "surveillance_prob", "transmission_prob", "prob_death",
				  "prob_noreinfect"});

	// The last two arguments of the quarantine model have defaults
	m.def(
		"ModelSEIRQuarantine",
		[](const std::string &name, double n, double prevalence,
		   double transmission_prob, double latent_period,
		   double recovery_rate, double hospitalization_rate,
		   double hospitalization_period, double detection_delay,
		   double quarantine_period, double quarantine_willingness,
		   double isolation_willingness, double isolation_period,
		   double contact_tracing_prob, double contact_tracing_window) {
			return registry().make_model(
				"SEIRQuarantine",
				{{"n", n},
				 {"prevalence", prevalence},
				 {"transmission_prob", transmission_prob},
				 {"latent_period", latent_period},
				 {"recovery_rate", recovery_rate},
				 {"hospitalization_rate", hospitalization_rate},
				 {"hospitalization_period", hospitalization_period},
				 {"detection_delay", detection_delay},
				 {"quarantine_period", quarantine_period},
				 {"quarantine_willingness", quarantine_willingness},
				 {"isolation_willingness", isolation_willingness},
				 {"isolation_period", isolation_period},
				 {"contact_tracing_prob", contact_tracing_prob},
				 {"contact_tracing_window", contact_tracing_window}},
				name);
		},
		mdoc::seirquarantine, py::arg("name"), py::arg("n"),
		py::arg("prevalence"), py::arg("transmission_prob"),
		py::arg("latent_period"), py::arg("recovery_rate"),
		py::arg("hospitalization_rate"), py::arg("hospitalization_period"),
		py::arg("detection_delay"), py::arg("quarantine_period"),
		py::arg("quarantine_willingness"), py::arg("isolation_willingness"),
		py::arg("isolation_period"), py::arg("contact_tracing_prob") = 1.0,
		py::arg("contact_tracing_window") = 4.0);

	m.def(
		 "make_model",
		 [](const std::string &id, std::map<std::string, double> values,
			const std::string &virus_name) {
			 return registry().make_model(id, values, virus_name);
		 },
		 mdoc::make_model, py::arg("id"),
		 py::arg("values") = std::map<std::string, double>{},
		 py::arg("virus_name") = "")
		.def(
			"list_models",
			[] {
				py::list out;
				for (const auto &info : registry().list_models())
					out.append(model_info_dict(info, false));
				return out;
			},
			mdoc::list_models)
		.def(
			"get_model_info",
			[](const std::string &id) {
				return model_info_dict(registry().get_model_info(id), true);
			},
			mdoc::get_model_info, py::arg("id"));

	// The diffusion network model takes data, so it keeps its constructor
	py::class_<epimodels::ModelDiffNet, Model>(m, "ModelDiffNet", mdoc::diffnet)
		.def(py::init<std::string, epiworld_fast_uint, double, double, bool,
					  double *, int, std::vector<size_t>,
					  std::vector<double>>(),
			 mdoc::diffnet_init, py::arg("name"), py::arg("n"),
			 py::arg("prevalence"), py::arg("prob_adopt"),
			 py::arg("normalize_exposure"), py::arg("data"),
			 py::arg("data_ncols"), py::arg("data_cols"), py::arg("params"));
}
