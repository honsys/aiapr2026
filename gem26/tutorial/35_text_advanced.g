# Gem Tutorial 35: Advanced Text & Document Formats
# This tutorial demonstrates the expanded 'text' module for multiple formats.

sys.print("--- Gem Advanced Text Tutorial ---")

# 1. Markdown & Plain Text
# Gem supports reading and writing standard markdown files.
text.write_markdown("demo.md", "# Gem Documentation\n- High performance\n- STEM focused")
md_content = text.read_markdown("demo.md")
sys.print("\nMarkdown written to demo.md")

# 2. YAML, HTML, and XML
# Support for common structured data and web formats.
text.write_html("page.html", "<h1>Gem Language</h1><p>Modern C++26</p>")
text.write_xml("config.xml", "<gem><version>1.0</version></gem>")
# yaml_obj = text.read_yaml("data.yaml")

# 3. Archival PDF (PDF/A)
# Unlike standard PDF, PDF/A is optimized for long-term preservation.
if text.write_pdf_a("archival_report.pdf", "Archival Quality Report")
    sys.print("PDF/A generated successfully.")
end

# 4. Scientific Headers (FITS & HDF)
# Extract metadata headers from large scientific data files.
fits = text.read_fits_header("scientific_obs.fits")
sys.print("\nFITS BITPIX:", fits.BITPIX)
sys.print("FITS SIMPLE:", fits.SIMPLE)

hdf = text.read_hdf_header("dataset.h5")
sys.print("HDF Header object created.")

# 5. String Manipulation (Reminders)
sample = "The Gem Language"
sys.print("\nSubstring:", text.sub(sample, 4, 3))
sys.print("Replace:", text.replace(sample, "Gem", "Ruby"))

sys.print("\nTutorial 35: Advanced text formats documented.")
