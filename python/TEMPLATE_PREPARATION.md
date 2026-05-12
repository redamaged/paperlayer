# Template Preparation Guide

This guide explains how to prepare your template images for the best results with the template matcher library.

## Template Image Requirements

### Format
- **Preferred**: PNG with alpha channel (transparency)
- **Supported**: PNG, JPG, BMP
- **With Transparency**: Allows masking out the background

### Size
- **Recommended**: 50-200 pixels
- **Minimum**: 30 pixels (very small templates lose features)
- **Maximum**: 400 pixels (large templates may be too specific)
- **Aspect Ratio**: Any ratio is fine

### Content Requirements

#### What Works Well
✅ Textured templates with distinct patterns
✅ Templates with color variation
✅ Templates with clear edges and corners
✅ Templates with repeated patterns or textures
✅ Natural images with diverse features

#### What Doesn't Work Well
❌ Solid color rectangles
❌ Very plain or uniform patterns
❌ Templates with only straight lines
❌ Templates that are too similar to background
❌ Very small text or fine details

## Preparing a Transparent Background Template

### Method 1: Using GIMP

1. Open your template image in GIMP
2. Add an alpha channel: Layer → Transparency → Add Alpha Channel
3. Use "Select by Color" tool to select background
4. Delete the background
5. Export as PNG (File → Export As)

### Method 2: Using Photoshop

1. Open template image
2. Add a layer mask to isolate your object
3. Save as PNG with transparency

### Method 3: Using Python (PIL/Pillow)

```python
from PIL import Image
import numpy as np

# Load image
img = Image.open("template.jpg").convert("RGBA")
data = np.array(img)

# Make white/light areas transparent
white_areas = (data[:,:,0] > 200) & (data[:,:,1] > 200) & (data[:,:,2] > 200)
data[white_areas, 3] = 0  # Set alpha to transparent

# Save
result = Image.fromarray(data)
result.save("template_transparent.png")
```

### Method 4: Using OpenCV (Python)

```python
import cv2
import numpy as np

# Load image
img = cv2.imread("template.jpg")
gray = cv2.cvtColor(img, cv2.COLOR_BGR2GRAY)

# Create mask for background (adjust threshold as needed)
_, mask = cv2.threshold(gray, 200, 255, cv2.THRESH_BINARY)

# Create RGBA image with alpha channel
img_rgba = cv2.cvtColor(img, cv2.COLOR_BGR2BGRA)
img_rgba[:,:,3] = cv2.bitwise_not(mask)

# Save
cv2.imwrite("template_transparent.png", img_rgba)
```

## Testing Your Template

### Visual Inspection

```python
from template_matcher.utils import load_image_with_transparency, create_checkerboard_bg
import cv2

# Load your template
template = load_image_with_transparency("template.png")

# View on checkerboard to see transparency
display = create_checkerboard_bg(template)
cv2.imshow("Template Preview", display)
cv2.waitKey(0)
```

### Feature Detection Test

```python
from template_matcher import TemplateMatcher, DetectionMethod
import cv2

# Load template
template_rgb = cv2.cvtColor(cv2.imread("template.png", -1), cv2.COLOR_BGRA2BGR)
template_gray = cv2.cvtColor(template_rgb, cv2.COLOR_BGR2GRAY)

# Test feature detection
matcher = TemplateMatcher(DetectionMethod.SIFT)
kp, desc = matcher.detector.detectAndCompute(template_gray, None)

print(f"Features detected: {len(kp)}")
if len(kp) < 10:
    print("WARNING: Template has too few features. Add more texture or use AKAZE.")
else:
    print("OK: Template has sufficient features")

# Visualize keypoints
img_with_kp = cv2.drawKeypoints(template_gray, kp, None)
cv2.imshow("Detected Features", img_with_kp)
cv2.waitKey(0)
```

## Troubleshooting Template Issues

### Issue: "Too few features detected"

**Symptoms**:
- Detection returns `success: False`
- `num_matches` is very low

**Solutions**:
1. Add texture/pattern to template
   - Add gradient colors
   - Add grid pattern
   - Include natural texture

2. Increase template size
   - Small templates (< 40px) often lack features
   - Try 80-150px instead

