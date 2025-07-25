#ifndef EPIWORLD_MODELS_SEIRMIXING_HPP
#define EPIWORLD_MODELS_SEIRMIXING_HPP

#define MM(i, j, n) \
    j * n + i

#if __cplusplus >= 202302L
    // C++23 or later
    #define GET_MODEL(model, output) \
        auto * output = dynamic_cast< ModelSEIRMixing<TSeq> * >( (model) ); \
        /*Using the [[assume(...)]] to avoid the compiler warning \
        if the standard is C++23 or later */ \
        [[assume((output) != nullptr)]]
#else
    // C++17 or C++20
    #define GET_MODEL(model, output) \
        auto * output = dynamic_cast< ModelSEIRMixing<TSeq> * >( (model) ); \
        assert((output) != nullptr); // Use assert for runtime checks
#endif

/**
 * @file seirentitiesconnected.hpp
 * @brief Template for a Susceptible-Exposed-Infected-Removed (SEIR) model with mixing
 */
template<typename TSeq = EPI_DEFAULT_TSEQ>
class ModelSEIRMixing : public epiworld::Model<TSeq> 
{
private:
    
    // Vector of vectors of infected agents     
    std::vector< size_t > infected;

    // Number of infected agents in each group
    std::vector< size_t > n_infected_per_group;
    
    // Where the agents start in the `infected` vector
    std::vector< size_t > entity_indices;

    void update_infected_list();
    std::vector< size_t > sampled_agents;
    size_t sample_agents(
        epiworld::Agent<TSeq> * agent,
        std::vector< size_t > & sampled_agents
        );
    std::vector< double > adjusted_contact_rate;
    std::vector< double > contact_matrix;

    #ifdef EPI_DEBUG
    std::vector< int > sampled_sizes;
    #endif

public:

    static const int SUSCEPTIBLE = 0;
    static const int EXPOSED     = 1;
    static const int INFECTED    = 2;
    static const int RECOVERED   = 3;

    ModelSEIRMixing() {};
    
    /**
     * @brief Constructs a ModelSEIRMixing object.
     *
     * @param model A reference to an existing ModelSEIRMixing object.
     * @param vname The name of the ModelSEIRMixing object.
     * @param n The number of entities in the model.
     * @param prevalence The initial prevalence of the disease in the model.
     * @param contact_rate The contact rate between entities in the model.
     * @param transmission_rate The transmission rate of the disease in the model.
     * @param avg_incubation_days The average incubation period of the disease in the model.
     * @param recovery_rate The recovery rate of the disease in the model.
     * @param contact_matrix The contact matrix between entities in the model. Specified in
     * column-major order.
     */
    ModelSEIRMixing(
        ModelSEIRMixing<TSeq> & model,
        const std::string & vname,
        epiworld_fast_uint n,
        epiworld_double prevalence,
        epiworld_double contact_rate,
        epiworld_double transmission_rate,
        epiworld_double avg_incubation_days,
        epiworld_double recovery_rate,
        std::vector< double > contact_matrix
    );
    
    /**
     * @brief Constructs a ModelSEIRMixing object.
     *
     * @param vname The name of the ModelSEIRMixing object.
     * @param n The number of entities in the model.
     * @param prevalence The initial prevalence of the disease in the model.
     * @param contact_rate The contact rate between entities in the model.
     * @param transmission_rate The transmission rate of the disease in the model.
     * @param avg_incubation_days The average incubation period of the disease in the model.
     * @param recovery_rate The recovery rate of the disease in the model.
     * @param contact_matrix The contact matrix between entities in the model.
     */
    ModelSEIRMixing(
        const std::string & vname,
        epiworld_fast_uint n,
        epiworld_double prevalence,
        epiworld_double contact_rate,
        epiworld_double transmission_rate,
        epiworld_double avg_incubation_days,
        epiworld_double recovery_rate,
        std::vector< double > contact_matrix
    );

    ModelSEIRMixing<TSeq> & run(
        epiworld_fast_uint ndays,
        int seed = -1
    );

    void reset();

    Model<TSeq> * clone_ptr();

    /**
     * @brief Set the initial states of the model
     * @param proportions_ Double vector with a single element:
     * - The proportion of non-infected individuals who have recovered.
    */
    ModelSEIRMixing<TSeq> & initial_states(
        std::vector< double > proportions_,
        std::vector< int > queue_ = {}
    );

