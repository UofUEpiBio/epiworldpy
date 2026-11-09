# Building blocks

The classes models are made of. Use them to add viruses, interventions, or
policies (such as social bubbles) to a model, or to build a custom model from an empty
{class}`~epiworldpy.Model`: a {class}`~epiworldpy.Virus` carries the stages of
the disease and the transitions between them, with their values
({class}`~epiworldpy.Value`) and durations ({class}`~epiworldpy.Duration`).

```{eval-rst}
.. currentmodule:: epiworldpy

.. autosummary::
   :toctree: generated
   :template: class.rst
   :nosignatures:

   Model
   Agent
   Virus
   Tool
   Value
   Duration
   Bubbles
```

## Placement

Who gets a virus, a tool, an initial stage, or a group, at the start of each
run.

```{eval-rst}
.. currentmodule:: epiworldpy.assign

.. autosummary::
   :toctree: generated
   :template: function.rst
   :nosignatures:

   share
   count
   shares
   counts
   group_shares
   group_counts
   agents
   labels
   by
```
