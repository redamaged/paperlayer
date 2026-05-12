# API Reference - Template Matcher Library

## Core Classes

### `TemplateMatcher`

Main class for template matching and rotation estimation.

#### Constructor
```python
TemplateMatcher(method: DetectionMethod = DetectionMethod.SIFT)
```

**Parameters:**
- `method` (DetectionMethod): Feature detection method to use
  - `DetectionMethod.SIFT` (default): Most accurate
  - `DetectionMethod.ORB`: Fastest
  - `DetectionMethod.AKAZE`: Good balance

**Example:**
```python
# Create with default SIFT
matcher = TemplateMatcher()

# Create with specific method
matcher = TemplateMatcher(DetectionMethod.ORB)
```

#### Methods

##### `match(template, image, min_matches=10) -> MatchResult`

Detect template in image and estimate rotation and translation.

**Parameters:**
- `template` (np.ndarray): Template image (BGR or BGRA with transparency)
- `image` (np.ndarray): Larger image to search in (BGR)
- `min_matches` (int): Minimum number of feature matches required for detection (default: 10)

**Returns:**
- `MatchResult`: Result object with position, rotation, and confidence

**Example:**
```python
result = matcher.match(template, image, min_matches=15)
if result.success:
    print(f"Found at: ({result.x}, {result.y})")
```

---

##### `set_method(method: DetectionMethod) -> None`

Switch to a different detection method.

**Parameters:**
- `method` (DetectionMethod): New method to use

**Example:**
```python
matcher.set_method(DetectionMethod.AKAZE)
result = matcher.match(template, image)
```

---

##### `match_with_visualization(template, image, draw_matches=False) -> Tuple[MatchResult, Optional[np.ndarray]]`

Detect template and optionally generate visualization.

**Parameters:**
- `template` (np.ndarray): Template image
- `image` (np.ndarray): Scene image
- `draw_matches` (bool): Whether to generate visualization (default: False)

**Returns:**
- Tuple of:
  - `MatchResult`: Detection result
  - `np.ndarray or None`: Visualization image with bounding box and rotation indicator

**Example:**
```python
result, visualization = matcher.match_with_visualization(
    template, image, draw_matches=True
)
if visualization is not None:
    cv2.imshow("Result", visualization)
```

---

### `MatchResult`

Dataclass containing detection results.

#### Attributes

```python
@dataclass
class MatchResult:
    x: float                    # X coordinate of template center in image
    y: float                    # Y coordinate of template center in image
    rotation_angle: float       # Rotation angle in degrees (-180 to 180)
    confidence: float           # Confidence score (0.0 to 1.0)
    num_matches: int            # Number of matched features
    method: str                 # Detection method used (e.g., "sift")
    success: bool               # Whether detection was successful
```

#### Properties Explained

**`x`, `y`**: Position of template center in the image coordinate system
- Origin (0, 0) is top-left
- `x` increases rightward
- `y` increases downward

**`rotation_angle`**: Rotation in degrees, normalized to [-180, 180]
- 0° = no rotation (original orientation)
- Positive = counter-clockwise rotation
- Negative = clockwise rotation

**`confidence`**: Reliability of detection (0.0 to 1.0)
- Based on ratio of inliers to total matches
- Higher is better
- 1.0 = perfect inliers match

**`num_matches`**: Number of successfully matched features
- Higher count generally means more reliable detection
- Should be ≥ `min_matches` for successful detection

**`method`**: Name of detection method used
- "orb", "sift", or "akaze"

**`success`**: Boolean flag indicating detection success
- True if enough matches found and homography computed
- False if detection failed

---

### `DetectionMethod`

Enum class for selecting detection algorithms.

#### Values

```python
class DetectionMethod(Enum):
    ORB = "orb"       # Oriented FAST and Rotated BRIEF
    SIFT = "sift"     # Scale-Invariant Feature Transform
    AKAZE = "akaze"   # Accelerated-KAZE
```

#### Comparison

