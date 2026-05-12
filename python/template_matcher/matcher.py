"""
Core template matching and rotation estimation module.
"""

from enum import Enum
from typing import Tuple, Optional, Dict, List, Any
import numpy as np
import cv2
from dataclasses import dataclass
import math


class DetectionMethod(Enum):
    """Available feature detection methods."""
    ORB = "orb"
    SIFT = "sift"
    AKAZE = "akaze"


@dataclass
class MatchResult:
    """Result of template matching."""
    x: float
    y: float
    rotation_angle: float  # in degrees
    confidence: float  # 0-1
    num_matches: int
    method: str
    success: bool


class TemplateMatcher:
    """Template matching with rotation estimation using feature detection."""
    
    def __init__(self, method: DetectionMethod = DetectionMethod.SIFT):
        """
        Initialize the template matcher.
        
        Args:
            method: Detection method to use (ORB, SIFT, or AKAZE)
        """
        self.method = method
        self.detector = self._create_detector()
        self.matcher = self._create_matcher()
        
    def _create_detector(self):
        """Create the appropriate feature detector."""
        if self.method == DetectionMethod.ORB:
            return cv2.ORB_create(nfeatures=5000)
        elif self.method == DetectionMethod.SIFT:
            return cv2.SIFT_create()
        elif self.method == DetectionMethod.AKAZE:
            return cv2.AKAZE_create()
        else:
            raise ValueError(f"Unknown detection method: {self.method}")
    
    def _create_matcher(self):
        """Create the appropriate feature matcher."""
        if self.method == DetectionMethod.SIFT:
            # SIFT uses FLANN matcher
            FLANN_INDEX_KDTREE = 1
            index_params = dict(algorithm=FLANN_INDEX_KDTREE, trees=5)
            search_params = dict(checks=50)
            return cv2.FlannBasedMatcher(index_params, search_params)
        else:
            # ORB and AKAZE use BFMatcher
            return cv2.BFMatcher(cv2.NORM_HAMMING, crossCheck=False)
    
    def set_method(self, method: DetectionMethod) -> None:
        """
        Switch to a different detection method.
        
        Args:
            method: New detection method
        """
        self.method = method
        self.detector = self._create_detector()
        self.matcher = self._create_matcher()
    
    def _extract_mask_from_template(self, template: np.ndarray) -> np.ndarray:
        """
        Extract alpha channel as mask from template with transparent background.
        
        Args:
            template: Template image (with or without alpha channel)
            
        Returns:
            Binary mask where template pixels are white, background is black
        """
        if template.shape[2] == 4:  # RGBA
            alpha = template[:, :, 3]
            mask = cv2.threshold(alpha, 127, 255, cv2.THRESH_BINARY)[1]
            return mask.astype(np.uint8)
        else:
            # If no alpha channel, use the whole image
            gray = cv2.cvtColor(template, cv2.COLOR_BGR2GRAY)
            return cv2.threshold(gray, 1, 255, cv2.THRESH_BINARY)[1]
    
    def _convert_template_rgb(self, template: np.ndarray) -> np.ndarray:
        """
        Convert template to RGB, removing alpha channel if present.
        
        Args:
            template: Template image
            
        Returns:
            RGB template image
        """
        if template.shape[2] == 4:  # RGBA
            return cv2.cvtColor(template, cv2.COLOR_BGRA2BGR)
        return template
    
    def match(
        self,
        template: np.ndarray,
        image: np.ndarray,
        min_matches: int = 10
    ) -> MatchResult:
        """
        Detect template in image and estimate rotation.
        
        Args:
            template: Template image (can have transparent background)
            image: Larger image to search in
            min_matches: Minimum number of matches required
            
        Returns:
            MatchResult containing position (x, y), rotation angle, and confidence
        """
        try:
            # Convert template if it has alpha channel
            template_rgb = self._convert_template_rgb(template)
            template_mask = self._extract_mask_from_template(template)
            
            # Convert both to grayscale for feature detection
            template_gray = cv2.cvtColor(template_rgb, cv2.COLOR_BGR2GRAY)
            image_gray = cv2.cvtColor(image, cv2.COLOR_BGR2GRAY)
            
            # Detect keypoints and compute descriptors
            kp_template, desc_template = self.detector.detectAndCompute(
                template_gray, template_mask
            )
            kp_image, desc_image = self.detector.detectAndCompute(image_gray, None)
            
            if desc_template is None or desc_image is None:
                return MatchResult(
                    x=0, y=0, rotation_angle=0,
                    confidence=0, num_matches=0,
                    method=self.method.value,
                    success=False
                )
            
            # Match descriptors
            if self.method == DetectionMethod.SIFT:
                matches = self.matcher.knnMatch(desc_template, desc_image, k=2)
                # Apply Lowe's ratio test
                good_matches = []
                for match_pair in matches:
                    if len(match_pair) == 2:
                        m, n = match_pair
                        if m.distance < 0.75 * n.distance:
                            good_matches.append(m)
            else:
                matches = self.matcher.knnMatch(desc_template, desc_image, k=2)
                good_matches = []
                for match_pair in matches:
                    if len(match_pair) == 2:
                        m, n = match_pair
                        if m.distance < 0.75 * n.distance:
                            good_matches.append(m)
            
            if len(good_matches) < min_matches:
                return MatchResult(
                    x=0, y=0, rotation_angle=0,
                    confidence=0, num_matches=len(good_matches),
                    method=self.method.value,
                    success=False
                )
            
            # Extract matched points
            src_pts = np.float32([kp_template[m.queryIdx].pt for m in good_matches]).reshape(-1, 1, 2)
            dst_pts = np.float32([kp_image[m.trainIdx].pt for m in good_matches]).reshape(-1, 1, 2)
            
            # Find homography
            H, mask = cv2.findHomography(src_pts, dst_pts, cv2.RANSAC, 5.0)
            
            if H is None:
                return MatchResult(
                    x=0, y=0, rotation_angle=0,
                    confidence=0, num_matches=len(good_matches),
                    method=self.method.value,
                    success=False
                )
            
            # Calculate rotation angle and translation from homography
            x, y, rotation_angle = self._extract_transformation(
                H, template_rgb, kp_template, good_matches
            )
            
            # Calculate confidence based on number of matches and inliers
            num_inliers = np.sum(mask)
            confidence = min(1.0, num_inliers / len(good_matches)) if good_matches else 0
            
            return MatchResult(
                x=x, y=y, rotation_angle=rotation_angle,
                confidence=confidence, num_matches=len(good_matches),
                method=self.method.value,
                success=True
            )
            
        except Exception as e:
            print(f"Error during matching: {e}")
            return MatchResult(
                x=0, y=0, rotation_angle=0,
                confidence=0, num_matches=0,
                method=self.method.value,
                success=False
            )
    
    def _extract_transformation(
        self,
        H: np.ndarray,
        template: np.ndarray,
        kp_template: List,
        matches: List
    ) -> Tuple[float, float, float]:
        """
        Extract translation and rotation from homography matrix.
        
        Args:
            H: Homography matrix
            template: Template image
            kp_template: Template keypoints
            matches: List of good matches
            
        Returns:
            Tuple of (x, y, rotation_angle_degrees)
        """
        # Get template corners
        h, w = template.shape[:2]
        template_corners = np.float32([
            [0, 0],
            [w, 0],
            [w, h],
            [0, h]
        ]).reshape(-1, 1, 2)
        
        # Transform template corners to image space
        transformed_corners = cv2.perspectiveTransform(template_corners, H)
        
        # Get center point of transformed template
        center = transformed_corners.mean(axis=0)[0]
        x, y = center[0], center[1]
        
        # Extract rotation angle from homography
        # Using SVD decomposition
        U, s, Vt = np.linalg.svd(H[:2, :2])
        rotation_matrix = U @ Vt
        
        # Handle reflection (determinant check)
        if np.linalg.det(rotation_matrix) < 0:
            Vt[-1, :] *= -1
            rotation_matrix = U @ Vt
        
        # Extract rotation angle
        rotation_angle = np.arctan2(rotation_matrix[1, 0], rotation_matrix[0, 0])
        rotation_angle_deg = np.degrees(rotation_angle)
        
        # Normalize to [0, 360) range
        rotation_angle_deg = rotation_angle_deg % 360
        
        # Convert to [-180, 180] range for convenience
        if rotation_angle_deg > 180:
            rotation_angle_deg -= 360
        
        return float(x), float(y), float(rotation_angle_deg)
    
    def match_with_visualization(
        self,
        template: np.ndarray,
        image: np.ndarray,
        draw_matches: bool = False
    ) -> Tuple[MatchResult, Optional[np.ndarray]]:
        """
        Detect template and optionally return visualization.
        
        Args:
            template: Template image
            image: Larger image
            draw_matches: Whether to draw match visualization
            
        Returns:
            Tuple of (MatchResult, visualization_image or None)
        """
        result = self.match(template, image)
        
        vis_image = None
        if draw_matches and result.success:
            vis_image = self._draw_result(template, image, result)
        
        return result, vis_image
    
    def _draw_result(
        self,
        template: np.ndarray,
        image: np.ndarray,
        result: MatchResult
    ) -> np.ndarray:
        """
        Draw the detected template position and rotation on the image.
        
        Args:
            template: Template image
            image: Source image
            result: Detection result
            
        Returns:
            Image with drawn result
        """
        output = image.copy()
        
        # Convert template if needed
        template_rgb = self._convert_template_rgb(template)
        h, w = template_rgb.shape[:2]
        
        # Create rotation matrix for the template
        center = (w / 2, h / 2)
        M = cv2.getRotationMatrix2D(center, -result.rotation_angle, 1.0)
        
        # Rotate template
        rotated = cv2.warpAffine(template_rgb, M, (w, h))
        
        # Get corners of the rotated template
        corners = np.float32([
            [0, 0],
            [w, 0],
            [w, h],
            [0, h]
        ])
        
        # Apply rotation to corners
        angle_rad = np.radians(-result.rotation_angle)
        cos_a = np.cos(angle_rad)
        sin_a = np.sin(angle_rad)
        
        rotated_corners = []
        for corner in corners:
            x_rel = corner[0] - center[0]
            y_rel = corner[1] - center[1]
            x_rot = x_rel * cos_a - y_rel * sin_a + result.x
            y_rot = x_rel * sin_a + y_rel * cos_a + result.y
            rotated_corners.append([x_rot, y_rot])
        
        rotated_corners = np.array(rotated_corners, dtype=np.int32)
        
        # Draw bounding box
        cv2.polylines(output, [rotated_corners], True, (0, 255, 0), 2)
        
        # Draw center point
        cv2.circle(output, (int(result.x), int(result.y)), 5, (0, 0, 255), -1)
        
        # Draw rotation indicator
        radius = 20
        end_x = int(result.x + radius * np.cos(np.radians(-result.rotation_angle)))
        end_y = int(result.y + radius * np.sin(np.radians(-result.rotation_angle)))
        cv2.arrowedLine(
            output, 
            (int(result.x), int(result.y)),
            (end_x, end_y),
            (255, 0, 0),
            2,
            tipLength=0.3
        )
        
        # Add text info
        text = f"Pos: ({result.x:.1f}, {result.y:.1f}) Rot: {result.rotation_angle:.1f}° Conf: {result.confidence:.2f}"
        cv2.putText(
            output,
            text,
            (10, 30),
            cv2.FONT_HERSHEY_SIMPLEX,
            0.7,
            (0, 255, 0),
            2
        )
        
        return output
