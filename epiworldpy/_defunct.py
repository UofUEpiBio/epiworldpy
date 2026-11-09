"""Functions removed in epiworldpy 1.0.0.

Each one is kept as a stub that raises with what replaces it. The table below
is the single list of them (the migration notes in the README follow it); the
stubs stay through 1.x.
"""

_NETWORK = "the network layer's method: model.get_layer(name).{}(...)"

DEFUNCT = {
    ("Model", "agents_smallworld"): (
        "Give the population to the constructor (e.g., ModelSIR(name, n, ...)) "
        "and add a network layer: model.add_layer(layer.network(name, "
        "graph.smallworld(k, p)))."
    ),
    ("Model", "agents_from_edgelist"): (
        "Give the population to the constructor and add a network layer: "
        "model.add_layer(layer.network(name, graph.edgelist(source, target, "
        "size, directed)))."
    ),
    ("Model", "agents_sbm"): (
        "Give the population to the constructor and add a network layer: "
        "model.add_layer(layer.network(name, graph.sbm(block_sizes, "
        "mixing_matrix)))."
    ),
    ("Model", "agents_bernoulli"): (
        "Give the population to the constructor and add a network layer: "
        "model.add_layer(layer.network(name, graph.bernoulli(p)))."
    ),
    ("Model", "agents_empty_graph"): (
        "Give the population to the constructor (e.g., Model(n)); a model "
        "without layers has no contacts."
    ),
    ("Model", "set_rewire_prop"): (
        "Rewiring belongs to a network layer: "
        "model.get_layer(name).set_rewiring(graph.rewire_degseq(param)), with "
        "the proportion in the model parameter `param`."
    ),
    ("Model", "get_rewire_prop"): (
        "The proportion rewired is the model parameter given to "
        "graph.rewire_degseq(param): use model.get_param(param)."
    ),
    ("Model", "rewire"): (
        "A network layer with a rewiring function (set_rewiring()) rewires "
        "itself at the end of every step."
    ),
    ("Model", "write_edgelist"): (
        "Read the ties of a network layer with "
        "model.get_layer(name).get_edgelist()."
    ),
    ("Model", "is_directed"): "Use " + _NETWORK.format("is_directed") + ".",
    ("Model", "add_edge"): "Use " + _NETWORK.format("add_edge") + ".",
    ("Model", "rm_edge"): "Use " + _NETWORK.format("rm_edge") + ".",
    ("Model", "has_edge"): "Use " + _NETWORK.format("has_edge") + ".",
    ("Agent", "get_n_neighbors"): (
        "Use model.get_layer(name).get_degree(agent.get_id())."
    ),
    ("Agent", "has_neighbor"): "Use model.get_layer(name).has_edge(i, j).",
    ("Agent", "get_neighbors"): "Use model.get_layer(name).get_edgelist().",
}

# Entities (removed in epiworld's phase 3d-3a) became the groups of a mixing
# layer.
_ENTITY = (
    "Entities are gone: groups live in a mixing layer. "
    "model.add_layer(layer.mixing(name, groups, ...)), with groups a list "
    "(one group per agent) or a placement (e.g., assign.counts(sizes)); the "
    "layer's members_of(group) and get_group(agent) read them."
)

DEFUNCT.update(
    {
        (cls_name, name): _ENTITY
        for cls_name, names in {
            "Model": ["add_entity", "get_entity", "get_n_entities"],
            "Agent": [
                "add_entity",
                "rm_entity",
                "has_entity",
                "get_entities",
                "get_n_entities",
            ],
        }.items()
        for name in names
    }
)


# The natural history (epiworld NH-1, NH-2) replaced the states and their
# update functions; viruses carry their stages and transitions.
_STATE = (
    "States are gone: the virus carries the stages of the disease and the "
    "transitions between them (Virus.add_stage(), add_infection(), add_rate(), "
    "add_after(); see help(epiworldpy.Virus))."
)

# Placement: one vocabulary, epiworldpy.assign
_ASSIGN_VIRUS = (
    "Place the virus when adding it: model.add_virus(virus, assign), with "
    "assign.share(p), assign.count(n), assign.agents(ids) or "
    "assign.group_shares(layer, p) (see help(epiworldpy.assign))."
)
_ASSIGN_TOOL = (
    "Place the tool when adding it: model.add_tool(tool, assign), with "
    "assign.share(p), assign.count(n), assign.agents(ids) or "
    "assign.group_shares(layer, p) (see help(epiworldpy.assign))."
)


