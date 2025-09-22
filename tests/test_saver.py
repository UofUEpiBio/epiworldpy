# tests/test_saver.py
import json
import csv
import os
import pytest
from epiworldpy import (
    Saver,
    SaverCsvBackend,
    SaverJsonBackend,
    HDF5Backend,
    ZarrBackend,
)


@pytest.fixture
def sample_results():
    return {
        "total_hist": {"dates": [1, 2], "counts": [10, 20]},
        "virus_hist": {"dates": [1, 2], "ids": [0, 1]},
    }


def test_csv_backend_creates_files(tmp_path, sample_results):
    fn = tmp_path / "results.csv"
    backend = SaverCsvBackend()

    backend.save(sample_results, str(fn))

    for key in sample_results:
        out = tmp_path / f"results-{key}.csv"
        assert out.exists()
        with open(out) as f:
            rows = list(csv.reader(f))
        assert rows  # not empty


def test_csv_backend_requires_filename(sample_results):
    backend = SaverCsvBackend()
    with pytest.raises(ValueError):
        backend.save(sample_results, None)


def test_json_backend_creates_files(tmp_path, sample_results):
    fn = tmp_path / "results.json"
    backend = SaverJsonBackend()
    backend.save(sample_results, str(fn))

    for key in sample_results:
        out = tmp_path / f"results-{key}.json"
        assert out.exists()
        data = json.loads(out.read_text())
        assert isinstance(data, dict)


def test_json_backend_requires_filename(sample_results):
    backend = SaverJsonBackend()
    with pytest.raises(ValueError):
        backend.save(sample_results, None)


@pytest.mark.skipif(HDF5Backend is None, reason="h5py not installed")
def test_hdf5_backend_creates_file(tmp_path, sample_results):
    fn = tmp_path / "results.h5"
    backend = HDF5Backend()
    backend.save(sample_results, str(fn))
    assert fn.exists()


@pytest.mark.skipif(ZarrBackend is None, reason="zarr not installed")
def test_zarr_backend_creates_dir(tmp_path, sample_results):
    fn = tmp_path / "results.zarr"
    backend = ZarrBackend()
    backend.save(sample_results, str(fn))
    assert fn.exists()
    assert fn.is_dir()


def test_saver_valid_and_invalid_keys(tmp_path, sample_results, monkeypatch):
    # patch backend to avoid real file writes
    class DummyBackend:
        def __init__(self):
            self.saved = None
            self.fn = None

        def save(self, results, fn=None):
            self.saved = results
            self.fn = fn
            return "OK"

    dummy = DummyBackend()

    saver = Saver("total_hist", backend=dummy, fn=str(tmp_path / "out.csv"))

    # Emulate db
    class DummyDB:
        def get_hist_total(self):
            return {"x": [1]}

        def get_hist_virus(self):
            return {}

        def get_hist_tool(self):
            return {}

        def get_hist_transition_matrix(self, skip):
            return {}

        def get_transmissions(self):
            return {}

        def get_reproductive_number(self):
            return {}

        def get_generation_time(self):
            return {}

    saver._db = DummyDB()
    result = saver.get_results()
    assert result == "OK"
    assert "total_hist" in dummy.saved

    with pytest.raises(ValueError):
        Saver("not_a_key")


def test_saver_get_results_without_run():
    saver = Saver("total_hist", backend=SaverCsvBackend(), fn="foo.csv")
    with pytest.raises(RuntimeError):
        saver.get_results()
