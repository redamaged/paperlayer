#!/usr/bin/env python3
"""
Setup script for template_matcher package.
"""

from setuptools import setup, find_packages

with open("template_matcher/README.md", "r", encoding="utf-8") as fh:
    long_description = fh.read()

setup(
    name="template-matcher",
    version="0.1.0",
    author="Your Name",
    description="Template matching with rotation estimation using ORB, SIFT, AKAZE",
    long_description=long_description,
    long_description_content_type="text/markdown",
    url="https://github.com/yourusername/template-matcher",
    packages=find_packages(),
    classifiers=[
        "Programming Language :: Python :: 3",
        "Programming Language :: Python :: 3.7",
        "Programming Language :: Python :: 3.8",
        "Programming Language :: Python :: 3.9",
        "Programming Language :: Python :: 3.10",
        "License :: OSI Approved :: MIT License",
        "Operating System :: OS Independent",
        "Development Status :: 3 - Alpha",
        "Intended Audience :: Developers",
        "Topic :: Scientific/Engineering :: Image Recognition",
    ],
    python_requires=">=3.7",
    install_requires=[
        "opencv-python>=4.5.0",
        "numpy>=1.19.0",
    ],
    extras_require={
        "dev": [
            "pytest>=6.0",
            "pytest-cov>=2.10",
        ],
    },
)
