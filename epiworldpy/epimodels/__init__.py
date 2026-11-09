"""Ready-to-use epidemiological models, from the model registry.

Every built-in model whose arguments are numbers is built by
:func:`make_model`; :func:`list_models` lists them and :func:`get_model_info`
describes their arguments. The named constructors (:func:`ModelSIR`,
:func:`ModelSEIR`, ...) call it and return a :class:`~epiworldpy.Model`, which
only needs contacts (a layer, see :mod:`epiworldpy.layer`) and a call to
:meth:`~epiworldpy.Model.run`.
"""

from .. import _core
from .._defunct import DEFUNCT_MODELS, model_stub

make_model = _core.epimodels.make_model
list_models = _core.epimodels.list_models
get_model_info = _core.epimodels.get_model_info

ModelDiffNet = _core.epimodels.ModelDiffNet
ModelSEIR = _core.epimodels.ModelSEIR
ModelSEIRD = _core.epimodels.ModelSEIRD
ModelSEIRQuarantine = _core.epimodels.ModelSEIRQuarantine
ModelSIR = _core.epimodels.ModelSIR
ModelSIRD = _core.epimodels.ModelSIRD
ModelSIS = _core.epimodels.ModelSIS
ModelSISD = _core.epimodels.ModelSISD
ModelSURV = _core.epimodels.ModelSURV

# Removed in 1.0.0: calling them raises with what replaces them
globals().update({name: model_stub(name) for name in DEFUNCT_MODELS})

__all__ = [
    "ModelDiffNet",
    "ModelSEIR",
    "ModelSEIRD",
    "ModelSEIRQuarantine",
    "ModelSIR",
    "ModelSIRD",
    "ModelSIS",
    "ModelSISD",
    "ModelSURV",
    "get_model_info",
    "list_models",
    "make_model",
]
