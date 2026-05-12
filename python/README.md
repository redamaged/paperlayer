# Template Matcher - Complete Python Implementation

A professional-grade Python library for detecting template positions and rotation angles in images using ORB, SIFT, or AKAZE feature detection algorithms.

## Overview

This library solves the template matching + rotation estimation problem:
- **Input**: A template image (polygon on transparent background) and a larger scene image
- **Output**: Position (x, y), rotation angle, and confidence score
- **Methods**: ORB (fast), SIFT (accurate), AKAZE (balanced)
- **Features**: Handles transparency, rotation, multiple detection methods, visualization

## Quick Start

```bash
# Install dependencies
pip install -r template_matcher/requirements.txt

# Run tests
cd template_matcher/tests
python test_demo.py

# Use in your code
from template_matcher import TemplateMatcher, DetectionMethod
from template_matcher.utils import load_image_with_transparency, load_image

template = load_image_with_transparency("template.png")
image = load_image("scene.jpg")

matcher = TemplateMatcher()  # Default: SIFT
result = matcher.match(template, image)

print(f"Position: ({result.x:.2f}, {result.y:.2f})")
print(f"Rotation: {result.rotation_angle:.2f}°")
print(f"Confidence: {result.confidence:.4f}")
```

## Project Structure

```
python/
├── template_matcher/                # Main library package
│   ├── __init__.py                 # Package exports
│   ├── matcher.py                  # Core matching algorithm (600+ lines)
│   ├── utils.py                    # Image utilities & visualization (350+ lines)
│   ├── README.md                   # Full documentation
│   ├── requirements.txt            # Dependencies
│   └── tests/
│       ├── __init__.py
│       ├── test_demo.py            # Comprehensive test suite (400+ lines)
│       └── examples.py             # Practical usage examples
│
├── setup.py                        # Package installer
├── QUICKSTART.md                   # Quick start guide
├── API_REFERENCE.md               # Complete API documentation
├── TEMPLATE_PREPARATION.md        # Guide for preparing templates
└── README.md                       # This file
```

## Features

### ✨ Core Features

- **Three Detection Methods**
  - `ORB`: Fast, real-time capable
  - `SIFT`: Most accurate, slower
  - `AKAZE`: Good balance between speed and accuracy

- **Rotation Estimation**: Automatically calculates rotation angle from feature matches
- **Translation Detection**: Precise x/y position detection
- **Transparent Background Support**: Works with PNG templates with alpha channel
- **Confidence Scoring**: Returns 0-1 confidence metric based on match quality
- **Easy Method Switching**: Switch algorithms at runtime

### 🎨 Visualization Features

- Draw bounding box around detected template
- Draw rotation indicator (arrow)
- Display position and confidence on image
- Overlay template on detection result
- Side-by-side image comparison utilities

### 📊 Utility Features

- Image loading (with/without transparency)
- Image resizing with aspect ratio preservation
- Image overlaying with alpha blending
- Batch processing helper patterns
- Comprehensive logging

## Installation

### Option 1: Direct Installation

```bash
# Install dependencies
pip install -r template_matcher/requirements.txt
```

### Option 2: Install as Package

```bash
# From python directory
pip install -e .
```

### Option 3: Manual Installation

```bash
pip install opencv-python>=4.5.0 numpy>=1.19.0
```

## Quick Examples

### Example 1: Basic Matching

```python
from template_matcher import TemplateMatcher
from template_matcher.utils import load_image_with_transparency, load_image

# Load images
template = load_image_with_transparency("template.png")
image = load_image("scene.jpg")

# Create matcher
matcher = TemplateMatcher()

# Match
result = matcher.match(template, image)

# Check results
if result.success:
    print(f"Found at: ({result.x}, {result.y})")
    print(f"Rotation: {result.rotation_angle}°")
    print(f"Confidence: {result.confidence}")
```

### Example 2: Compare Methods

