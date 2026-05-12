# Template Matcher Library - Quick Start Guide

## Installation

```bash
cd python
pip install -r template_matcher/requirements.txt
```

Or to install as a package:

```bash
pip install -e .
```

## Basic Usage

### 1. Simple Template Matching

```python
from template_matcher import TemplateMatcher, DetectionMethod
from template_matcher.utils import load_image_with_transparency, load_image

# Load template (can have transparent background)
template = load_image_with_transparency("path/to/template.png")

# Load scene image
image = load_image("path/to/scene.jpg")

# Create matcher (default: SIFT - most accurate)
matcher = TemplateMatcher()

# Match template in image
result = matcher.match(template, image)

# Print results
print(f"Found at: ({result.x:.2f}, {result.y:.2f})")
print(f"Rotation: {result.rotation_angle:.2f}°")
print(f"Confidence: {result.confidence:.4f}")
```

### 2. Switch Between Methods

```python
# Use different detection methods
matcher.set_method(DetectionMethod.ORB)    # Fastest
matcher.set_method(DetectionMethod.AKAZE)  # Balanced
matcher.set_method(DetectionMethod.SIFT)   # Most accurate

result = matcher.match(template, image)
```

### 3. Get Visualization

```python
# Get result with visualization
result, visualization = matcher.match_with_visualization(
    template, image,
    draw_matches=True
)

if visualization is not None:
    from template_matcher.utils import save_image
    save_image(visualization, "result.png")
```

### 4. Test with Synthetic Data

```bash
# Run comprehensive test with synthetic data
cd template_matcher/tests
python test_demo.py
```

This will:
- Generate synthetic template and scenes
- Test all three methods
- Log results to `template_matching.log`
- Show accuracy metrics

## File Structure

```
python/
├── template_matcher/          # Main library
│   ├── __init__.py
│   ├── matcher.py            # Core matching logic
│   ├── utils.py              # Image utilities
│   ├── README.md             # Detailed documentation
│   ├── requirements.txt
│   └── tests/
│       ├── __init__.py
│       ├── test_demo.py      # Comprehensive test suite
│       └── examples.py       # Practical examples
├── setup.py                  # Package setup
└── QUICKSTART.md             # This file
```

## Key Classes and Functions

### TemplateMatcher

Main class for template matching.

```python
# Create with specific method
matcher = TemplateMatcher(DetectionMethod.SIFT)

# Switch method
matcher.set_method(DetectionMethod.ORB)

# Basic matching
result = matcher.match(template, image, min_matches=10)

# With visualization
result, vis = matcher.match_with_visualization(template, image, draw_matches=True)
```

### MatchResult

Result object with detection information.

```python
result.success            # bool - Was detection successful?
result.x                 # float - X coordinate of template center
result.y                 # float - Y coordinate of template center
result.rotation_angle    # float - Rotation in degrees (-180 to 180)
result.confidence        # float - Confidence score (0-1)
result.num_matches       # int - Number of matched features
result.method            # str - Detection method used
```

### Utility Functions

```python
# Image loading
template = load_image_with_transparency("file.png")  # Preserves alpha
image = load_image("file.jpg")                       # Standard BGR

# Image operations
resized = resize_image(image, max_size=1000)
combined = combine_images_horizontal(img1, img2)
overlay = overlay_template_on_image(template, image, x, y, rotation_angle)

# I/O
save_image(image, "output.png")
```

## Algorithm Comparison

| Method | Speed | Accuracy | Robustness |
|--------|-------|----------|-----------|
| **ORB** | ⚡⚡⚡ | ⭐⭐ | ⭐⭐ |
| **AKAZE** | ⚡⚡ | ⭐⭐⭐ | ⭐⭐⭐ |
| **SIFT** | ⚡ | ⭐⭐⭐⭐ | ⭐⭐⭐⭐ |

### When to Use Each:

- **ORB**: Real-time applications, embedded systems, when speed is critical
- **AKAZE**: Good balance for most applications
- **SIFT**: Highest accuracy needed, non-real-time applications

## Common Issues & Solutions

### "Success: False"
- Template may lack distinctive features
- Template might be too small
- Try increasing `min_matches` parameter
- Ensure template is actually in the image

### Low confidence score
- Try different detection method
- Use template with more texture/features
- Check image quality
- Ensure proper scale relationships

### Incorrect rotation angle
- Check that template has good feature distribution
- Try SIFT for better stability
- Verify template isn't too small

### Slow processing
- Use ORB instead of SIFT
- Reduce image size with `resize_image()`
- Process at lower resolution first

## Performance Tips

1. **Preprocessing**: Resize large images (>2000px) to ~1000px
2. **Feature Quality**: Use templates with distinctive patterns
3. **Min Matches**: Increase for more confidence (default 10)
4. **Method Selection**: 
   - Use SIFT for maximum accuracy
   - Use ORB for speed-critical applications
   - Use AKAZE for balance

## Advanced Usage

### Batch Processing

```python
from template_matcher import TemplateMatcher, DetectionMethod
from template_matcher.utils import load_image, load_image_with_transparency
import glob

template = load_image_with_transparency("template.png")
matcher = TemplateMatcher()

for image_file in glob.glob("scenes/*.jpg"):
    image = load_image(image_file)
    result = matcher.match(template, image)
    print(f"{image_file}: ({result.x:.1f}, {result.y:.1f}) @ {result.rotation_angle:.1f}°")
```

### Using Results

```python
result = matcher.match(template, image)

if result.success:
    # Template found
    center_x = result.x
    center_y = result.y
    angle = result.rotation_angle
    confidence = result.confidence
    
    # Do something with the results
    print(f"Template detected with {confidence*100:.1f}% confidence")
else:
    # Template not found
    print("Template not found in image")
```

## Logging

All test results are logged to `template_matching.log`:

```
2024-05-11 10:30:45,123 - INFO - Testing method: SIFT
2024-05-11 10:30:45,456 - INFO - Success: True
2024-05-11 10:30:45,789 - INFO - Position: x=250.00, y=280.00
2024-05-11 10:30:46,012 - INFO - Rotation: 45.00°
...
```

## Next Steps

1. **Run Tests**: `python template_matcher/tests/test_demo.py`
2. **Try Examples**: `python template_matcher/tests/examples.py`
3. **Use Your Own Images**: See examples for how to load and process
4. **Read Full Docs**: See [template_matcher/README.md](template_matcher/README.md)

## Support & Contribution

For issues, feature requests, or contributions, please refer to the main README.md file.
