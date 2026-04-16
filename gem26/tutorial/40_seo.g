# Gem Tutorial 40: SEO — Search Engine Optimization Indexer
# ----------------------------------------------------------
# Covers: seo.index(), seo.analyze()
# Signals: title, description, keywords, og tags, canonical,
#          word count, image alt coverage, internal/external links, headings

sys.print("--- Gem SEO Tutorial ---")

# 1. Index a list of URLs — crawls each page and extracts SEO signals
string urls = ["https://example.com", "https://example.com/about"]
string index_file = "seo_index.json"

seo.index(urls, index_file)
sys.print("Index written →", index_file)

# 2. Analyze the index — prints a human-readable SEO report
seo.analyze(index_file)

# 3. Single URL shorthand
seo.index("https://example.com", "single_page.json")
seo.analyze("single_page.json")

sys.print("Tutorial 40 complete.")