| Feature | ORB | AKAZE | SIFT |
|---------|-----|-------|------|
| Speed | ⚡⚡⚡ | ⚡⚡ | ⚡ |
| Accuracy | ⭐⭐ | ⭐⭐⭐ | ⭐⭐⭐⭐ |
| Robustness | ⭐⭐ | ⭐⭐⭐ | ⭐⭐⭐⭐ |
| Memory | Low | Medium | High |

---

## Utility Functions

### Image Loading

#### `load_image_with_transparency(image_path: str) -> np.ndarray`

Load image preserving alpha channel if present.

**Parameters:**
- `image_path` (str): Path to image file

**Returns:**
- `np.ndarray`: BGRA image if PNG with alpha, BGR otherwise

**Example:**
```python
template = load_image_with_transparency("template.png")  # Preserves transparency
```

---

#### `load_image(image_path: str) -> np.ndarray`

Load image as BGR (standard OpenCV format).

**Parameters:**
- `image_path` (str): Path to image file

**Returns:**
- `np.ndarray`: BGR image

**Example:**
```python
image = load_image("scene.jpg")
```

---

### Image Processing

#### `resize_image(image: np.ndarray, max_size: int = 1000) -> np.ndarray`

Resize image if larger than specified size, maintaining aspect ratio.

**Parameters:**
- `image` (np.ndarray): Input image
- `max_size` (int): Maximum width or height (default: 1000)

**Returns:**
- `np.ndarray`: Resized image (or original if smaller)

**Example:**
```python
small_image = resize_image(large_image, max_size=800)
```

---

#### `overlay_template_on_image(template, image, x, y, rotation_angle=0.0, alpha=0.5) -> np.ndarray`

Overlay template at specified position and rotation on image.

**Parameters:**
- `template` (np.ndarray): Template image (BGRA or BGR)
- `image` (np.ndarray): Target image (BGR)
- `x` (float): X coordinate of template center
- `y` (float): Y coordinate of template center
- `rotation_angle` (float): Rotation angle in degrees (default: 0.0)
- `alpha` (float): Blending alpha 0.0-1.0 (default: 0.5)

**Returns:**
- `np.ndarray`: Image with overlayed template

**Example:**
```python
overlayed = overlay_template_on_image(
    template, image,
    result.x, result.y, 
    result.rotation_angle,
    alpha=0.7
)
```

---

#### `combine_images_horizontal(img1: np.ndarray, img2: np.ndarray) -> np.ndarray`

Combine two images side-by-side.

**Parameters:**
- `img1` (np.ndarray): First image
- `img2` (np.ndarray): Second image

**Returns:**
- `np.ndarray`: Combined image

**Example:**
```python
combined = combine_images_horizontal(before, after)
```

---

### File I/O

#### `save_image(image: np.ndarray, output_path: str) -> None`

Save image to file.

**Parameters:**
- `image` (np.ndarray): Image to save
- `output_path` (str): Output file path

**Example:**
```python
save_image(result_image, "output.png")
```

---

#### `display_image(image: np.ndarray, title: str = "Image", wait_ms: int = 0) -> None`

Display image in window.

**Parameters:**
- `image` (np.ndarray): Image to display
- `title` (str): Window title (default: "Image")
- `wait_ms` (int): Wait time in milliseconds, 0 = wait for key press (default: 0)

**Example:**
```python
display_image(result_image, "Match Result", wait_ms=3000)
```

---

### Visualization

#### `create_checkerboard_bg(template: np.ndarray, bg_color=(200,200,200)) -> np.ndarray`

Create visualization of template on checkerboard background.

**Parameters:**
- `template` (np.ndarray): Template image (BGRA)
- `bg_color` (tuple): RGB background color

**Returns:**
- `np.ndarray`: Visualization image

**Example:**
```python
display = create_checkerboard_bg(template, bg_color=(255,255,255))
```

---

## Common Patterns

### Pattern 1: Basic Matching