    void set_contact_matrix(std::vector< double > cmat)
    {
        contact_matrix = cmat;
        return;
    };

};

template<typename TSeq>
inline void ModelSEIRMixing<TSeq>::update_infected_list()
{

    auto & agents = Model<TSeq>::get_agents();

    std::fill(n_infected_per_group.begin(), n_infected_per_group.end(), 0u);
    
    for (auto & a : agents)
    {

        if (a.get_state() == ModelSEIRMixing<TSeq>::INFECTED)
        {
            if (a.get_n_entities() > 0u)
            {
                const auto & entity = a.get_entity(0u);
                infected[
                    // Position of the group in the `infected` vector
                    entity_indices[entity.get_id()] +
                    // Position of the agent in the group
                    n_infected_per_group[entity.get_id()]++
                ] = a.get_id();

            }
        }

    }

    return;

}

template<typename TSeq>
inline size_t ModelSEIRMixing<TSeq>::sample_agents(
    epiworld::Agent<TSeq> * agent,
    std::vector< size_t > & sampled_agents
    )
{

    size_t agent_group_id = agent->get_entity(0u).get_id();
    size_t ngroups = this->entities.size();

    int samp_id = 0;
    for (size_t g = 0; g < ngroups; ++g)
    {

        size_t group_size = n_infected_per_group[g];

        // How many from this entity?
        int nsamples = epiworld::Model<TSeq>::rbinom(
            group_size,
            adjusted_contact_rate[g] * contact_matrix[
                MM(agent_group_id, g, ngroups)
            ]
        );

        if (nsamples == 0)
            continue;

        // Sampling from the entity
        for (int s = 0; s < nsamples; ++s)
        {

            // Randomly selecting an agent
            int which = epiworld::Model<TSeq>::runif() * group_size;

            // Correcting overflow error
            if (which >= static_cast<int>(group_size))
                which = static_cast<int>(group_size) - 1;

            #ifdef EPI_DEBUG
            auto & a = this->population.at(infected.at(entity_indices[g] + which));
            #else
            auto & a = this->get_agent(infected[entity_indices[g] + which]);
            #endif

            #ifdef EPI_DEBUG
            if (a.get_state() != ModelSEIRMixing<TSeq>::INFECTED)
                throw std::logic_error(
                    "The agent is not infected, but it should be."
                );
            #endif

            // Can't sample itself
            if (a.get_id() == agent->get_id())
                continue;

            sampled_agents[samp_id++] = a.get_id();
            
        }

    }
    
    return samp_id;

}

template<typename TSeq>
inline ModelSEIRMixing<TSeq> & ModelSEIRMixing<TSeq>::run(
    epiworld_fast_uint ndays,
    int seed
)
{

    Model<TSeq>::run(ndays, seed);

    return *this;

}

template<typename TSeq>
inline void ModelSEIRMixing<TSeq>::reset()
{

    Model<TSeq>::reset();   

    // Checking contact matrix's rows add to one
    size_t nentities = this->entities.size();
    if (this->contact_matrix.size() !=  nentities*nentities)
        throw std::length_error(
            std::string("The contact matrix must be a square matrix of size ") +
            std::string("nentities x nentities. ") +
            std::to_string(this->contact_matrix.size()) +
            std::string(" != ") + std::to_string(nentities*nentities) +
            std::string(".")
            );

    for (size_t i = 0u; i < this->entities.size(); ++i)
    {
        double sum = 0.0;
        for (size_t j = 0u; j < this->entities.size(); ++j)
        {
            if (this->contact_matrix[MM(i, j, nentities)] < 0.0)
                throw std::range_error(
                    std::string("The contact matrix must be non-negative. ") +
                    std::to_string(this->contact_matrix[MM(i, j, nentities)]) +
                    std::string(" < 0.")
                    );
            sum += this->contact_matrix[MM(i, j, nentities)];
        }
        if (sum < 0.999 || sum > 1.001)
            throw std::range_error(
                std::string("The contact matrix must have rows that add to one. ") +
                std::to_string(sum) +
                std::string(" != 1.")
                );
    }

    // Do it the first time only
    sampled_agents.resize(Model<TSeq>::size());

    // We only do it once
    n_infected_per_group.resize(this->entities.size(), 0u);
    std::fill(n_infected_per_group.begin(), n_infected_per_group.end(), 0u);

    // We are assuming one agent per entity
    infected.resize(Model<TSeq>::size());
    std::fill(infected.begin(), infected.end(), 0u);

    // This will say when do the groups start in the `infected` vector
    entity_indices.resize(this->entities.size(), 0u);
    std::fill(entity_indices.begin(), entity_indices.end(), 0u);
    for (size_t i = 1u; i < this->entities.size(); ++i)
    {

        entity_indices[i] +=
            this->entities[i - 1].size() +
            entity_indices[i - 1]
            ;

    }
    
    // Adjusting contact rate
    adjusted_contact_rate.clear();
    adjusted_contact_rate.resize(this->entities.size(), 0.0);

    for (size_t i = 0u; i < this->entities.size(); ++i)
    {
                
        adjusted_contact_rate[i] = 
            Model<TSeq>::get_param("Contact rate") /
                static_cast< epiworld_double > (this->get_entity(i).size());


        // Possibly correcting for a small number of agents
        if (adjusted_contact_rate[i] > 1.0)
            adjusted_contact_rate[i] = 1.0;

    }

    this->update_infected_list();

    return;

}

