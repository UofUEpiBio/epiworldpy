# Models

Ready-to-use epidemiological models from {mod}`epiworldpy.epimodels`. Each one
returns a {class}`~epiworldpy.Model` with its parameters and virus already set
up; its contacts come from its layers (see {mod}`epiworldpy.layer`). Every
model whose arguments are numbers is built by the registry ({func}`make_model`),
and the named constructors call it. The models that differed only in their
contacts (`ModelSIRCONN`, `ModelSIRMixing`, `ModelSEIRMixingQuarantine`, ...)
were removed in 1.0.0: calling one raises with what replaces it.

```{eval-rst}
.. currentmodule:: epiworldpy.epimodels
```

## The registry

```{eval-rst}
.. autosummary::
   :toctree: generated
   :template: function.rst
   :nosignatures:

   make_model
   list_models
   get_model_info
```

## SIR family

```{eval-rst}
.. autosummary::
   :toctree: generated
   :template: function.rst
   :nosignatures:

   ModelSIR
   ModelSIRD
```

## SEIR family

```{eval-rst}
.. autosummary::
   :toctree: generated
   :template: function.rst
   :nosignatures:

   ModelSEIR
   ModelSEIRD
   ModelSEIRQuarantine
```

## SIS family

```{eval-rst}
.. autosummary::
   :toctree: generated
   :template: function.rst
   :nosignatures:

   ModelSIS
   ModelSISD
```

## Other models

```{eval-rst}
.. autosummary::
   :toctree: generated
   :template: function.rst
   :nosignatures:

   ModelSURV

.. autosummary::
   :toctree: generated
   :template: class.rst
   :nosignatures:

   ModelDiffNet
```
