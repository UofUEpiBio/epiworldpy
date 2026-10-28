"""Ready-to-use epidemiological models.

Each class sets up its states, parameters, and virus, so it only needs its
contacts (a layer, see :mod:`epiworldpy.layer`) and a call to
:meth:`~epiworldpy.Model.run`. All of them are :class:`~epiworldpy.Model`
subclasses.
"""

from .. import _core
from .._defunct import DEFUNCT_MODELS, model_stub

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
]