3. Try different detection method
   - AKAZE might work better
   - SIFT is most tolerant of plain templates

### Issue: "False detections or incorrect position"

**Symptoms**:
- High confidence but wrong location
- Unrealistic rotation angles

**Solutions**:
1. Ensure template is unique
   - Add more distinctive features
   - Make sure template won't match other objects in scene

2. Increase `min_matches` parameter
   - Use `min_matches=20` or higher
   - Higher threshold = more confident but might miss matches

3. Verify scene contains template
   - Check that template is actually visible in scene
   - Check scale (template shouldn't be too different in size)

### Issue: "Very slow processing"

**Symptoms**:
- Matching takes >5 seconds
- Only happens with certain methods

**Solutions**:
1. Use ORB instead of SIFT
   - Much faster, acceptable accuracy for many cases

2. Use AKAZE as compromise
   - Better than ORB, faster than SIFT

3. Resize template
   - Smaller template = faster processing

4. Resize scene image
   - If scene > 2000px, reduce to ~1000px
   - Use `resize_image()` utility

## Template Examples

### Good Template: Textured Object
```
Properties:
- Size: 120x100 px
- Color varied with texture
- Multiple colors (red, green, blue)
- Features: circles, rectangles, lines
- Result: ~200 features detected ✓
```

### Poor Template: Solid Color
```
Properties:
- Size: 100x100 px
- Single color throughout
- No texture or variation
- Features: only edges
- Result: ~5 features detected ✗
```

### Good Template: Natural Image
```
Properties:
- Size: 100x80 px
- Photo of object with natural texture
- Varied colors and lighting
- Complex structure
- Result: ~300+ features detected ✓
```

## Testing Strategy

1. **Create synthetic test first**
   - Use test_demo.py with generated data
   - Verify library works correctly

2. **Test with your template**
   - Start with same method as synthetic test
   - Verify detection works

3. **Compare methods**
   - Try all three methods with your data
   - Pick best-performing one

4. **Optimize min_matches**
   - Adjust for your use case
   - Higher for more confidence, lower for recall

5. **Fine-tune template**
   - If results are poor, modify template
   - Add more texture if needed

## Best Practices

1. **Use consistent lighting** - Templates and scenes should have similar lighting conditions
2. **Include context** - A bit of surrounding area in template helps
3. **Avoid reflections** - Shiny/reflective surfaces confuse feature detectors
4. **Test variations** - Test with different rotations to verify robustness
5. **Document results** - Keep notes on what works and what doesn't

## Example Workflow

```python
from template_matcher import TemplateMatcher, DetectionMethod
from template_matcher.utils import load_image_with_transparency, load_image
import logging

logging.basicConfig(level=logging.INFO)

# 1. Load your images
template = load_image_with_transparency("my_template.png")
image = load_image("my_scene.jpg")

# 2. Try each method
for method in [DetectionMethod.SIFT, DetectionMethod.AKAZE, DetectionMethod.ORB]:
    matcher = TemplateMatcher(method=method)
    result = matcher.match(template, image, min_matches=15)
    
    print(f"\n{method.value.upper()}:")
    print(f"  Success: {result.success}")
    if result.success:
        print(f"  Position: ({result.x:.1f}, {result.y:.1f})")
        print(f"  Rotation: {result.rotation_angle:.1f}°")
        print(f"  Confidence: {result.confidence:.3f}")
        print(f"  Matches: {result.num_matches}")

# 3. Use the best method for your production code
# (Or implement ensemble voting for robustness)
```

## Additional Resources

- OpenCV Feature Detection: https://docs.opencv.org/master/d0/d13/classcv_1_1Feature2D.html
- SIFT: Distinctive Image Features from Scale-Invariant Keypoints
- ORB: An Efficient Alternative to SIFT or SURF
- AKAZE: Fast Explicit Diffusion for Accelerated Features in Nonlinear Scale Spaces

---

**Tips from Experience:**
- Start with SIFT - it's most robust
- Use a template with 8+ distinguishable features
- If template is very small, reduce `min_matches`
- Batch test with different `min_matches` values
- Always validate results with visualization
