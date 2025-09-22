#include "model.hpp"
#include "agent-meat-state.hpp"

#define MODEL_OF(m, model, description)                                        \
	py::class_<epimodels::Model##model<int>, Model<int>>(m, "Model" #model,    \
														 description)

using namespace epiworldpy;
using namespace epiworld;
namespace py = pybind11;

static std::shared_ptr<DataBase<int>> get_db(Model<int> &self) {
	return std::shared_ptr<DataBase<int>>(
		&self.get_db(), [](DataBase<int> *) { /* do nothing, no delete */ });
}

static void update_susceptible(Agent<int> *p, Model<int> *m) {

	Virus<int> *virus = sampler::sample_virus_single<int>(p, m);

	if (virus != nullptr) {
		p->set_virus(*virus, m);
	}
}

static void update_exposed(Agent<int> *p, Model<int> *m) {
	if (p->get_virus() == nullptr)
		throw std::logic_error(
			std::string("Using the -default_update_exposed- on agents WITHOUT "
						"viruses makes no sense! ") +
			std::string("Agent id ") + std::to_string(p->get_id()) +
			std::string(" has no virus registered."));

	auto &virus = p->get_virus();
	m->array_double_tmp[0u] =
		virus->get_prob_death(m) * (1.0 - p->get_death_reduction(virus, m));

	m->array_double_tmp[1u] =
		1.0 - (1.0 - virus->get_prob_recovery(m)) *
				  (1.0 - p->get_recovery_enhancer(virus, m));

	int which = roulette(2u, m);

	if (which < 0) {
		return;
	}

	if (which == 0u) {
		p->rm_agent_by_virus(m);
	} else {
		p->rm_virus(m);
	}
}

static void local_default_update_susceptible(Agent<int> *p, Model<int> *m) {
	Virus<int> *virus = sampler::sample_virus_single<int>(p, m);

	if (virus == nullptr)
		return;

	p->set_virus(*virus, m);
}

