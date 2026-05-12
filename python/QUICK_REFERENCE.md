# Template Matcher - Quick Reference

## Installation

```bash
# Install dependencies
pip install -r template_matcher/requirements.txt

# Or install as package
pip install -e .
```

## Basic Usage

```python
from template_matcher import TemplateMatcher, DetectionMethod
from template_matcher.utils import load_image_with_transparency, load_image

# Load images
template = load_image_with_transparency("template.png")  # Supports transparency
image = load_image("scene.jpg")                          # Regular image

# Create matcher (default: SIFT)
matcher = TemplateMatcher()

# Match template
result = matcher.match(template, image)

# Check result
print(f"Found: {result.success}")
print(f"Position: {result.x}, {result.y}")
print(f"Rotation: {result.rotation_angle}°")
```

## Common Operations

### 1. Switch Detection Method

```python
matcher.set_method(DetectionMethod.ORB)     # Fast
matcher.set_method(DetectionMethod.AKAZE)   # Balanced
matcher.set_method(DetectionMethod.SIFT)    # Accurate
```

### 2. Get Visualization

```python
result, vis = matcher.match_with_visualization(template, image, draw_matches=True)
if vis is not None:
    from template_matcher.utils import save_image
    save_image(vis, "result.png")
```

### 3. Overlay Result

```python
from template_matcher.utils import overlay_template_on_image

overlay = overlay_template_on_image(
    template, image,
    result.x, result.y,
    result.rotation_angle,
    alpha=0.6
)
```

### 4. Compare Methods

```python
for method in DetectionMethod:
    matcher.set_method(method)
    result = matcher.match(template, image)
    print(f"{method.value}: ({result.x:.1f}, {result.y:.1f}) @ {result.rotation_angle:.1f}°")
```

### 5. Batch Processing

```python
import glob

for image_file in glob.glob("scenes/*.jpg"):
    result = matcher.match(template, load_image(image_file))
    print(f"{image_file}: {result.x:.1f}, {result.y:.1f}")
```

### 6. Adjust Minimum Matches

```python
# Higher confidence (requires more matches)
result = matcher.match(template, image, min_matches=25)

# Lower confidence (accepts fewer matches)
result = matcher.match(template, image, min_matches=5)
```

## MatchResult Fields

| Field | Type | Meaning |
|-------|------|---------|
| `success` | bool | Detection successful? |
| `x` | float | X position (pixels) |
| `y` | float | Y position (pixels) |
| `rotation_angle` | float | Rotation (-180 to 180°) |
| `confidence` | float | Confidence score (0-1) |
| `num_matches` | int | Number of feature matches |
| `method` | str | Method used ("orb", "sift", "akaze") |

## Utility Functions

```python
# Loading
template = load_image_with_transparency("template.png")
image = load_image("scene.jpg")

# Saving
save_image(image, "output.png")

# Resizing
small_image = resize_image(image, max_size=1000)

# Visualization
overlay = overlay_template_on_image(template, image, x, y, angle)
combined = combine_images_horizontal(img1, img2)
display_image(image, title="Preview", wait_ms=3000)

# Transparency visualization
checkerboard = create_checkerboard_bg(template)
```

## Method Selection Guide

| Use Case | Method | Why |
|----------|--------|-----|
| Real-time processing | ORB | Fastest speed |
| Balanced application | AKAZE | Speed + accuracy |
| High precision needed | SIFT | Most accurate |
| Mobile/embedded | ORB | Low memory usage |
| Textured templates | SIFT | Handles texture well |
| Plain templates | AKAZE | More robust |

## Performance Tips

1. **Reduce image size**: `resize_image(image, max_size=800)`
2. **Use ORB for speed**: `DetectionMethod.ORB`
3. **Use SIFT for accuracy**: `DetectionMethod.SIFT`
4. **Adjust min_matches**: Balance confidence vs recall
5. **Preprocess template**: Ensure it has distinctive features

## Troubleshooting

| Problem | Cause | Solution |
|---------|-------|----------|
| `success: False` | Too few matches | Add texture to template |
| Low `confidence` | Weak match | Try SIFT, increase `min_matches` |
| Wrong position | Ambiguous features | Use clearer template |
| Slow processing | Using SIFT + large images | Use ORB, resize image |
| Incorrect rotation | Poor homography | Ensure good features in template |

## Test & Verify

```bash
# Run full test suite
cd template_matcher/tests
python test_demo.py

# Run examples
python examples.py

# Check logs
cat template_matching.log
```

## Template Preparation

✅ Good templates:
- 80-150 pixels size
- Multiple colors
- Distinct features (circles, corners, edges)
- Good texture/contrast

❌ Poor templates:
- Solid colors
- Only straight lines
- Very small (< 40px)
- Very plain or uniform

## Code Snippets

### Find Best Method

```python
best_method = None
best_confidence = 0

for method in DetectionMethod:
    matcher.set_method(method)
    result = matcher.match(template, image)
    if result.success and result.confidence > best_confidence:
        best_confidence = result.confidence
        best_method = method

print(f"Best: {best_method.value} ({best_confidence:.3f})")
```

### Save All Results

```python
from template_matcher.utils import overlay_template_on_image, save_image

result = matcher.match(template, image)

if result.success:
    # Save visualization
    vis = matcher._draw_result(template, image, result)
    save_image(vis, "debug_visualization.png")
    
    # Save overlay
    overlay = overlay_template_on_image(template, image, result.x, result.y, result.rotation_angle)
    save_image(overlay, "overlay.png")
    
    # Log results
    print(f"Success! Position: ({result.x:.2f}, {result.y:.2f}), Rotation: {result.rotation_angle:.2f}°")
```

### Error Handling

```python
try:
    result = matcher.match(template, image)
    
    if not result.success:
        print(f"Detection failed - only {result.num_matches} matches")
    elif result.confidence < 0.5:
        print(f"Low confidence: {result.confidence:.2f}")
    else:
        print(f"Success: ({result.x:.1f}, {result.y:.1f})")
        
except Exception as e:
    print(f"Error: {e}")
```

## Resources

- **[README.md](README.md)** - Full documentation
- **[API_REFERENCE.md](API_REFERENCE.md)** - Complete API reference
- **[TEMPLATE_PREPARATION.md](TEMPLATE_PREPARATION.md)** - Template preparation guide
- **[QUICKSTART.md](QUICKSTART.md)** - Detailed quick start
- **tests/test_demo.py** - Comprehensive tests
- **tests/examples.py** - Practical examples

## Imports You'll Need

```python
# Core library
from template_matcher import TemplateMatcher, DetectionMethod

# Utilities
from template_matcher.utils import (
    load_image_with_transparency,
    load_image,
    save_image,
    resize_image,
    overlay_template_on_image,
    combine_images_horizontal,
    display_image
)

# Standard libraries
import cv2
import numpy as np
import glob
import logging
```

## Quick Debug Checklist

When detection fails:

- [ ] Template actually in the scene?
- [ ] Template size reasonable (50-200px)?
- [ ] Template has distinctive features?
- [ ] Try different detection method
- [ ] Increase `min_matches` to verify match quality
- [ ] Check visualization to see what's matched
- [ ] Try SIFT for best accuracy
- [ ] Resize images if very large

---

**For full documentation, see the main README.md**
