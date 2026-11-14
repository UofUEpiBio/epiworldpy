"""Extending epiworldpy in C++.

C++ compiled against epiworldpy reaches a model built in Python with
``epiworldpy::as_model()`` (in ``<epiworldpy/extension.hpp>``) and can then use
the whole epiworld C++ API on it: global events, tools, viruses with their
natural histories, layers, parameters. :func:`cpp_source` compiles such code
in a session; a package adds :func:`get_include` to its include directories.
"""

from __future__ import annotations

import hashlib
import importlib.util
import os
import sys
import tempfile
from pathlib import Path
from types import ModuleType


def get_include() -> str:
    """Return the directory with epiworldpy's C++ headers.

    A package that builds C++ extensions adds it to its include directories
    (with pybind11's) and includes ``<epiworldpy/extension.hpp>``.

    Returns
    -------
    str
        The path of the directory.
    """
    return str(Path(__file__).parent / "include")


def cpp_source(
    code: str | None = None,
    file: str | Path | None = None,
    verbose: bool = False,
) -> ModuleType:
    """Compile C++ that works on epiworldpy's models, and import it.

    The code is compiled as a pybind11 module, as C++17, with epiworldpy's
    headers. It includes ``<epiworldpy/extension.hpp>`` and defines the module
    with ``PYBIND11_MODULE(EPIWORLDPY_EXTENSION, m) { ... }``: the name
    ``EPIWORLDPY_EXTENSION`` is set here. ``epiworldpy::as_model(model)``
    returns the ``epiworld::Model &`` behind a Python model; what the code adds
    to it belongs to the model, as if added from Python. ``as_model()`` raises
    an error if the code was built against another major version of epiworld
    than the installed epiworldpy's.

    Compiling takes a while (the epiworld headers are large). The same code is
    compiled once per session. It needs setuptools
    (``pip install epiworldpy[ext]``) and a C++17 compiler.

    Parameters
    ----------
    code : str, optional
        The C++ code. Give either ``code`` or ``file``.
    file : str or pathlib.Path, optional
        A file with the C++ code.
    verbose : bool, default False
        Whether to show the compiler's output.

    Returns
    -------
    module
        The compiled module, with the functions the code defines.
    """
    if (code is None) == (file is None):
        raise ValueError("Give either `code` or `file`.")
    if file is not None:
        code = Path(file).read_text()

    try:
        from setuptools import Distribution
        from pybind11.setup_helpers import Pybind11Extension, build_ext
    except ImportError as e:
        raise ImportError(
            "cpp_source() needs setuptools: pip install epiworldpy[ext]"
        ) from e

    name = "epiworldpy_ext_" + hashlib.sha1(code.encode()).hexdigest()[:16]
    if name in sys.modules:
        return sys.modules[name]

    build = Path(tempfile.mkdtemp(prefix=name + "_"))
    source = build / (name + ".cpp")
    source.write_text(f"#define EPIWORLDPY_EXTENSION {name}\n" + code)

    ext = Pybind11Extension(
        name, [str(source)], include_dirs=[get_include()], cxx_std=17
    )
    cmd = build_ext(Distribution({"name": name, "ext_modules": [ext]}))
    cmd.build_lib = str(build)
    cmd.build_temp = str(build / "tmp")
    cmd.ensure_finalized()

    if verbose:
        cmd.run()
    else:
        _quietly(cmd.run)

    spec = importlib.util.spec_from_file_location(name, cmd.get_ext_fullpath(name))
    module = importlib.util.module_from_spec(spec)
    spec.loader.exec_module(module)
    sys.modules[name] = module
    return module


def _quietly(run) -> None:
    """Run ``run()`` with the compiler's output captured, shown on failure."""
    sys.stdout.flush()
    sys.stderr.flush()
    with tempfile.TemporaryFile(mode="w+") as log:
        saved = os.dup(1), os.dup(2)
        os.dup2(log.fileno(), 1)
        os.dup2(log.fileno(), 2)
        ok = False
        try:
            run()
            ok = True
        finally:
            sys.stdout.flush()
            sys.stderr.flush()
            for fd, old in zip((1, 2), saved):
                os.dup2(old, fd)
                os.close(old)
            if not ok:
                log.seek(0)
                sys.stderr.write(log.read())
