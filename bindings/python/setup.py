"""Setup for arix-algo Python package."""
from setuptools import setup, find_packages

setup(
    name="arix-algo",
    version="0.7.8",
    packages=find_packages(),
    package_data={"arix_algo": ["_arix_c*.pyd", "_arix_c*.so"]},
    include_package_data=True,
    zip_safe=False,
)
