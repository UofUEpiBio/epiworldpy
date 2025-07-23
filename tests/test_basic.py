from importlib.metadata import version

def test_version():
    assert version('epiworldpy') == "0.6.0.post0"