def _virus_value(what: str) -> str:
    return (
        "A virus holds no values: its transitions name model parameters. "
        f"Declare the {what} as a transition (Virus.add_infection(), "
        "add_rate() or add_after(), with a number, a parameter name, or a "
        "Value); a strain reads other parameters with Virus.use_param()."
    )


_TOOL_STATE = (
    "Tools do not move agents: they change the probabilities of infection and "
    "of the transitions they act on (Virus.add_rate(..., effect=\"recovery\" "
    "or \"death\"))."
)

DEFUNCT.update(
    {
        ("Model", "add_state"): _STATE,
        ("Model", "set_state_function"): _STATE,
        ("Model", "get_states"): "Use model.get_stages() and model.get_statuses().",
        ("Model", "get_n_states"): "Use model.get_stages() and model.get_statuses().",
        ("Model", "state_of"): "Use model.get_stages() and model.get_statuses().",
        ("Model", "print_state_codes"): (
            "Use model.get_stages() and model.get_statuses()."
        ),
        ("Model", "initial_states"): (
            "Use model.set_initial(assign.shares({<stage>: <share>, ...})): the "
            "agents start in stages without the virus."
        ),
        ("Model", "set_infectious_states"): (
            "A stage transmits if it is declared infectious: "
            "virus.add_stage(label, infectious=True)."
        ),
        ("Model", "get_infectious_states"): (
            "Use virus.get_stages() (its \"infectious\" entry)."
        ),
        ("Model", "get_agents_states"): (
            "Use model.get_agents_stages() and model.get_agents_statuses()."
        ),
        ("Model", "get_agents_in_state"): (
            "Use model.get_agents_stages() and model.get_agents_statuses()."
        ),
        ("Agent", "get_state"): (
            "Use model.get_stage(agent) and model.get_status(agent)."
        ),
        ("Agent", "get_state_prev"): (
            "Use model.get_stage(agent); DataBase.get_hist_transitions() counts "
            "the moves."
        ),
        ("Agent", "get_state_last_changed"): (
            "Use model.get_stage(agent); DataBase.get_hist_transitions() counts "
            "the moves."
        ),
        ("Agent", "change_state"): (
            "An agent moves only along a declared transition: "
            "model.fire(transition, agent_id) (e.g., an external one, "
            "Virus.add_external())."
        ),
        ("Agent", "rm_virus"): (
            "Removing a virus needs the stage to move to: declare an external "
            "transition (Virus.add_external()) and fire it with "
            "model.fire(transition, agent_id)."
        ),
        ("Virus", "set_state"): _STATE,
        ("Virus", "set_queue"): _STATE,
        ("Virus", "get_state"): _STATE,
        ("Virus", "get_queue"): _STATE,
        ("Virus", "set_distribution"): _ASSIGN_VIRUS,
        ("Virus", "distribute"): _ASSIGN_VIRUS,
        ("Virus", "set_sequence"): (
            "Variants are recorded by id and parent: genetic sequences are gone "
            "(see Virus.get_lineage_id())."
        ),
        ("Virus", "mutate"): (
            "Use agent.mutate_virus(model), which calls the function from "
            "Virus.set_mutation()."
        ),
        ("Tool", "set_state"): _TOOL_STATE,
        ("Tool", "set_queue"): _TOOL_STATE,
        ("Tool", "get_state"): _TOOL_STATE,
        ("Tool", "get_queue"): _TOOL_STATE,
        ("Tool", "set_distribution"): _ASSIGN_TOOL,
        ("Tool", "distribute"): _ASSIGN_TOOL,
    }
)

DEFUNCT.update(
    {
        ("Virus", f"{verb}_{what}{suffix}"): _virus_value(label)
        for what, label in {
            "prob_infecting": "probability of infection",
            "prob_recovery": "recovery",
            "prob_death": "death",
            "incubation": "incubation",
        }.items()
        for verb, suffix in (("set", ""), ("set", "_fun"), ("get", ""))
    }
)
DEFUNCT.update(
    {
        ("Virus", name): _virus_value("immunity after recovery")
        for name in ("set_post_immunity", "set_post_recovery", "post_recovery")
    }
)