```python
from template_matcher import TemplateMatcher
from template_matcher.utils import load_image, load_image_with_transparency

template = load_image_with_transparency("template.png")
image = load_image("scene.jpg")

matcher = TemplateMatcher()
result = matcher.match(template, image)

if result.success:
    print(f"Position: ({result.x:.2f}, {result.y:.2f})")
    print(f"Rotation: {result.rotation_angle:.2f}°")
```

---

### Pattern 2: Compare All Methods

```python
from template_matcher import TemplateMatcher, DetectionMethod

matcher = TemplateMatcher()
best_result = None
best_confidence = 0

for method in DetectionMethod:
    matcher.set_method(method)
    result = matcher.match(template, image)
    
    if result.success and result.confidence > best_confidence:
        best_confidence = result.confidence
        best_result = result

print(f"Best: {best_result.method} with confidence {best_confidence:.3f}")
```

---

### Pattern 3: Batch Processing

```python
from pathlib import Path
from template_matcher import TemplateMatcher
from template_matcher.utils import load_image, load_image_with_transparency

template = load_image_with_transparency("template.png")
matcher = TemplateMatcher()

for image_path in Path("scenes").glob("*.jpg"):
    image = load_image(str(image_path))
    result = matcher.match(template, image)
    print(f"{image_path.name}: {result.x:.1f}, {result.y:.1f}, {result.rotation_angle:.1f}°")
```

---

### Pattern 4: Visualization Pipeline

```python
from template_matcher import TemplateMatcher
from template_matcher.utils import (
    load_image, load_image_with_transparency,
    overlay_template_on_image, save_image
)

template = load_image_with_transparency("template.png")
image = load_image("scene.jpg")

matcher = TemplateMatcher()
result, vis = matcher.match_with_visualization(template, image, draw_matches=True)

if result.success:
    # Save visualization
    save_image(vis, "visualization.png")
    
    # Create overlay
    overlay = overlay_template_on_image(
        template, image,
        result.x, result.y, result.rotation_angle
    )
    save_image(overlay, "overlay.png")
```

---

## Error Handling

### Pattern: Safe Matching

```python
from template_matcher import TemplateMatcher
from template_matcher.utils import load_image_with_transparency, load_image

try:
    template = load_image_with_transparency("template.png")
    image = load_image("scene.jpg")
    
    matcher = TemplateMatcher()
    result = matcher.match(template, image, min_matches=15)
    
    if not result.success:
        print(f"Detection failed: only {result.num_matches} matches found")
    elif result.confidence < 0.5:
        print(f"Low confidence result: {result.confidence:.2f}")
    else:
        print(f"Success: ({result.x:.1f}, {result.y:.1f}) @ {result.rotation_angle:.1f}°")
        
except FileNotFoundError as e:
    print(f"Could not load image: {e}")
except Exception as e:
    print(f"Matching error: {e}")
```

---

## Performance Considerations

### Time Complexity

- **ORB**: O(n) where n = image size (fast)
- **AKAZE**: O(n log n) (medium)
- **SIFT**: O(n²) in worst case (slow but most accurate)

### Memory Usage

- **ORB**: ~50 MB for 1000x1000 image
- **AKAZE**: ~80 MB for 1000x1000 image
- **SIFT**: ~150 MB for 1000x1000 image

### Optimization Tips

1. **Reduce image size** before matching
2. **Use ORB** for speed-critical applications
3. **Cache detector** if processing multiple images
4. **Parallelize** batch processing across cores

---

## Debugging

### Print Detection Info

```python
result = matcher.match(template, image)
print(f"Success: {result.success}")
print(f"Method: {result.method}")
print(f"Position: ({result.x}, {result.y})")
print(f"Rotation: {result.rotation_angle}°")
print(f"Confidence: {result.confidence}")
print(f"Matches: {result.num_matches}")
```

### Enable Logging

```python
import logging
logging.basicConfig(level=logging.DEBUG)

# Now run matching - will see debug output
result = matcher.match(template, image)
```

---

## Version History

- **0.1.0** (2024-05-11): Initial release with ORB, SIFT, AKAZE support