```python
from template_matcher import TemplateMatcher, DetectionMethod

matcher = TemplateMatcher()

for method in DetectionMethod:
    matcher.set_method(method)
    result = matcher.match(template, image)
    print(f"{method.value}: ({result.x:.1f}, {result.y:.1f}) @ {result.rotation_angle:.1f}°")
```

### Example 3: With Visualization

```python
result, visualization = matcher.match_with_visualization(template, image, draw_matches=True)

if visualization is not None:
    from template_matcher.utils import save_image
    save_image(visualization, "result.png")
```

### Example 4: Batch Processing

```python
import glob
from template_matcher.utils import load_image

template = load_image_with_transparency("template.png")
matcher = TemplateMatcher()

for image_file in glob.glob("scenes/*.jpg"):
    image = load_image(image_file)
    result = matcher.match(template, image)
    print(f"{image_file}: ({result.x:.1f}, {result.y:.1f}) @ {result.rotation_angle:.1f}°")
```

## Running Tests

### Test 1: Comprehensive Test Suite

```bash
cd template_matcher/tests
python test_demo.py
```

This will:
- Generate synthetic test data
- Test all three detection methods
- Compare with ground truth
- Generate visualizations
- Log results to `template_matching.log`

### Test 2: Practical Examples

```bash
cd template_matcher/tests
python examples.py
```

This runs 6 practical examples:
1. Basic template matching
2. Compare all detection methods
3. Visualization generation
4. Overlay results
5. Batch processing
6. Parameter tuning

## Documentation

### Getting Started
- **[QUICKSTART.md](QUICKSTART.md)** - Quick start guide with basic examples
- **[template_matcher/README.md](template_matcher/README.md)** - Full feature documentation

### Reference
- **[API_REFERENCE.md](API_REFERENCE.md)** - Complete API documentation with all functions and classes
- **[TEMPLATE_PREPARATION.md](TEMPLATE_PREPARATION.md)** - Guide for preparing high-quality template images

## Key Classes

### `TemplateMatcher`
Main class for template matching and rotation estimation.

```python
from template_matcher import TemplateMatcher, DetectionMethod

# Create matcher
matcher = TemplateMatcher(method=DetectionMethod.SIFT)

# Switch method
matcher.set_method(DetectionMethod.ORB)

# Match
result = matcher.match(template, image, min_matches=10)

# With visualization
result, vis = matcher.match_with_visualization(template, image, draw_matches=True)
```

### `MatchResult`
Result object containing detection information.

```python
result.x                 # float - X position
result.y                 # float - Y position  
result.rotation_angle    # float - Rotation in degrees
result.confidence        # float - Confidence (0-1)
result.num_matches       # int - Number of matched features
result.success           # bool - Detection successful?
result.method            # str - Method used
```

### `DetectionMethod`
Enum for selecting algorithm.

```python
from template_matcher import DetectionMethod

DetectionMethod.ORB      # Fastest
DetectionMethod.AKAZE    # Balanced
DetectionMethod.SIFT     # Most accurate
```

## Utility Functions

```python
from template_matcher.utils import (
    load_image_with_transparency,    # Load with alpha channel
    load_image,                       # Load standard image
    save_image,                       # Save image
    resize_image,                     # Resize with aspect ratio
    overlay_template_on_image,        # Overlay with rotation
    combine_images_horizontal,        # Combine two images
    display_image,                    # Show image
    create_checkerboard_bg            # Visualize transparency
)
```

## Performance Comparison

| Method | Speed | Accuracy | Best For |
|--------|-------|----------|----------|
| **ORB** | ⚡⚡⚡ Very Fast | ⭐⭐ Good | Real-time, embedded systems |
| **AKAZE** | ⚡⚡ Fast | ⭐⭐⭐ Very Good | Most applications |
| **SIFT** | ⚡ Slow | ⭐⭐⭐⭐ Excellent | High precision needed |

