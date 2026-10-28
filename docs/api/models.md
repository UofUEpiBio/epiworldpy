# Models

Ready-to-use epidemiological models from {mod}`epiworldpy.epimodels`. Each one
is a {class}`~epiworldpy.Model` with its states, parameters, and virus already
set up; its contacts come from its layers (see {mod}`epiworldpy.layer`). The
models that differed only in their contacts (`ModelSIRCONN`, `ModelSIRMixing`,
`ModelSEIRMixingQuarantine`, ...) were removed in 1.0.0: calling one raises
with what replaces it.

```{eval-rst}
.. currentmodule:: epiworldpy.epimodels
```

## SIR family

```{eval-rst}
.. autosummary::
   :toctree: generated
   :template: class.rst
   :nosignatures:

   ModelSIR
   ModelSIRD
```

## SEIR family

```{eval-rst}
.. autosummary::
   :toctree: generated
   :template: class.rst
   :nosignatures:

   ModelSEIR
   ModelSEIRD
   ModelSEIRQuarantine
```

## SIS family

```{eval-rst}
.. autosummary::
   :toctree: generated
   :template: class.rst
   :nosignatures:

   ModelSIS
   ModelSISD
```

## Other models

```{eval-rst}
.. autosummary::
   :toctree: generated
   :template: class.rst
   :nosignatures:

   ModelSURV
   ModelDiffNet
```
