#!/usr/bin/env python3
"""
Practical examples of template matching usage.
"""

import sys
from pathlib import Path

sys.path.insert(0, str(Path(__file__).parent.parent))

from template_matcher import TemplateMatcher, DetectionMethod
from template_matcher.utils import (
    load_image_with_transparency,
    load_image,
    save_image,
    overlay_template_on_image
)


def example_1_basic_matching():
    """Example 1: Basic template matching with default SIFT method."""
    print("\n" + "="*60)
    print("Example 1: Basic Template Matching")
    print("="*60)
    
    # Load images (you should replace these with your actual files)
    try:
        template = load_image_with_transparency("template.png")
        image = load_image("scene.png")
    except Exception as e:
        print(f"Could not load images: {e}")
        print("Please provide 'template.png' and 'scene.png' files")
        return
    
    # Create matcher with default SIFT method
    matcher = TemplateMatcher()
    
    # Match template
    result = matcher.match(template, image)
    
    # Print results
    print(f"\nDetection successful: {result.success}")
    print(f"Position: x={result.x:.2f}, y={result.y:.2f}")
    print(f"Rotation: {result.rotation_angle:.2f}°")
    print(f"Confidence: {result.confidence:.4f}")
    print(f"Matched features: {result.num_matches}")


def example_2_compare_methods():
    """Example 2: Compare all three detection methods."""
    print("\n" + "="*60)
    print("Example 2: Compare Detection Methods")
    print("="*60)
    
    try:
        template = load_image_with_transparency("template.png")
        image = load_image("scene.png")
    except Exception as e:
        print(f"Could not load images: {e}")
        return
    
    matcher = TemplateMatcher()
    
    results = {}
    
    # Test each method
    for method in DetectionMethod:
        matcher.set_method(method)
        result = matcher.match(template, image)
        results[method] = result
        
        print(f"\n{method.value.upper()}:")
        print(f"  Position: ({result.x:.2f}, {result.y:.2f})")
        print(f"  Rotation: {result.rotation_angle:.2f}°")
        print(f"  Confidence: {result.confidence:.4f}")
        print(f"  Matches: {result.num_matches}")
    
    # Find best result
    best_method = max(results, key=lambda m: results[m].confidence if results[m].success else 0)
    print(f"\nBest result: {best_method.value.upper()} with confidence {results[best_method].confidence:.4f}")


def example_3_with_visualization():
    """Example 3: Get visualization of detection."""
    print("\n" + "="*60)
    print("Example 3: Visualization")
    print("="*60)
    
    try:
        template = load_image_with_transparency("template.png")
        image = load_image("scene.png")
    except Exception as e:
        print(f"Could not load images: {e}")
        return
    
    matcher = TemplateMatcher(DetectionMethod.SIFT)
    
    # Get result with visualization
    result, visualization = matcher.match_with_visualization(template, image, draw_matches=True)
    
    if visualization is not None:
        save_image(visualization, "detection_result.png")
        print("\nVisualization saved to: detection_result.png")
    else:
        print("\nDetection failed, no visualization generated")


def example_4_overlay_result():
    """Example 4: Overlay detected template on image."""
    print("\n" + "="*60)
    print("Example 4: Overlay Result")
    print("="*60)
    
    try:
        template = load_image_with_transparency("template.png")
        image = load_image("scene.png")
    except Exception as e:
        print(f"Could not load images: {e}")
        return
    
    matcher = TemplateMatcher()
    result = matcher.match(template, image)
    
    if result.success:
        # Overlay the template at detected position
        overlay = overlay_template_on_image(
            template, image,
            x=result.x,
            y=result.y,
            rotation_angle=result.rotation_angle,
            alpha=0.6
        )
        
        save_image(overlay, "overlay_result.png")
        print("Overlay saved to: overlay_result.png")
    else:
        print("Detection failed, could not create overlay")


def example_5_batch_processing():
    """Example 5: Process multiple images."""
    print("\n" + "="*60)
    print("Example 5: Batch Processing")
    print("="*60)
    
    import glob
    
    try:
        template = load_image_with_transparency("template.png")
    except Exception as e:
        print(f"Could not load template: {e}")
        return
    
    # Find all images
    image_files = glob.glob("scenes/*.png") + glob.glob("scenes/*.jpg")
    
    if not image_files:
        print("No images found in 'scenes/' directory")
        return
    
    matcher = TemplateMatcher(DetectionMethod.SIFT)
    results_summary = []
    
    for image_file in image_files:
        try:
            image = load_image(image_file)
            result = matcher.match(template, image)
            
            results_summary.append({
                'file': image_file,
                'success': result.success,
                'x': result.x,
                'y': result.y,
                'rotation': result.rotation_angle,
                'confidence': result.confidence
            })
            
            print(f"{image_file}: "
                  f"{'SUCCESS' if result.success else 'FAILED'} - "
                  f"({result.x:.1f}, {result.y:.1f}) "
                  f"@ {result.rotation_angle:.1f}°")
        except Exception as e:
            print(f"{image_file}: ERROR - {e}")
    
    print(f"\nProcessed {len(results_summary)} images")


def example_6_tune_min_matches():
    """Example 6: Adjust minimum matches threshold."""
    print("\n" + "="*60)
    print("Example 6: Tuning Minimum Matches")
    print("="*60)
    
    try:
        template = load_image_with_transparency("template.png")
        image = load_image("scene.png")
    except Exception as e:
        print(f"Could not load images: {e}")
        return
    
    matcher = TemplateMatcher()
    
    # Try different minimum match thresholds
    for min_matches in [5, 10, 20, 50]:
        result = matcher.match(template, image, min_matches=min_matches)
        
        print(f"\nmin_matches={min_matches}:")
        print(f"  Success: {result.success}")
        if result.success:
            print(f"  Position: ({result.x:.2f}, {result.y:.2f})")
            print(f"  Rotation: {result.rotation_angle:.2f}°")
            print(f"  Confidence: {result.confidence:.4f}")
        print(f"  Matched: {result.num_matches}")


if __name__ == "__main__":
    print("Template Matcher - Practical Examples")
    
    # Run examples (comment out those you don't want to run)
    example_1_basic_matching()
    example_2_compare_methods()
    example_3_with_visualization()
    example_4_overlay_result()
    example_5_batch_processing()
    example_6_tune_min_matches()
    
    print("\n" + "="*60)
    print("Examples completed!")
    print("="*60)