static void local_default_update_exposed(Agent<int> *p, Model<int> *m) {
	if (p->get_virus() == nullptr) {
		throw std::logic_error(
			std::string("Using the -default_update_exposed- on agents WITHOUT "
						"viruses makes no sense! ") +
			std::string("Agent id ") + std::to_string(p->get_id()) +
			std::string(" has no virus registered."));
	}

	auto &virus = p->get_virus();
	m->array_double_tmp[0u] =
		virus->get_prob_death(m) * (1.0 - p->get_death_reduction(virus, m));

	m->array_double_tmp[1u] =
		1.0 - (1.0 - virus->get_prob_recovery(m)) *
				  (1.0 - p->get_recovery_enhancer(virus, m));

	int which = roulette(2u, m);
	if (which < 0)
		return;

	if (which == 0u) {
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
					[]() {
						return (std::function<void(Agent<int> *, Model<int> *)>)
							local_default_update_susceptible;
					})
		.def_static("default_update_exposed", []() {
			return (std::function<void(Agent<int> *, Model<int> *)>)
				local_default_update_exposed;
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
		.def("get_db", &get_db,
			 "Get the data from the model run, which may then be queried with "
			 "associated methods.",
			 py::return_value_policy::reference);
}

void epiworldpy::export_all_models(pybind11::module &m) {
	/* Export models.
	 *
	 * TODO: Make Python model proxys inhereit from an interface and do this in
	 * a loop.
	 */
	auto diffnet = MODEL_OF(m, DiffNet, "A network diffusion model.");
	auto seir = MODEL_OF(m, SEIR,
						 "A model with four compartments: susceptible, "
						 "exposed, infectious, and recovered.");
	auto seirconn = MODEL_OF(
		m, SEIRCONN,
		"A model with four compartments: susceptible, exposed, infectious, and "
		"recovered.");
	auto seird = MODEL_OF(m, SEIRD,
						  "A model with five compartments: susceptible, "
						  "exposed, infectious, recovered, and dead.");
	auto sir = MODEL_OF(m, SIR,
						"A model with three compartments: "
						"susceptible, infectious, and recovered.");
	auto sirconn = MODEL_OF(m, SIRCONN,
							"A model with three compartments: susceptible, "
							"infectious, and recovered.");
	auto sird = MODEL_OF(m, SIRD,
						 "A model with four compartments: susceptible, "
						 "infectious, recovered, and dead.");
	auto sirdconn = MODEL_OF(m, SIRDCONN,
							 "A model with four compartments: susceptible, "
							 "infectious, recovered, and dead.");
	auto sis = MODEL_OF(
		m, SIS, "A model wth two compartments: susceptible and infectious.");
	auto sisd = MODEL_OF(
		m, SISD,
		"A model wth three compartments: susceptible, infectious, and death.");
	auto surv = MODEL_OF(
		m, SURV,
		"A model where agents may be isolated, even when asymptomatic.");

	epiworldpy::export_diffnet(diffnet);
	epiworldpy::export_seir(seir);
	epiworldpy::export_seirconn(seirconn);
	epiworldpy::export_seird(seird);
	epiworldpy::export_sir(sir);
	epiworldpy::export_sirconn(sirconn);
	epiworldpy::export_sird(sird);
	epiworldpy::export_sirdconn(sirdconn);
	epiworldpy::export_sis(sis);
	epiworldpy::export_sisd(sisd);
	epiworldpy::export_surv(surv);
}

void epiworldpy::export_diffnet(MODEL_CHILD_TYPE(DiffNet) & c) {
	c.def(py::init<std::string, double, double, bool, double *, int,
				   std::vector<size_t>, std::vector<double>>(),
		  "Create a new DiffNet model.", py::arg("name"), py::arg("prevalence"),
		  py::arg("prob_adopt"), py::arg("normalize_exposure"), py::arg("data"),
		  py::arg("data_ncols"), py::arg("data_cols"), py::arg("params"));
}

void epiworldpy::export_seir(MODEL_CHILD_TYPE(SEIR) & c) {
	c.def(py::init<std::string, double, double, double, double>(),
		  "Create a new SEIR model.", py::arg("name"), py::arg("prevalence"),
		  py::arg("transmission_rate"), py::arg("incubation_days"),
		  py::arg("recovery_rate"));
}

void epiworldpy::export_seirconn(MODEL_CHILD_TYPE(SEIRCONN) & c) {
	c.def(py::init<std::string, int, double, double, double, double, double>(),
		  "Create a new SEIRCONN model.", py::arg("name"), py::arg("n"),
		  py::arg("prevalence"), py::arg("contact_rate"),
		  py::arg("transmission_rate"), py::arg("incubation_days"),
		  py::arg("recovery_rate"));
}

void epiworldpy::export_seird(MODEL_CHILD_TYPE(SEIRD) & c) {
	c.def(py::init<std::string, double, double, double, double, double>(),
		  "Create a new SEIRD model.", py::arg("name"), py::arg("prevalence"),
		  py::arg("transmission_rate"), py::arg("incubation_days"),
		  py::arg("recovery_rate"), py::arg("death_rate"));
}

void epiworldpy::export_sir(MODEL_CHILD_TYPE(SIR) & c) {
	c.def(py::init<std::string, double, double, double>(),
		  "Create a new SIR model.", py::arg("name"), py::arg("prevalence"),
		  py::arg("transmission_rate"), py::arg("recovery_rate"));
}

void epiworldpy::export_sirconn(MODEL_CHILD_TYPE(SIRCONN) & c) {
	c.def(py::init<std::string, int, double, double, double, double>(),
		  "Create a new SIRCONN model.", py::arg("name"), py::arg("n"),
		  py::arg("prevalence"), py::arg("contact_rate"),
		  py::arg("transmission_rate"), py::arg("recovery_rate"));
}

void epiworldpy::export_sird(MODEL_CHILD_TYPE(SIRD) & c) {
	c.def(py::init<std::string, double, double, double, double>(),
		  "Create a new SIRD model.", py::arg("name"), py::arg("prevalence"),
		  py::arg("transmission_rate"), py::arg("recovery_rate"),
		  py::arg("death_rate"));
}

void epiworldpy::export_sirdconn(MODEL_CHILD_TYPE(SIRDCONN) & c) {
	c.def(py::init<std::string, int, double, double, double, double, double>(),
		  "Create a new SIRDCONN model.", py::arg("name"), py::arg("n"),
		  py::arg("prevalence"), py::arg("contact_rate"),
		  py::arg("transmission_rate"), py::arg("recovery_rate"),
		  py::arg("death_rate"));
}

void epiworldpy::export_sis(MODEL_CHILD_TYPE(SIS) & c) {
	c.def(py::init<std::string, double, double, double>(),
		  "Create a new SIS model.", py::arg("name"), py::arg("prevalence"),
		  py::arg("transmission_rate"), py::arg("recovery_rate"));
}

void epiworldpy::export_sisd(MODEL_CHILD_TYPE(SISD) & c) {
	c.def(py::init<std::string, double, double, double, double>(),
		  "Create a new SISD model.", py::arg("name"), py::arg("prevalence"),
		  py::arg("transmission_rate"), py::arg("recovery_rate"),
		  py::arg("death_rate"));
}

void epiworldpy::export_surv(MODEL_CHILD_TYPE(SURV) & c) {
	/*
	 * TODO: I noticed that epiworld::epimodels::ModelSURV is overloaded to
	 * accept a variable number of arguments, should this be expressed on the
	 * Python side? It isn't in R, as far as I can tell, and I don't know how
	 * concerning symmetry is.
	 */
	c.def(py::init<std::string, double, double, double, double, double, double,
				   double, double, double, double, double, double>(),
		  "Create a new SURV model.", py::arg("name"), py::arg("prevalence"),
		  py::arg("efficacy_vax"), py::arg("latent_period"),
		  py::arg("prob_symptoms"), py::arg("prop_vaccinated"),
		  py::arg("prop_vax_redux_transm"), py::arg("infect_period"),
		  py::arg("prop_vax_redux_infect"), py::arg("surveillance_prob"),
		  py::arg("transmission_rate"), py::arg("prob_death"),
		  py::arg("prob_noreinfect"));
}