# Classes removed in 1.0.0 (the update functions of the states)
DEFUNCT_CLASSES = {"UpdateFun": _STATE, "NativeUpdateFun": _STATE}

_GROUPING = (
    "Give layer.mixing() a placement whose labels are the groups: "
    "assign.counts(sizes) or assign.shares(proportions) (see "
    "help(epiworldpy.assign))."
)


_RANDOM_MIXING = (
    "with a random-mixing layer: everyone in one group, a contact matrix of 1, "
    "and set_contact_rate(\"Contact rate\") (model.add_param(rate, "
    "\"Contact rate\"); see help(epiworldpy.layer.Mixing))."
)
_MATRIX = (
    "and model.add_layer(layer.mixing(name, groups, "
    "contact_matrix=contact_matrix))."
)

# The models removed in 1.0.0: a model's states describe the course of
# infection, and its layers describe who meets whom.
DEFUNCT_MODELS = {
    "ModelSIRCONN": "Use ModelSIR() " + _RANDOM_MIXING,
    "ModelSEIRCONN": "Use ModelSEIR() " + _RANDOM_MIXING,
    "ModelSIRDCONN": "Use ModelSIRD() " + _RANDOM_MIXING,
    "ModelSEIRDCONN": "Use ModelSEIRD() " + _RANDOM_MIXING,
    "ModelSIRMixing": "Use ModelSIR() " + _MATRIX,
    "ModelSEIRMixing": "Use ModelSEIR() " + _MATRIX,
    "ModelSEIRMixingQuarantine": (
        "Use ModelSEIRQuarantine() " + _MATRIX + " It traces every layer."
    ),
    "ModelSEIRNetworkQuarantine": (
        "Use ModelSEIRQuarantine() and add the network layer; it traces every "
        "layer."
    ),
}


def model_stub(name: str):
    """A stand-in for a removed model that raises with its replacement."""
    return _function_stub(f"epimodels.{name}", name, DEFUNCT_MODELS[name])


def grouping_stub(name: str):
    """A stand-in for a removed function of ``epiworldpy.grouping``."""
    return _function_stub(f"grouping.{name}", name, _GROUPING)


def class_stub(name: str):
    """A stand-in for a removed class: using it raises with its replacement."""
    msg = f"{name} is defunct as of epiworldpy 1.0.0. {DEFUNCT_CLASSES[name]}"

    class _Meta(type):
        def __getattr__(cls, attr):
            # Introspection (hasattr(), inspect) asks for dunder names
            if attr.startswith("__"):
                raise AttributeError(attr)
            raise RuntimeError(msg)

        def __call__(cls, *args, **kwargs):
            raise RuntimeError(msg)

    return _Meta(name, (), {"__doc__": msg, "__module__": "epiworldpy"})


def entity_stub():
    """A stand-in for the removed ``Entity`` class."""
    return _function_stub("Entity", "Entity", _ENTITY)


def _function_stub(label: str, name: str, msg: str):
    def defunct(*args, **kwargs):
        raise RuntimeError(f"{label}() is defunct as of epiworldpy 1.0.0. {msg}")

    defunct.__name__ = name
    defunct.__qualname__ = name
    defunct.__doc__ = f"Defunct as of epiworldpy 1.0.0. {msg}"
    return defunct


def _stub(cls_name: str, name: str, msg: str):
    def defunct(self, *args, **kwargs):
        raise RuntimeError(
            f"{cls_name}.{name}() is defunct as of epiworldpy 1.0.0. {msg}"
        )

    defunct.__name__ = name
    defunct.__qualname__ = f"{cls_name}.{name}"
    defunct.__doc__ = f"Defunct as of epiworldpy 1.0.0. {msg}"
    return defunct


def install(classes: dict) -> None:
    """Attach the stubs to the classes they were methods of.

    Parameters
    ----------
    classes : dict
        The classes, by name (e.g., ``{"Model": Model}``).
    """
    for (cls_name, name), msg in DEFUNCT.items():
        setattr(classes[cls_name], name, _stub(cls_name, name, msg))
