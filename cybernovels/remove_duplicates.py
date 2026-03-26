import re
import sys
import os

def remove_duplicate_sentences(text):
    # Split text into paragraphs based on two or more newlines
    paragraphs = re.split(r'\n{2,}', text)
    cleaned_paragraphs = []
    
    for paragraph in paragraphs:
        # Split paragraph into sentences. Retain delimiters for reconstruction.
        # This regex tries to split by . ! ? followed by whitespace or end of string,
        # but also tries to handle abbreviations and decimals by looking ahead.
        # This is a simplified approach, a more robust solution would use a proper NLP library.
        sentences = re.split(r'(?<=[.!?])\s+(?=[A-Z])|\n(?=[A-Z])', paragraph)
        
        seen_sentences = set()
        unique_sentences = []

        for sentence in sentences:
            stripped_sentence = sentence.strip()
            if not stripped_sentence:
                continue

            # Normalize sentence for comparison (lowercase, remove non-alphanumeric except punctuation)
            # Remove punctuation except for sentence-ending ones for better comparison
            normalized_sentence = re.sub(r'[^\w\s.!?]', '', stripped_sentence.lower())
            
            if normalized_sentence not in seen_sentences:
                seen_sentences.add(normalized_sentence)
                unique_sentences.append(sentence)
        
        if unique_sentences:
            cleaned_paragraphs.append(" ".join(unique_sentences)) # Rejoin sentences in a paragraph
    
    return "\n\n".join(cleaned_paragraphs) # Rejoin paragraphs with double newline

if __name__ == "__main__":
    if len(sys.argv) != 2:
        print("Usage: python remove_duplicates.py <file_path>")
        sys.exit(1)

    file_path = sys.argv[1]
    if not os.path.exists(file_path):
        print(f"Error: File not found at {file_path}")
        sys.exit(1)

    with open(file_path, 'r', encoding='utf-8') as f:
        content = f.read()

    cleaned_content = remove_duplicate_sentences(content)
    print(cleaned_content)