import re
from importlib.metadata import version
from pathlib import Path

import epiworldpy


def test_version():
    pyproject = (Path(__file__).parents[1] / "pyproject.toml").read_text()
    expected = re.search(r'^version = "(.+)"', pyproject, re.M).group(1)
    assert version("epiworldpy") == expected


def test_epiworld_version():
    assert re.fullmatch(r"\d+\.\d+\.\d+.*", epiworldpy.__epiworld_version__)
