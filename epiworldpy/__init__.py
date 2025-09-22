# |##########################################|
# |       Base C++ Extension Publication     |
# |##########################################|

from ._core import Virus  # type: ignore[import]
from . import epimodels

__all__ = ["Virus", "epimodels"]

# |##########################################|
# |     Platform-independent saver module    |
# |##########################################|

import os
import csv
import json
import numpy as np  # type: ignore[import]
from pathlib import Path
from typing import Any

DatabaseResults = dict[str, Any]


def write_db_results_multiple_csv(
    results: DatabaseResults, base: str = "results", directory: Path | str = Path(".")
) -> None:
    if isinstance(directory, str):
        directory = Path(directory)

    os.makedirs(directory, exist_ok=True)
    assert directory.exists() and directory.is_dir()

    for key, value in results.items():
        with open(directory / f"{base}-{key}.csv", "w", newline="") as f:
            writer = csv.writer(f)
            if isinstance(value, dict):
                writer.writerow(value.keys())
                rows = zip(*value.values())
                writer.writerows(rows)
            else:
                writer.writerows(value)


class NumpyJSONEncoder(json.JSONEncoder):
    def default(self, o):
        if isinstance(o, np.integer):
            return {"__numpy__": True, "dtype": str(o.dtype), "value": int(o)}
        elif isinstance(o, np.floating):
            return {"__numpy__": True, "dtype": str(o.dtype), "value": float(o)}
        elif isinstance(o, np.ndarray):
            return {
                "__numpy__": True,
                "dtype": str(o.dtype),
                "shape": o.shape,
                "value": o.tolist(),
            }

        return super().default(o)


def numpy_json_decoder(dct):
    if "__numpy__" in dct:
        dtype = np.dtype(dct["dtype"])
        if "shape" in dct:
            return np.array(dct["value"], dtype=dtype).reshape(dct["shape"])
        else:
            return np.array(dct["value"], dtype=dtype).item()

    return dct


def write_db_results_json(
    results: DatabaseResults, filename: Path = Path("results.json")
):
    if isinstance(filename, str):
        filename = Path(filename)

    with open(filename, "w") as f:
        json.dump(results, f, cls=NumpyJSONEncoder)


def write_db_results_multiple_json(
    results: DatabaseResults,
    base: str = "results",
    directory: Path = Path("."),
) -> None:
    if isinstance(directory, str):
        directory = Path(directory)

    os.makedirs(directory, exist_ok=True)
    assert directory.exists() and directory.is_dir()

    for key, value in results.items():
        with open(directory / f"{base}-{key}.json", "w") as f:
            json.dump(value, f, cls=NumpyJSONEncoder)


try:
    import h5py  # type: ignore[import]

    def write_db_results_hdf5(  # type: ignore
        results: DatabaseResults, filename: Path = Path("results.h5"), **hdf5_kwargs
    ):
        def write_group(grp: h5py.Group, value):
            if (
                isinstance(value, dict)
                and len(value) == 2
                and "values" in value
                and "indexes" in value
            ):
                arr = np.asarray(value["indexes"])
                if arr.dtype == object:
                    arr = arr.astype("U")
                dset = grp.create_dataset("data", data=arr)
                v = value["values"]
                if isinstance(v, np.ndarray):
                    v = v.tolist()
                dset.attrs["values"] = v

            elif isinstance(value, dict):
                for subkey, subval in value.items():
                    subgrp = grp.create_group(subkey)
                    write_group(subgrp, subval)

            else:
                arr = np.asarray(value)
                if arr.dtype == object:
                    arr = arr.astype("U")
                grp.create_dataset("data", data=arr)

        if isinstance(filename, str):
            filename = Path(filename)

        with h5py.File(filename, "w") as root:
            for key, value in results.items():
                subgrp = root.create_group(key)
                write_group(subgrp, value)

except ImportError:

    def write_db_results_hdf5(results, filename=None):
        raise ImportError(
            "The `h5py' package is not installed, cannot write HDF5 files."
        )


try:
    import zarr  # type: ignore[import]

    def write_db_results_zarr(  # type: ignore
        results: DatabaseResults, filename: Path = Path("results.zarr")
    ):
        def write_group(grp: zarr.Group, value):
            if (
                isinstance(value, dict)
                and len(value) == 2
                and "values" in value
                and "indexes" in value
            ):
                arr = np.asarray(value["indexes"])
                if arr.dtype == object:
                    arr = arr.astype("U")
                dset = grp.create_array("data", data=arr)
                v = value["values"]
                if isinstance(v, np.ndarray):
                    v = v.tolist()
                dset.attrs["values"] = v

            elif isinstance(value, dict):
                for subkey, subval in value.items():
                    subgrp = grp.create_group(subkey)
                    write_group(subgrp, subval)

            else:
                arr = np.asarray(value)
                if arr.dtype == object:
                    arr = arr.astype("U")
                grp.create_array("data", data=arr)

        if isinstance(filename, str):
            filename = Path(filename)

        root = zarr.open_group(filename, mode="w")
        for key, value in results.items():
            subgrp = root.create_group(key)
            write_group(subgrp, value)

except ImportError:

    def write_db_results_zarr(results, filename=None):
        raise ImportError(
            "The `zarr' package is not installed, cannot write Zarr files."
        )


def extract_database_results(db, *whats) -> DatabaseResults:
    VALID_WHATS = {
        "total_hist",
        "virus_hist",
        "tool_hist",
        "transition",
        "transmission",
        "reproductive",
        "generation",
    }

    if db is None:
        raise RuntimeError("No results available. Run `run_multiple` first.")

    whats = list(whats)
    for w in whats:
        if w not in VALID_WHATS:
            raise ValueError(
                f"What '{w}' is not supported. Must be one of {sorted(VALID_WHATS)}"
            )

    results = {}
    for what in whats:
        match what:
            case "total_hist":
                results[what] = db.get_hist_total()
            case "virus_hist":
                results[what] = db.get_hist_virus()
            case "tool_hist":
                results[what] = db.get_hist_tool()
            case "transition":
                results[what] = db.get_hist_transition_matrix(False)
            case "transmission":
                results[what] = db.get_transmissions()
            case "reproductive":
                results[what] = db.get_reproductive_number()
            case "generation":
                results[what] = db.get_generation_time()
            case _:
                assert False

    return results


__all__ += [
    "DatabaseResults",
    "extract_database_results",
    "write_db_results_multiple_csv",
    "write_db_results_json",
    "write_db_results_multiple_json",
]

if write_db_results_hdf5 is not None:
    __all__.append("write_db_results_hdf5")

if write_db_results_zarr is not None:
    __all__.append("write_db_results_zarr")
