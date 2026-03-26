# Gem Tutorial 04: Image Processing
# ---------------------------------

# Note: This uses the integrated ImageMagick source code via Magick++

# 1. Image Info
# Create a dummy image first via shell for demo if needed
# sys.exec("magick -size 100x100 xc:blue input.jpg")

# info = img.info("input.jpg")
# sys.print("Format:", info.format)
# sys.print("Dimensions:", info.width, "x", info.height)

# 2. Resizing and Grayscale
# img.resize("input.jpg", 50, 50, "small.jpg")
# img.grayscale("small.jpg", "small_gray.jpg")

# 3. Video Processing (requires ffmpeg)
# img.to_video("animated.gif", "output.mp4")
# img.extract_frame("output.mp4", 5.0, "frame_at_5s.jpg")

sys.print("Image processing demo ready. (Requires input files)")
