"""
Utility functions for image handling and visualization.
"""

from typing import Optional, Tuple
import cv2
import numpy as np


def load_image_with_transparency(image_path: str) -> np.ndarray:
    """
    Load an image, preserving alpha channel if present.
    
    Args:
        image_path: Path to the image file
        
    Returns:
        Image array (BGR or BGRA)
    """
    image = cv2.imread(image_path, cv2.IMREAD_UNCHANGED)
    if image is None:
        raise ValueError(f"Could not load image: {image_path}")
    return image


def load_image(image_path: str) -> np.ndarray:
    """
    Load an image as BGR.
    
    Args:
        image_path: Path to the image file
        
    Returns:
        BGR image array
    """
    image = cv2.imread(image_path, cv2.IMREAD_COLOR)
    if image is None:
        raise ValueError(f"Could not load image: {image_path}")
    return image


def save_image(image: np.ndarray, output_path: str) -> None:
    """
    Save an image to file.
    
    Args:
        image: Image array
        output_path: Path to save the image
    """
    cv2.imwrite(output_path, image)


def resize_image(image: np.ndarray, max_size: int = 1000) -> np.ndarray:
    """
    Resize image if it's larger than max_size.
    
    Args:
        image: Input image
        max_size: Maximum width/height
        
    Returns:
        Resized image
    """
    h, w = image.shape[:2]
    if max(h, w) > max_size:
        scale = max_size / max(h, w)
        new_w = int(w * scale)
        new_h = int(h * scale)
        return cv2.resize(image, (new_w, new_h))
    return image


def display_image(image: np.ndarray, title: str = "Image", wait_ms: int = 0) -> None:
    """
    Display an image in a window.
    
    Args:
        image: Image to display
        title: Window title
        wait_ms: How long to wait in milliseconds (0 = forever, until key pressed)
    """
    cv2.imshow(title, image)
    cv2.waitKey(wait_ms)


def create_checkerboard_bg(template: np.ndarray, bg_color: Tuple[int, int, int] = (200, 200, 200)) -> np.ndarray:
    """
    Create a visualization of template on checkerboard background.
    
    Args:
        template: Template image (can have alpha)
        bg_color: RGB background color
        
    Returns:
        Image with template on solid background
    """
    if template.shape[2] != 4:
        return template
    
    h, w = template.shape[:2]
    background = np.full((h, w, 3), bg_color, dtype=np.uint8)
    
    # Blend using alpha
    alpha = template[:, :, 3:4] / 255.0
    for c in range(3):
        background[:, :, c] = (template[:, :, c] * alpha[:, :, 0] + 
                               background[:, :, c] * (1 - alpha[:, :, 0])).astype(np.uint8)
    
    return background


def combine_images_horizontal(img1: np.ndarray, img2: np.ndarray) -> np.ndarray:
    """
    Combine two images horizontally.
    
    Args:
        img1: First image
        img2: Second image
        
    Returns:
        Combined image
    """
    # Ensure same height
    h1, w1 = img1.shape[:2]
    h2, w2 = img2.shape[:2]
    
    if h1 != h2:
        if h1 > h2:
            img2 = cv2.copyMakeBorder(img2, 0, h1 - h2, 0, 0, cv2.BORDER_CONSTANT, value=(0, 0, 0))
        else:
            img1 = cv2.copyMakeBorder(img1, 0, h2 - h1, 0, 0, cv2.BORDER_CONSTANT, value=(0, 0, 0))
    
    return np.hstack([img1, img2])


def overlay_template_on_image(
    template: np.ndarray,
    image: np.ndarray,
    x: float,
    y: float,
    rotation_angle: float = 0.0,
    alpha: float = 0.5
) -> np.ndarray:
    """
    Overlay a rotated template on an image.
    
    Args:
        template: Template image
        image: Target image
        x: X coordinate of template center
        y: Y coordinate of template center
        rotation_angle: Rotation angle in degrees
        alpha: Blending alpha (0-1)
        
    Returns:
        Blended image
    """
    output = image.copy()
    
    # Convert template if needed
    if template.shape[2] == 4:
        template_rgb = cv2.cvtColor(template, cv2.COLOR_BGRA2BGR)
        mask = template[:, :, 3]
    else:
        template_rgb = template
        mask = np.ones_like(template[:, :, 0]) * 255
    
    h, w = template_rgb.shape[:2]
    
    # Rotate template
    center = (w / 2, h / 2)
    M = cv2.getRotationMatrix2D(center, rotation_angle, 1.0)
    rotated = cv2.warpAffine(template_rgb, M, (w, h))
    rotated_mask = cv2.warpAffine(mask, M, (w, h))
    
    # Place on output image
    x_start = int(x - w / 2)
    y_start = int(y - h / 2)
    x_end = x_start + w
    y_end = y_start + h
    
    # Clip to image bounds
    x_start_clipped = max(0, x_start)
    y_start_clipped = max(0, y_start)
    x_end_clipped = min(output.shape[1], x_end)
    y_end_clipped = min(output.shape[0], y_end)
    
    x_start_rot = x_start_clipped - x_start
    y_start_rot = y_start_clipped - y_start
    x_end_rot = x_start_rot + (x_end_clipped - x_start_clipped)
    y_end_rot = y_start_rot + (y_end_clipped - y_start_clipped)
    
    if x_end_clipped > x_start_clipped and y_end_clipped > y_start_clipped:
        mask_clipped = rotated_mask[y_start_rot:y_end_rot, x_start_rot:x_end_rot] / 255.0
        for c in range(3):
            output[y_start_clipped:y_end_clipped, x_start_clipped:x_end_clipped, c] = (
                rotated[y_start_rot:y_end_rot, x_start_rot:x_end_rot, c] * mask_clipped * alpha +
                output[y_start_clipped:y_end_clipped, x_start_clipped:x_end_clipped, c] * (1 - mask_clipped * alpha)
            ).astype(np.uint8)
    
    return output
