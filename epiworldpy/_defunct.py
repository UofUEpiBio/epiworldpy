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
