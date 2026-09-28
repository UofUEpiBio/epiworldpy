# Results

A model's results are in its {class}`~epiworldpy.DataBase`, from
{meth}`Model.get_db() <epiworldpy.Model.get_db>`. The functions below collect
several results at once and save them to files.

```{eval-rst}
.. currentmodule:: epiworldpy

.. autosummary::
   :toctree: generated
   :template: class.rst
   :nosignatures:

   DataBase
```

## Saving results

```{eval-rst}
.. autosummary::
   :toctree: generated
   :template: function.rst
   :nosignatures:

   extract_database_results
   write_db_results_multiple_csv
   write_db_results_json
   write_db_results_multiple_json
   write_db_results_hdf5
   write_db_results_zarr
```

## JSON helpers

```{eval-rst}
.. autosummary::
   :toctree: generated
   :template: class.rst
   :nosignatures:

   NumpyJSONEncoder

.. autosummary::
   :toctree: generated
   :template: function.rst
   :nosignatures:

   numpy_json_decoder
```
