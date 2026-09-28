"""Check that the public API has complete NumPy-style docstrings.

Every public class, method, and function must have a one-line summary. Any
callable that takes arguments must document each of them in a ``Parameters``
section, and any callable that returns something must have a ``Returns``
section. pybind11 prepends the signature(s) to the docstrings of the compiled
bindings; each overload is checked separately.
"""

from __future__ import annotations

import enum
import inspect
import re

import pytest

import epiworldpy
from epiworldpy import epimodels


def _split_top_level(s: str) -> list[str]:
    """Split a pybind11 argument list on commas outside brackets."""
    parts, depth, cur = [], 0, ""
    for ch in s:
        if ch in "([{":
            depth += 1
        elif ch in ")]}":
            depth -= 1
        if ch == "," and depth == 0:
            parts.append(cur)
            cur = ""
        else:
            cur += ch
    if cur.strip():
        parts.append(cur)
    return [p.strip() for p in parts]


def _parse_signature(sig: str) -> tuple[list[str], str]:
    """Return the argument names (without ``self``) and the return type."""
    start = sig.index("(")
    depth = 0
    for end in range(start, len(sig)):
        if sig[end] == "(":
            depth += 1
        elif sig[end] == ")":
            depth -= 1
            if depth == 0:
                break
    args = [a.split(":")[0].split("=")[0].strip()
            for a in _split_top_level(sig[start + 1:end])]
    ret = sig[end + 1:].strip()
    ret = ret[2:].strip() if ret.startswith("->") else "None"
    return [a for a in args if a not in ("self", "")], ret


def _pybind_overloads(func) -> list[tuple[list[str], str, str]]:
    """Split a pybind11 docstring into (args, return type, body) per overload."""
    doc = func.__doc__ or ""
    name = func.__name__
    if "\nOverloaded function.\n" in doc:
        chunks = re.split(rf"\n\d+\. (?={re.escape(name)}\()", doc)[1:]
    else:
        chunks = [doc]
    out = []
    for chunk in chunks:
        sig, _, body = chunk.partition("\n")
        args, ret = _parse_signature(sig)
        out.append((args, ret, body.strip()))
    return out


def _section(body: str, title: str) -> str | None:
    """Return the text of a NumPy-style section, or None if it is missing."""
    m = re.search(rf"^{title}\n-{{{len(title)}}}\n(.*?)(?=^\S[^\n]*\n-{{3,}}\n|\Z)",
                  body, flags=re.S | re.M)
    return m.group(1) if m else None


def _check(qualname: str, args: list[str], ret: str, body: str) -> list[str]:
    problems = []
    summary = body.split("\n\n")[0].strip()
    if not summary:
        return [f"{qualname}: missing docstring"]
    if args:
        params = _section(body, "Parameters")
        if params is None:
            problems.append(f"{qualname}: missing 'Parameters' section")
        else:
            documented = set(re.findall(r"^(\w+)(?: :|$)", params, flags=re.M))
            missing = [a for a in args if a not in documented]
            if missing:
                problems.append(f"{qualname}: undocumented parameters {missing}")
    if ret not in ("None", "") and _section(body, "Returns") is None:
        problems.append(f"{qualname}: missing 'Returns' section")
    return problems


def _is_pybind(obj) -> bool:
    return type(obj).__name__ in ("builtin_function_or_method", "instancemethod") \
        or type(obj).__name__.startswith("pybind11")


def _public_classes():
    for mod in (epiworldpy, epimodels):
        for name in sorted(dir(mod)):
            obj = getattr(mod, name)
            if (not name.startswith("_") and inspect.isclass(obj)
                    and obj.__module__.startswith("epiworldpy")):
                yield f"{mod.__name__}.{name}", obj


def _public_members(cls):
    for name, member in sorted(vars(cls).items()):
        if name.startswith("_") and name != "__init__":
            continue
        if isinstance(member, staticmethod):
            member = member.__func__
        if callable(member):
            yield name, member


CLASSES = list(_public_classes())
FUNCTIONS = [
    (f"epiworldpy.{name}", getattr(epiworldpy, name))
    for name in epiworldpy.__all__
    if inspect.isfunction(getattr(epiworldpy, name))
]


@pytest.mark.parametrize("qualname,cls", CLASSES, ids=[q for q, _ in CLASSES])
def test_class_docstrings(qualname, cls):
    problems = []
    if not (cls.__doc__ or "").strip():
        problems.append(f"{qualname}: missing class docstring")
    if issubclass(cls, enum.Enum) or hasattr(cls, "__members__"):
        assert not problems, "\n".join(problems)
        return
    for name, member in _public_members(cls):
        if not _is_pybind(member):
            continue
        for args, ret, body in _pybind_overloads(member):
            problems += _check(f"{qualname}.{name}", args, ret, body)
    assert not problems, "\n".join(problems)


@pytest.mark.parametrize("qualname,func", FUNCTIONS,
                         ids=[q for q, _ in FUNCTIONS])
def test_function_docstrings(qualname, func):
    sig = inspect.signature(func)
    args = [p for p in sig.parameters
            if sig.parameters[p].kind not in (inspect.Parameter.VAR_KEYWORD,
                                              inspect.Parameter.VAR_POSITIONAL)]
    empty = (None, "None", inspect.Signature.empty)
    ret = "None" if sig.return_annotation in empty else "value"
    body = inspect.getdoc(func) or ""
    problems = _check(qualname, args, ret, body)
    assert not problems, "\n".join(problems)


def test_parser_understands_overloads():
    overloads = _pybind_overloads(epiworldpy.Agent.has_tool)
    assert [args for args, _, _ in overloads] == [["t"], ["name"]]
    assert all(ret == "bool" for _, ret, _ in overloads)
