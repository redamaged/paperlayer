#!/usr/bin/env python3
"""
Comprehensive test and demo script for template matching with rotation estimation.

This script demonstrates:
- Loading template (with transparent background) and larger image
- Using all three detection methods (ORB, SIFT, AKAZE)
- Plotting results
- Logging position and rotation values
"""

import sys
import os
import numpy as np
import cv2
import logging
from pathlib import Path
from typing import Optional

# Add parent directory to path
sys.path.insert(0, str(Path(__file__).parent.parent))

from template_matcher import TemplateMatcher, DetectionMethod
from template_matcher.utils import (
    load_image_with_transparency,
    load_image,
    save_image,
    overlay_template_on_image,
    combine_images_horizontal
)

# Configure logging
logging.basicConfig(
    level=logging.INFO,
    format='%(asctime)s - %(levelname)s - %(message)s',
    handlers=[
        logging.StreamHandler(),
        logging.FileHandler('template_matching.log')
    ]
)
logger = logging.getLogger(__name__)


def create_synthetic_template(size: int = 100) -> np.ndarray:
    """
    Create a synthetic template with transparency for testing.
    
    Args:
        size: Template size
        
    Returns:
        BGRA image with transparent background
    """
    # Create transparent background
    template = np.zeros((size, size, 4), dtype=np.uint8)
    
    # Draw multiple colored shapes for more features
    # Large circle
    cv2.circle(template, (size // 2, size // 2), size // 3, (0, 255, 0, 255), -1)
    
    # Rectangle outline
    cv2.rectangle(template, (size // 4, size // 4), (3 * size // 4, 3 * size // 4), (255, 0, 0, 255), 3)
    
    # Small inner circle
    cv2.circle(template, (size // 2, size // 2), size // 6, (255, 255, 0, 255), -1)
    
    # Diagonal lines
    cv2.line(template, (0, 0), (size, size), (255, 0, 255, 255), 2)
    cv2.line(template, (size, 0), (0, size), (0, 255, 255, 255), 2)
    
    # Horizontal and vertical lines
    cv2.line(template, (size // 2, 0), (size // 2, size), (128, 128, 255, 255), 1)
    cv2.line(template, (0, size // 2), (size, size // 2), (255, 128, 128, 255), 1)
    
    # Corner squares
    cv2.rectangle(template, (0, 0), (size // 5, size // 5), (255, 255, 255, 255), -1)
    cv2.rectangle(template, (size - size // 5, 0), (size, size // 5), (0, 0, 0, 255), -1)
    cv2.rectangle(template, (0, size - size // 5), (size // 5, size), (128, 0, 128, 255), -1)
    cv2.rectangle(template, (size - size // 5, size - size // 5), (size, size), (0, 128, 128, 255), -1)
    
    # Add some texture with small circles
    for i in range(10):
        x = np.random.randint(size // 6, 5 * size // 6)
        y = np.random.randint(size // 6, 5 * size // 6)
        radius = np.random.randint(2, 5)
        color = (np.random.randint(0, 256), np.random.randint(0, 256), np.random.randint(0, 256), 255)
        cv2.circle(template, (x, y), radius, color, -1)
    
    return template


def create_synthetic_scene(
    template: np.ndarray,
    scene_size: int = 500,
    template_x: float = 200,
    template_y: float = 200,
    rotation_angle: float = 45.0
) -> np.ndarray:
    """
    Create a synthetic scene with the template at specified position and rotation.
    
    Args:
        template: Template image (BGRA)
        scene_size: Size of the scene
        template_x: X position of template center
        template_y: Y position of template center
        rotation_angle: Rotation angle in degrees
        
    Returns:
        BGR scene image
    """
    # Create background
    scene = np.ones((scene_size, scene_size, 3), dtype=np.uint8) * 200
    
    # Add some texture to background
    for i in range(50):
        pt1 = (np.random.randint(0, scene_size), np.random.randint(0, scene_size))
        pt2 = (np.random.randint(0, scene_size), np.random.randint(0, scene_size))
        cv2.line(scene, pt1, pt2, (150, 150, 150), 1)
    
    # Overlay template
    scene = overlay_template_on_image(
        template, scene,
        template_x, template_y,
        rotation_angle,
        alpha=1.0
    )
    
    return scene


def test_method(
    template: np.ndarray,
    image: np.ndarray,
    method: DetectionMethod,
    expected_x: Optional[float] = None,
    expected_y: Optional[float] = None,
    expected_angle: Optional[float] = None,
    min_matches: int = 10
):
    """
    Test a specific detection method.
    
    Args:
        template: Template image
        image: Scene image
        method: Detection method to test
        expected_x: Expected x position (for logging)
        expected_y: Expected y position (for logging)
        expected_angle: Expected rotation angle (for logging)
        min_matches: Minimum matches required
    """
    logger.info(f"\n{'='*60}")
    logger.info(f"Testing method: {method.value.upper()}")
    logger.info(f"{'='*60}")
    
    # Create matcher
    matcher = TemplateMatcher(method=method)
    
    # Run matching
    result = matcher.match(template, image, min_matches=min_matches)
    
    # Log results
    logger.info(f"Success: {result.success}")
    logger.info(f"Position: x={result.x:.2f}, y={result.y:.2f}")
    logger.info(f"Rotation: {result.rotation_angle:.2f}°")
    logger.info(f"Confidence: {result.confidence:.4f}")
    logger.info(f"Number of matches: {result.num_matches}")
    
    if expected_x is not None and expected_y is not None and expected_angle is not None:
        error_x = abs(result.x - expected_x)
        error_y = abs(result.y - expected_y)
        # Handle angle wrapping
        angle_diff = abs(result.rotation_angle - expected_angle)
        if angle_diff > 180:
            angle_diff = 360 - angle_diff
        
        logger.info(f"\nExpected: x={expected_x:.2f}, y={expected_y:.2f}, angle={expected_angle:.2f}°")
        logger.info(f"Error: x={error_x:.2f}, y={error_y:.2f}, angle={angle_diff:.2f}°")
    
    return result, matcher


def test_all_methods_interactive(template_path: str, image_path: str) -> None:
    """
    Test all methods on provided images with interactive visualization.
    
    Args:
        template_path: Path to template image file
        image_path: Path to scene image file
    """
    logger.info(f"\nLoading template from: {template_path}")
    logger.info(f"Loading image from: {image_path}")
    
    # Load images
    template = load_image_with_transparency(template_path)
    image = load_image(image_path)
    
    logger.info(f"Template size: {template.shape}")
    logger.info(f"Image size: {image.shape}")
    
    results = {}
    matchers = {}
    visualizations = {}
    
    # Test all methods
    for method in DetectionMethod:
        logger.info(f"\n{'='*60}")
        logger.info(f"Testing: {method.value.upper()}")
        logger.info(f"{'='*60}")
        
        matcher = TemplateMatcher(method=method)
        result, vis = matcher.match_with_visualization(template, image, draw_matches=True)
        
        results[method] = result
        matchers[method] = matcher
        visualizations[method] = vis
        
        logger.info(f"Success: {result.success}")
        logger.info(f"Position: ({result.x:.2f}, {result.y:.2f})")
        logger.info(f"Rotation: {result.rotation_angle:.2f}°")
        logger.info(f"Confidence: {result.confidence:.4f}")
        logger.info(f"Matches: {result.num_matches}")
    
    # Display results
    logger.info(f"\n{'='*60}")
    logger.info("SUMMARY")
    logger.info(f"{'='*60}")
    for method, result in results.items():
        logger.info(f"{method.value.upper()}: "
                   f"({'SUCCESS' if result.success else 'FAILED'}) - "
                   f"Pos: ({result.x:.1f}, {result.y:.1f}), "
                   f"Rot: {result.rotation_angle:.1f}°, "
                   f"Conf: {result.confidence:.2f}")


def test_with_synthetic_data(show_visualization: bool = True) -> None:
    """
    Test all methods with synthetic data.
    
    Args:
        show_visualization: Whether to display visualizations
    """
    logger.info("\n" + "="*60)
    logger.info("SYNTHETIC DATA TEST")
    logger.info("="*60)
    
    # Create synthetic data
    logger.info("\nGenerating synthetic template and scene...")
    template = create_synthetic_template(size=80)
    
    # Test parameters
    expected_x = 250.0
    expected_y = 280.0
    expected_angle = 35.0
    
    scene = create_synthetic_scene(
        template, scene_size=500,
        template_x=expected_x,
        template_y=expected_y,
        rotation_angle=expected_angle
    )
    
    logger.info(f"Scene size: {scene.shape}")
    logger.info(f"Template placed at: ({expected_x}, {expected_y}), rotation: {expected_angle}°")
    
    # Test all methods
    for method in DetectionMethod:
        result, matcher = test_method(
            template, scene, method,
            expected_x, expected_y, expected_angle,
            min_matches=8  # Synthetic data might have fewer matches
        )
        
        if show_visualization and result.success:
            vis = matcher._draw_result(template, scene, result)
            
            # Save visualization
            output_path = f"/tmp/template_match_{method.value}.png"
            save_image(vis, output_path)
            logger.info(f"Saved visualization to: {output_path}")


def main():
    """Main test function."""
    logger.info("\n" + "="*80)
    logger.info("TEMPLATE MATCHER - ROTATION ESTIMATION TEST")
    logger.info("="*80)
    
    # Test 1: Synthetic data
    logger.info("\n[TEST 1] Synthetic Data Test")
    test_with_synthetic_data(show_visualization=True)
    
    # Test 2: User-provided images (if they exist)
    template_file = None
    image_file = None
    
    # Look for sample files in common locations
    possible_template_paths = [
        "./template.png",
        "./template.jpg",
        "./Data/template.png",
        "../Data/template.png",
    ]
    
    possible_image_paths = [
        "./image.png",
        "./image.jpg",
        "./scene.png",
        "./Data/image.png",
        "../Data/image.png",
    ]
    
    for path in possible_template_paths:
        if os.path.exists(path):
            template_file = path
            break
    
    for path in possible_image_paths:
        if os.path.exists(path):
            image_file = path
            break
    
    if template_file and image_file:
        logger.info("\n[TEST 2] User-Provided Images Test")
        logger.info(f"Found template: {template_file}")
        logger.info(f"Found image: {image_file}")
        test_all_methods_interactive(template_file, image_file)
    else:
        logger.info("\n[TEST 2] User-Provided Images Test - SKIPPED")
        logger.info("No template and image files found in expected locations.")
        logger.info("Place 'template.png' and 'image.png' in the current directory to test.")
    
    logger.info("\n" + "="*80)
    logger.info("TEST COMPLETED")
    logger.info("="*80)
    logger.info("Log file saved to: template_matching.log")


if __name__ == "__main__":
    main()
