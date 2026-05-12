"""
Template Matcher - Rotation and Translation Detection Library

A Python library for detecting template positions and rotation angles in images
using ORB, SIFT, or AKAZE feature detection algorithms.
"""

from .matcher import TemplateMatcher, DetectionMethod, MatchResult
from . import utils

__version__ = "0.1.0"
__all__ = ["TemplateMatcher", "DetectionMethod", "MatchResult", "utils"]
