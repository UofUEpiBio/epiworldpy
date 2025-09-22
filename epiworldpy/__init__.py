# |##########################################|
# |       Base C++ Extension Publication     |
# |##########################################|

from . import epimodels

__all__ = ["epimodels"]

# |##########################################|
# |     Platform-independent saver module    |
# |##########################################|


import csv
import json
from pathlib import Path
from typing import Protocol, Any


class SaverBackend(Protocol):
    def save(self, results: dict[str, Any], fn: str | None = None) -> None: ...


class SaverCsvBackend:
    def save(self, results: dict[str, Any], fn: str | None = None) -> None:
        if fn is None:
            raise ValueError("Filename required for CSV backend.")
        base = Path(fn)
        for key, value in results.items():
            outfile = base.with_name(f"{base.stem}-{key}.csv")
            with open(outfile, "w", newline="") as f:
                writer = csv.writer(f)
                if isinstance(value, dict):
                    writer.writerow(value.keys())
                    rows = zip(*value.values())
                    writer.writerows(rows)
                else:
                    writer.writerows(value)


class SaverJsonBackend:
    def save(self, results: dict[str, Any], fn: str | None = None) -> None:
        if fn is None:
            raise ValueError("Filename required for JSON backend.")
        base = Path(fn)
        for key, value in results.items():
            outfile = base.with_name(f"{base.stem}-{key}.json")
            with open(outfile, "w") as f:
                json.dump(value, f)


try:
    import h5py

    class HDF5Backend(SaverBackend):
        def save(self, results, fn=None):
            if fn is None:
                raise ValueError("Filename required for HDF5 backend.")
            base = Path(fn).with_suffix(".h5")

            with h5py.File(base, "w") as f:
                for key, value in results.items():
                    grp = f.create_group(key)

                    if isinstance(value, dict):
                        for subkey, arr in value.items():
                            grp.create_dataset(subkey, data=arr)
                    else:
                        grp.create_dataset("data", data=value)

except ImportError:
    HDF5Backend = None  # type: ignore[assignment]

try:
    import zarr

    class ZarrBackend(SaverBackend):
        def save(self, results, fn=None):
            if fn is None:
                raise ValueError("Filename required for Zarr backend.")
            base = Path(fn).with_suffix(".zarr")

            root = zarr.open_group(base, mode="w")

            for key, value in results.items():
                grp = root.create_group(key)

                if isinstance(value, dict):
                    for subkey, arr in value.items():
                        grp.create_dataset(subkey, data=arr)
                else:
                    grp.create_dataset("data", data=value)

except ImportError:
    ZarrBackend = None  # type: ignore[assignment]


class Saver:
    VALID_WHATS = {
        "total_hist",
        "virus_hist",
        "tool_hist",
        "transition",
        "transmission",
        "reproductive",
        "generation",
    }

    def __init__(self, *what, backend=SaverCsvBackend(), fn=None):
        for w in what:
            if w not in self.VALID_WHATS:
                raise ValueError(
                    f"What '{w}' is not supported. "
                    f"Must be one of {sorted(self.VALID_WHATS)}"
                )

        self.what = list(what)
        self.backend = backend
        self.fn = fn
        self._db = None

    def run_multiple(self, model, ndays, nsims, **kwargs):
        self._db = model.run_multiple(ndays, nsims, **kwargs)
        return model

    def get_results(self):
        if self._db is None:
            raise RuntimeError("No results available. Run `run_multiple` first.")

        results = {}
        for what in self.what:
            if what == "total_hist":
                results[what] = self._db.get_hist_total()
            elif what == "virus_hist":
                results[what] = self._db.get_hist_virus()
            elif what == "tool_hist":
                results[what] = self._db.get_hist_tool()
            elif what == "transition":
                results[what] = self._db.get_hist_transition_matrix(False)
            elif what == "transmission":
                results[what] = self._db.get_transmissions()
            elif what == "reproductive":
                results[what] = self._db.get_reproductive_number()
            elif what == "generation":
                results[what] = self._db.get_generation_time()
            else:
                raise ValueError(f"Unsupported what: {what}")

        return self.backend.save(results, self.fn)


__all__ += ["Saver", "SaverCsvBackend", "SaverJsonBackend"]

if HDF5Backend is not None:
    __all__.append("HDF5Backend")

if ZarrBackend is not None:
    __all__.append("ZarrBackend")
