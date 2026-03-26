"""
Native conversion library for Python.
Uses markdown-it-py for Markdown -> HTML, and WeasyPrint for HTML -> PDF.
"""
from markdown_it import MarkdownIt
from weasyprint import HTML
import os

def convert_md_to_html(in_path, out_path):
    """Converts Markdown to HTML."""
    md = MarkdownIt()
    with open(in_path, 'r', encoding='utf-8') as f:
        text = f.read()
    html_content = md.render(text)
    with open(out_path, 'w', encoding='utf-8') as f:
        f.write(html_content)

def convert_html_to_pdf(in_path, out_path):
    """Converts HTML to PDF."""
    HTML(in_path).write_pdf(out_path)

def convert_file(input_path, output_path):
    """
    Selects the correct conversion path based on file extensions.
    """
    in_ext = os.path.splitext(input_path)[1].lower()
    out_ext = os.path.splitext(output_path)[1].lower()

    print(f"Native conversion from '{in_ext}' to '{out_ext}'")

    if in_ext == '.md' and out_ext == '.html':
        convert_md_to_html(input_path, output_path)
    elif in_ext == '.html' and out_ext == '.pdf':
        convert_html_to_pdf(input_path, output_path)
    elif in_ext == '.md' and out_ext == '.pdf':
        # Chain conversion: md -> html -> pdf
        html_temp_path = os.path.splitext(input_path)[0] + ".temp.html"
        try:
            print("Step 1: md -> html")
            convert_md_to_html(input_path, html_temp_path)
            print("Step 2: html -> pdf")
            convert_html_to_pdf(html_temp_path, output_path)
        finally:
            # Clean up temporary file
            if os.path.exists(html_temp_path):
                os.remove(html_temp_path)
    else:
        raise NotImplementedError(f"Native conversion from {in_ext} to {out_ext} is not supported.")
