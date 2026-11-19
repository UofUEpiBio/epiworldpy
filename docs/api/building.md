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

## Contact layers

Network and mixing layers share participation controls on
{class}`~epiworldpy.layer.ContactLayer`. Use
{meth}`~epiworldpy.layer.ContactLayer.exclude_status` between runs to exclude
a status from one layer, or {meth}`~epiworldpy.layer.ContactLayer.close` and
{meth}`~epiworldpy.layer.ContactLayer.reopen` to stop and resume its contacts.
During a run, closures take effect from the next day and last for that run.
Use {meth}`~epiworldpy.Tool.add_layer_target` to restrict a tool's
susceptibility and transmission reductions to named layers; recovery and
death effects still apply everywhere.

```{eval-rst}
.. currentmodule:: epiworldpy.layer

.. autosummary::
   :toctree: generated
   :template: class.rst
   :nosignatures:

   ContactLayer
   Network
   Mixing
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

## C++ extensions

Compile C++ that works on a model built in Python (see
[Extending epiworldpy in C++](../extending.md)).

```{eval-rst}
.. currentmodule:: epiworldpy

.. autosummary::
   :toctree: generated
   :template: function.rst
   :nosignatures:

   cpp_source
   get_include
```
