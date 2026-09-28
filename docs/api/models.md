# Models

Ready-to-use epidemiological models from {mod}`epiworldpy.epimodels`. Each one
is a {class}`~epiworldpy.Model` with its states, parameters, and virus already
set up.

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
   ModelSIRCONN
   ModelSIRD
   ModelSIRDCONN
   ModelSIRMixing
```

## SEIR family

```{eval-rst}
.. autosummary::
   :toctree: generated
   :template: class.rst
   :nosignatures:

   ModelSEIR
   ModelSEIRCONN
   ModelSEIRD
   ModelSEIRDCONN
   ModelSEIRMixing
   ModelSEIRMixingQuarantine
   ModelSEIRNetworkQuarantine
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