template<typename TSeq>
inline Model<TSeq> * ModelSEIRMixing<TSeq>::clone_ptr()
{
    
    ModelSEIRMixing<TSeq> * ptr = new ModelSEIRMixing<TSeq>(
        *dynamic_cast<const ModelSEIRMixing<TSeq>*>(this)
        );

    #if __cplusplus >= 202302L
        // C++23 or later
        [[assume(ptr != nullptr)]]
    #else
        // C++17 or C++20
        assert(ptr != nullptr); // Use assert for runtime checks
    #endif

    return dynamic_cast< Model<TSeq> *>(ptr);

}


/**
 * @brief Template for a Susceptible-Exposed-Infected-Removed (SEIR) model
 * 
 * @param model A Model<TSeq> object where to set up the SIR.
 * @param vname std::string Name of the virus
 * @param prevalence Initial prevalence (proportion)
 * @param contact_rate Average number of contacts (interactions) per step.
 * @param transmission_rate Probability of transmission
 * @param recovery_rate Probability of recovery
 */
template<typename TSeq>
inline ModelSEIRMixing<TSeq>::ModelSEIRMixing(
    ModelSEIRMixing<TSeq> & model,
    const std::string & vname,
    epiworld_fast_uint n,
    epiworld_double prevalence,
    epiworld_double contact_rate,
    epiworld_double transmission_rate,
    epiworld_double avg_incubation_days,
    epiworld_double recovery_rate,
    std::vector< double > contact_matrix
    )
{

    // Setting up the contact matrix
    this->contact_matrix = contact_matrix;

    epiworld::UpdateFun<TSeq> update_susceptible = [](
        epiworld::Agent<TSeq> * p, epiworld::Model<TSeq> * m
        ) -> void
        {

            if (p->get_n_entities() == 0)
                return;

            // Downcasting to retrieve the sampler attached to the
            // class
            GET_MODEL(m, m_down);
            
            size_t ndraws = m_down->sample_agents(p, m_down->sampled_agents);

            #ifdef EPI_DEBUG
            m_down->sampled_sizes.push_back(static_cast<int>(ndraws));
            #endif

            if (ndraws == 0u)
                return;
            
            // Drawing from the set
            int nviruses_tmp = 0;
            for (size_t n = 0u; n < ndraws; ++n)
            {

                auto & neighbor = m->get_agent(m_down->sampled_agents[n]);

                auto & v = neighbor.get_virus();

                #ifdef EPI_DEBUG
                if (nviruses_tmp >= static_cast<int>(m->array_virus_tmp.size()))
                    throw std::logic_error(
                        "Trying to add an extra element to a temporal array outside of the range."
                    );
                #endif
                    
                /* And it is a function of susceptibility_reduction as well */ 
                m->array_double_tmp[nviruses_tmp] =
                    (1.0 - p->get_susceptibility_reduction(v, m)) * 
                    v->get_prob_infecting(m) * 
                    (1.0 - neighbor.get_transmission_reduction(v, m)) 
                    ; 
            
                m->array_virus_tmp[nviruses_tmp++] = &(*v);

            }

            // Running the roulette
            int which = roulette(nviruses_tmp, m);

            if (which < 0)
                return;

            p->set_virus(
                *m->array_virus_tmp[which],
                m,
                ModelSEIRMixing<TSeq>::EXPOSED
                );

            return; 

        };

    epiworld::UpdateFun<TSeq> update_exposed_and_infected = [](
        epiworld::Agent<TSeq> * p, epiworld::Model<TSeq> * m
        ) -> void {

            auto state = p->get_state();

            if (state == ModelSEIRMixing<TSeq>::EXPOSED)
            {

                // Getting the virus
                auto & v = p->get_virus();

                // Does the agent become infected?
                if (m->runif() < 1.0/(v->get_incubation(m)))
                {

                    p->change_state(m, ModelSEIRMixing<TSeq>::INFECTED);
                    return;

                }


            } else if (state == ModelSEIRMixing<TSeq>::INFECTED)
            {


                // Odd: Die, Even: Recover
                epiworld_fast_uint n_events = 0u;
                const auto & v = p->get_virus();

                // Recover
                m->array_double_tmp[n_events++] = 
                    1.0 - (1.0 - v->get_prob_recovery(m)) * (1.0 - p->get_recovery_enhancer(v, m)); 

                #ifdef EPI_DEBUG
                if (n_events == 0u)
                {
                    printf_epiworld(
                        "[epi-debug] agent %i has 0 possible events!!\n",
                        static_cast<int>(p->get_id())
                        );
                    throw std::logic_error("Zero events in exposed.");
                }
                #else
                if (n_events == 0u)
                    return;
                #endif
                

                // Running the roulette
                int which = roulette(n_events, m);

                if (which < 0)
                    return;

                // Which roulette happen?
                p->rm_virus(m);

                return ;

            } else
                throw std::logic_error("This function can only be applied to exposed or infected individuals. (SEIR)") ;

            return;

        };

    // Setting up parameters
    model.add_param(contact_rate, "Contact rate");
    model.add_param(transmission_rate, "Prob. Transmission");
    model.add_param(recovery_rate, "Prob. Recovery");
    model.add_param(avg_incubation_days, "Avg. Incubation days");
    
    // state
    model.add_state("Susceptible", update_susceptible);
    model.add_state("Exposed", update_exposed_and_infected);
    model.add_state("Infected", update_exposed_and_infected);
    model.add_state("Recovered");

    // Global function
    epiworld::GlobalFun<TSeq> update = [](epiworld::Model<TSeq> * m) -> void
    {

        GET_MODEL(m, m_down);
        
        m_down->update_infected_list();

        return;

    };

    model.add_globalevent(update, "Update infected individuals");


    // Preparing the virus -------------------------------------------
    epiworld::Virus<TSeq> virus(vname, prevalence, true);
    virus.set_state(
        ModelSEIRMixing<TSeq>::EXPOSED,
        ModelSEIRMixing<TSeq>::RECOVERED,
        ModelSEIRMixing<TSeq>::RECOVERED
        );

    virus.set_prob_infecting(&model("Prob. Transmission"));
    virus.set_prob_recovery(&model("Prob. Recovery"));
    virus.set_incubation(&model("Avg. Incubation days"));

    model.add_virus(virus);

    model.queuing_off(); // No queuing need

    // Adding the empty population
    model.agents_empty_graph(n);

    model.set_name("Susceptible-Exposed-Infected-Removed (SEIR) with Mixing");

    return;

}

template<typename TSeq>
inline ModelSEIRMixing<TSeq>::ModelSEIRMixing(
    const std::string & vname,
    epiworld_fast_uint n,
    epiworld_double prevalence,
    epiworld_double contact_rate,
    epiworld_double transmission_rate,
    epiworld_double avg_incubation_days,
    epiworld_double recovery_rate,
    std::vector< double > contact_matrix
    )
{   

    this->contact_matrix = contact_matrix;

    ModelSEIRMixing(
        *this,
        vname,
        n,
        prevalence,
        contact_rate,
        transmission_rate,
        avg_incubation_days,
        recovery_rate,
        contact_matrix
    );

    return;

}

template<typename TSeq>
inline ModelSEIRMixing<TSeq> & ModelSEIRMixing<TSeq>::initial_states(
    std::vector< double > proportions_,
    std::vector< int > /* queue_ */
)
{

    Model<TSeq>::initial_states_fun =
        create_init_function_seir<TSeq>(proportions_)
        ;

    return *this;

}
#undef MM
#undef GET_MODEL
#endif
