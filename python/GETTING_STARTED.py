#!/usr/bin/env python3
"""
GETTING STARTED - Template Matcher Library

Follow these steps to get up and running in 5 minutes!
"""

print("""
╔══════════════════════════════════════════════════════════════╗
║         TEMPLATE MATCHER - GETTING STARTED GUIDE            ║
╚══════════════════════════════════════════════════════════════╝

📍 LOCATION: /Users/redamaged/Documents/paperlayer/python/

📦 STEP 1: INSTALL DEPENDENCIES
━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━

  cd /Users/redamaged/Documents/paperlayer/python
  pip install -r template_matcher/requirements.txt

  This installs:
  - opencv-python (4.5.0+)
  - numpy (1.19.0+)

✅ STEP 2: VERIFY INSTALLATION
━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━

  python3 -c "from template_matcher import TemplateMatcher; print('✓ Ready to use!')"

🧪 STEP 3: RUN TESTS
━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━

  cd template_matcher/tests
  python test_demo.py

  This will:
  - Generate synthetic test data
  - Test all three detection methods (ORB, SIFT, AKAZE)
  - Compare results
  - Generate visualizations
  - Log results to template_matching.log

  Expected output:
  ✓ ORB detection: Position and rotation
  ✓ SIFT detection: Position and rotation  
  ✓ AKAZE detection: Position and rotation
  ✓ Visualization files generated

📚 STEP 4: REVIEW DOCUMENTATION
━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━

  Start here:
  1. README.md              - Overview and examples
  2. QUICKSTART.md          - Quick start guide
  3. API_REFERENCE.md       - Complete API documentation
  4. QUICK_REFERENCE.md     - One-page cheat sheet
  5. TEMPLATE_PREPARATION.md - How to prepare your template

🚀 STEP 5: WRITE YOUR FIRST CODE
━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━

  Create a file: my_first_match.py

  ─────────────────────────────────────────────────────────────
  from template_matcher import TemplateMatcher
  from template_matcher.utils import load_image_with_transparency, load_image

  # Load your template (PNG with transparency supported)
  template = load_image_with_transparency("template.png")

  # Load your scene image
  image = load_image("scene.jpg")

  # Create matcher (default: SIFT - most accurate)
  matcher = TemplateMatcher()

  # Find template
  result = matcher.match(template, image)

  # Print results
  print(f"Found at: ({result.x:.2f}, {result.y:.2f})")
  print(f"Rotation: {result.rotation_angle:.2f}°")
  print(f"Confidence: {result.confidence:.4f}")
  print(f"Success: {result.success}")

  # Get visualization
  result_vis, vis = matcher.match_with_visualization(template, image, draw_matches=True)
  if vis is not None:
      from template_matcher.utils import save_image
      save_image(vis, "result.png")
  ─────────────────────────────────────────────────────────────

  Run it:
  python my_first_match.py

📂 PROJECT STRUCTURE
━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━

  python/
  ├── template_matcher/           Core library
  │   ├── matcher.py             Main algorithm
  │   ├── utils.py               Image utilities
  │   ├── tests/
  │   │   ├── test_demo.py       Test suite
  │   │   └── examples.py        Examples
  │   └── README.md              Documentation
  │
  ├── README.md                   Main docs (START HERE)
  ├── QUICKSTART.md              Quick start
  ├── API_REFERENCE.md           Full API
  ├── QUICK_REFERENCE.md         Cheat sheet
  └── TEMPLATE_PREPARATION.md   Template tips

🎯 QUICK REFERENCE
━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━

  Import:
    from template_matcher import TemplateMatcher, DetectionMethod
    from template_matcher.utils import load_image_with_transparency, load_image

  Create:
    matcher = TemplateMatcher()

  Switch method:
    matcher.set_method(DetectionMethod.ORB)      # Fast
    matcher.set_method(DetectionMethod.SIFT)     # Accurate
    matcher.set_method(DetectionMethod.AKAZE)    # Balanced

  Match:
    result = matcher.match(template, image)

  Access results:
    result.x              # X position
    result.y              # Y position
    result.rotation_angle # Rotation in degrees
    result.confidence     # Confidence (0-1)
    result.success        # Detection successful?
    result.num_matches    # Number of matched features

  Visualize:
    result, vis = matcher.match_with_visualization(template, image)

⚡ QUICK TESTS
━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━

  Test 1 - Verify Installation:
    cd /Users/redamaged/Documents/paperlayer/python
    python3 -c "from template_matcher import TemplateMatcher; print('OK')"

  Test 2 - Run Full Test Suite:
    cd template_matcher/tests
    python test_demo.py

  Test 3 - Run Examples:
    cd template_matcher/tests
    python examples.py

  Test 4 - Check Imports:
    python3 << 'EOF'
    from template_matcher import TemplateMatcher, DetectionMethod, MatchResult
    from template_matcher.utils import load_image_with_transparency, load_image
    print("✓ All imports working")
    EOF

🔧 TROUBLESHOOTING
━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━

  Problem: ModuleNotFoundError: No module named 'cv2'
  Solution: pip install opencv-python

  Problem: test_demo.py won't run
  Solution: Make sure you're in the correct directory
            cd template_matcher/tests before running

  Problem: Detection returns success: False
  Solution: Template might lack distinctive features
            Try adding more texture or use SIFT instead of ORB

  Problem: Imports not working
  Solution: Make sure you're in the python directory:
            cd /Users/redamaged/Documents/paperlayer/python

📖 NEXT STEPS
━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━

  1. ✓ Install dependencies
  2. ✓ Run test_demo.py
  3. ✓ Read QUICKSTART.md
  4. ✓ Try examples.py
  5. ✓ Write your own code
  6. ✓ Prepare your template
  7. ✓ Process your images

🎓 LEARNING PATH
━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━

  Beginner:
    1. README.md
    2. QUICKSTART.md
    3. Run test_demo.py
    4. Try basic example

  Intermediate:
    1. API_REFERENCE.md
    2. Run examples.py
    3. Try all three methods
    4. Prepare your template

  Advanced:
    1. Study matcher.py
    2. Review utils.py
    3. Customize parameters
    4. Batch processing

💡 TIPS
━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━

  • Use SIFT for best accuracy
  • Use ORB for speed
  • Use AKAZE for balance
  • Template should have 50-150 pixels size
  • Template should have distinctive features
  • Start with test_demo.py to verify installation
  • Check template_matching.log for detailed results
  • Use match_with_visualization() for debugging

🎉 YOU'RE READY!
━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━

  Your template matching library is ready to use!

  Next: Create your first script or run the examples:
    cd python/template_matcher/tests
    python examples.py

  Questions? Check:
    • README.md
    • API_REFERENCE.md  
    • QUICK_REFERENCE.md
    • TEMPLATE_PREPARATION.md

  Have fun with template matching! 🚀

═══════════════════════════════════════════════════════════════
""")

# Now show directory structure
print("\n📁 CURRENT STRUCTURE:")
print("=" * 60)

import os
import sys

python_dir = "/Users/redamaged/Documents/paperlayer/python"
for root, dirs, files in os.walk(python_dir):
    level = root.replace(python_dir, '').count(os.sep)
    indent = ' ' * 2 * level
    print(f'{indent}{os.path.basename(root)}/')
    
    subindent = ' ' * 2 * (level + 1)
    for file in sorted(files):
        if not file.startswith('.'):
            size = os.path.getsize(os.path.join(root, file))
            if size > 1024*100:  # > 100KB
                print(f'{subindent}{file} ({size//1024}KB)')
            elif size > 1024:    # > 1KB
                print(f'{subindent}{file} ({size}B)')
            else:
                print(f'{subindent}{file}')
    
    if level > 2:  # Don't go too deep
        break

print("\n" + "=" * 60)
print("\n✅ Installation complete! You're ready to start!")
