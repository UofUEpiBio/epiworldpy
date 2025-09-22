#include "model.hpp"
#include "agent-meat-state.hpp"

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

template <bool Local>
static void update_susceptible_impl(Agent<int> *p, Model<int> *m) {
	Virus<int> *virus = sampler::sample_virus_single<int>(p, m);
	if constexpr (Local) {
		if (virus == nullptr)
			return;
	}
	if (virus != nullptr) {
		p->set_virus(*virus, m);
	}
}

template <bool Local>
static void update_exposed_impl(Agent<int> *p, Model<int> *m) {
	if (p->get_virus() == nullptr) {
		throw std::logic_error("Using the default_update_exposed on an agent "
							   "without a virus. Agent id " +
							   std::to_string(p->get_id()));
	}

	auto &virus = p->get_virus();
	m->array_double_tmp[0] =
		virus->get_prob_death(m) * (1.0 - p->get_death_reduction(virus, m));
	m->array_double_tmp[1] =
		1.0 - (1.0 - virus->get_prob_recovery(m)) *
				  (1.0 - p->get_recovery_enhancer(virus, m));

	int which = roulette(2, m);
	if (which < 0)
		return;

	if (which == 0) {
		p->rm_agent_by_virus(m);
	} else {
		p->rm_virus(m);
	}
}

void epiworldpy::export_update_fun(
	pybind11::class_<epiworld::UpdateFun<int>> &c) {
	c.def_static(
		 "default",
		 []() {
			 return (std::function<void(Agent<int> *, Model<int> *)>)nullptr;
		 })
		.def_static("default_update_susceptible",
					[] {
						return std::function<void(Agent<int> *, Model<int> *)>(
							update_susceptible_impl<true>);
					})
		.def_static("default_update_exposed", [] {
			return std::function<void(Agent<int> *, Model<int> *)>(
				update_exposed_impl<true>);
		});
}

void epiworldpy::export_model(py::class_<epiworld::Model<int>> &c) {
	c.def(py::init<>(), "Create a new empty model.")
		.def("add_state",
			 static_cast<void (epiworld::Model<>::*)(
				 std::string, UpdateFun<int>)>(&Model<int>::add_state),
			 py::arg("lab"), py::arg("fun"))
		.def("get_states", &Model<int>::get_states)
		.def("get_name", &Model<int>::get_name,
			 "Get the name of the type of model.")
		.def("agents_from_edgelist", &Model<int>::agents_from_edgelist,
			 "Populatates the model's agents from an edge list.",
			 py::arg("source"), py::arg("target"), py::arg("size"),
			 py::arg("directed"))
		.def(
			"add_virus",
			[](Model<int> &model, Virus<int> virus) {
				virus.set_state(1, 2, 3);
				model.add_virus(virus);
			},
			"Adds a virus to the model", py::arg("virus"))
		.def("add_tool", &Model<int>::add_tool,
			 "Adds a tool to modify the model.", py::arg("tool"))
		.def("add_entity", &Model<int>::add_entity)
		.def("get_entity", &Model<int>::get_entity)
		.def("agents_smallworld", &Model<int>::agents_smallworld,
			 "Populate the model without an edgelist.", py::arg("n"),
			 py::arg("k"), py::arg("d"), py::arg("p"))
		.def("print", &Model<int>::print, "Print a summary of the model run.",
			 py::arg("summary") = true)
		.def("run", &Model<int>::run,
			 "Run the model according to the previously specific parameters.",
			 py::arg("ndays"), py::arg("seed") = 1u)
		.def("get_db", py::overload_cast<>(&Model<int>::get_db),
			 py::return_value_policy::reference_internal,
			 "Get the data from the model run, which may then be queried with "
			 "associated methods.");
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
	/* Export models.
	 *
	 * TODO: Make Python model proxys inhereit from an interface and do this in
	 * a loop.
	 */

	auto diffnet = model_of<epimodels::ModelDiffNet<int>>(
		m, "DiffNet", "A network diffusion model.");
	auto seir = model_of<epimodels::ModelSEIR<int>>(
		m, "SEIR",
		"A model with four compartments: susceptible, exposed, infectious, and "
		"recovered.");
	auto seirconn = model_of<epimodels::ModelSEIRCONN<int>>(
		m, "SEIRCONN",
		"A model with four compartments: susceptible, exposed, infectious, and "
		"recovered.");
	auto seird = model_of<epimodels::ModelSEIRD<int>>(
		m, "SEIRD",
		"A model with five compartments: susceptible, exposed, infectious, "
		"recovered, and dead.");
	auto sir = model_of<epimodels::ModelSIR<int>>(
		m, "SIR",
		"A model with three compartments: susceptible, infectious, and "
		"recovered.");
	auto sirconn = model_of<epimodels::ModelSIRCONN<int>>(
		m, "SIRCONN",
		"A model with three compartments: susceptible, infectious, and "
		"recovered.");
	auto sird = model_of<epimodels::ModelSIRD<int>>(
		m, "SIRD",
		"A model with four compartments: susceptible, infectious, recovered, "
		"and dead.");
	auto sirdconn = model_of<epimodels::ModelSIRDCONN<int>>(
		m, "SIRDCONN",
		"A model with four compartments: susceptible, infectious, recovered, "
		"and dead.");
	auto sis = model_of<epimodels::ModelSIS<int>>(
		m, "SIS", "A model wth two compartments: susceptible and infectious.");
	auto sisd = model_of<epimodels::ModelSISD<int>>(
		m, "SISD",
		"A model wth three compartments: susceptible, infectious, and death.");
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