## Algorithm Details

### How It Works

1. **Feature Detection**: Extract distinctive keypoints from both template and scene
2. **Feature Matching**: Find correspondences between template and scene features
3. **Homography Estimation**: Compute transformation matrix using RANSAC
4. **Transformation Extraction**: Extract translation (x, y) and rotation from matrix

### Rotation Calculation

The rotation angle is extracted from the homography matrix using SVD decomposition:
- Homography `H` = transformation matrix
- Extract rotation component via `U @ Vt` (orthogonal part)
- Convert to angle using `arctan2`
- Normalize to [-180°, 180°] range

## Troubleshooting

### Detection Returns False
- Template lacks distinctive features
- Template too small or plain
- Template not in scene
- **Solution**: Add texture to template or increase size

### Low Confidence
- Not enough feature matches
- Method not suitable for template type
- **Solution**: Try SIFT, increase `min_matches`

### Slow Processing
- Using SIFT on large images
- Image resolution too high
- **Solution**: Use ORB, resize image to ~1000px

### Incorrect Rotation
- Poorly conditioned homography
- Template too plain
- **Solution**: Add more features to template, use SIFT

## Advanced Usage

### Custom Minimum Matches

```python
# More stringent (higher confidence)
result = matcher.match(template, image, min_matches=30)

# More lenient (higher recall)
result = matcher.match(template, image, min_matches=5)
```

### Ensemble Voting

```python
# Use all methods and vote on best result
results = {}
for method in DetectionMethod:
    matcher.set_method(method)
    result = matcher.match(template, image)
    results[method.value] = result

# Pick best by confidence
best = max(results.values(), key=lambda r: r.confidence if r.success else 0)
```

### Batch Processing with Logging

```python
import logging
logging.basicConfig(level=logging.INFO, 
                   format='%(message)s',
                   filename='results.log')

for image_file in image_files:
    result = matcher.match(template, load_image(image_file))
    logging.info(f"{image_file}: {result.x:.1f}, {result.y:.1f}, {result.rotation_angle:.1f}°")
```

## Requirements

- Python 3.7+
- OpenCV (opencv-python 4.5.0+)
- NumPy 1.19.0+

## Limitations

1. **Fixed Scale**: Only estimates rotation + translation, not scale
2. **Minimum Features**: Template must have distinguishable features
3. **Rotation Range**: Works best for full 360° but may struggle at extreme angles
4. **Feature Density**: Works best with textured templates, not solid colors

## Known Issues

None currently. See troubleshooting section for common usage issues.

## Contributing

Contributions welcome! Potential improvements:
- Multi-scale detection
- GPU acceleration with CUDA
- Additional feature detectors
- Perspective transformation handling
- Template pyramid for large images

## Future Enhancements

- [ ] Scale estimation
- [ ] Batch CUDA acceleration
- [ ] Deep learning feature extractor option
- [ ] Template pyramid for large scale differences
- [ ] Real-time tracking mode
- [ ] Confidence estimation improvements

## License

MIT License

## Citation

If you use this library in research, please cite:

```bibtex
@software{template_matcher_2024,
  title={Template Matcher: Rotation and Translation Detection Library},
  author={Your Name},
  year={2024},
  url={https://github.com/yourrepo/template-matcher}
}
```

## Support

- Check [API_REFERENCE.md](API_REFERENCE.md) for API documentation
- Review [TEMPLATE_PREPARATION.md](TEMPLATE_PREPARATION.md) for template preparation
- Run `python template_matcher/tests/test_demo.py` to verify installation
- Run `python template_matcher/tests/examples.py` for practical examples

## Changelog

### Version 0.1.0 (2024-05-11)
- Initial release
- ORB, SIFT, AKAZE detection methods
- Rotation and translation estimation
- Transparent background support
- Comprehensive visualization utilities
- Full test suite and documentation

---

**Made with ❤️ for computer vision enthusiasts**
