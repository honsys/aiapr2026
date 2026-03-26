import re
import sys
import os # Import os for path manipulation

def markdown_to_renpy(markdown_content, character_definitions):
    rpy_script = []
    
    # Add character definitions to the beginning of the script
    rpy_script.append("# Character Definitions")
    for char_var, char_name, color in character_definitions:
        rpy_script.append(f"define {char_var} = Character('{char_name}', color='{color}')")
    rpy_script.append("\n")

    lines = markdown_content.split('\n')
    previous_speaker_var = None

    for line in lines:
        line = line.strip()
        if not line:
            rpy_script.append("    # (blank line)\n")
            continue

        # Markdown headings to Ren'Py labels
        if line.startswith('#'):
            # Convert heading to a valid Ren'Py label name
            # Remove leading '#'s, replace spaces with underscores, remove non-alphanumeric, lowercase
            label_name = re.sub(r'^#+\s*', '', line).strip()
            label_name = re.sub(r'[^a-zA-Z0-9_]', '', re.sub(r'\s+', '_', label_name)).lower()
            
            # Ensure label starts with a letter or underscore if it could start with a number
            if label_name and label_name[0].isdigit():
                label_name = "_" + label_name
            
            # If the label is empty after sanitization, create a generic one or skip
            if not label_name:
                rpy_script.append(f"    # Original heading was: {line} (could not generate valid label)\n")
                continue

            rpy_script.append(f"label {label_name}:\n")
            rpy_script.append(f"    # {line}\n")
            continue

        # Character Log entries (e.g., ***BailieBoy's Log***)
        log_match = re.match(r'\*\*\*(.*?)\'s Log\*\*\*', line)
        if log_match:
            char_name_from_log = log_match.group(1).replace(' ', '') # Remove space for var
            # Try to find the char_var from character_definitions
            char_var_found = None
            for char_var, name, color in character_definitions:
                if name.replace(' ', '').lower() == char_name_from_log.lower():
                    char_var_found = char_var
                    break
            if char_var_found:
                rpy_script.append(f"    {char_var} \"{line}\"\n")
                previous_speaker_var = char_var
            else:
                rpy_script.append(f"    \"\"{line}\"\"\n") # Fallback to narration if char not found, using triple quotes
                previous_speaker_var = None
            continue
        
        # Dialogue and inline character attribution (e.g., *text* - Character.)
        inline_dialogue_match = re.match(r'^\*(.*?)\* - (.*?)\.$', line)
        if inline_dialogue_match:
            dialogue_text = inline_dialogue_match.group(1)
            char_name_inline = inline_dialogue_match.group(2).replace(' ', '')
            char_var_found = None
            for char_var, name, color in character_definitions:
                if name.replace(' ', '').lower() == char_name_inline.lower():
                    char_var_found = char_var
                    break
            if char_var_found:
                rpy_script.append(f"    {char_var} \"{dialogue_text}\"\n")
                previous_speaker_var = char_var
            else:
                rpy_script.append(f"    \"\"({line})\"\"\n") # Fallback to narration if char not found, using triple quotes
                previous_speaker_var = None
            continue

        # Explicit character dialogue (e.g., Daniel "Hello." or Daniel: "Hello.")
        explicit_dialogue_match = re.match(r'^([A-Za-z_]+)\s*[:"](.*?)"?$', line)
        if explicit_dialogue_match:
            char_var = explicit_dialogue_match.group(1)
            dialogue = explicit_dialogue_match.group(2).strip()
            
            # Check if char_var is in our defined characters, case-insensitively
            found_char = False
            for c_var, c_name, c_color in character_definitions:
                if c_var.lower() == char_var.lower():
                    char_var = c_var # Use the defined character variable for consistency
                    found_char = True
                    break
            
            if found_char:
                rpy_script.append(f"    {char_var} \"{dialogue.replace('"', '\\"')}\"\n")
                previous_speaker_var = char_var
            else: # If not an explicit character, treat as narration or complex dialogue line
                rpy_script.append(f"    \"\"(Unrecognized speaker format: {line})\"\"\n") # Changed to triple quotes for narration
                previous_speaker_var = None
            continue

        # Lines in quotes (assuming dialogue, infer speaker if possible)
        quoted_line_match = re.match(r'^"(.*?)"$', line)
        if quoted_line_match:
            dialogue_text = quoted_line_match.group(1).replace('"', '\\"')
            if previous_speaker_var:
                rpy_script.append(f"    {previous_speaker_var} \"{dialogue_text}\"\n")
            else:
                rpy_script.append(f"    \"\"... {dialogue_text}\"\"\n") # Narrator if no previous speaker (changed to triple quotes)
            continue

        # Generic paragraphs as narration
        rpy_script.append(f"    \"\"{line}\"\"\n") # Using triple quotes for narration
        previous_speaker_var = None

    return "".join(rpy_script)

if __name__ == "__main__":
    if len(sys.argv) != 3:
        print("Usage: python md_to_rpy.py <input_markdown_file> <output_rpy_file>")
        sys.exit(1)

    input_md_file = sys.argv[1]
    output_rpy_file = sys.argv[2]

    # Define characters with Ren'Py variable, full name, and color
    characters = [
        ("daniel", "Daniel", "#c8ffc8"),
        ("zorya", "Zorya", "#ffc8c8"),
        ("fof", "FoF", "#c8c8ff"),
        ("jay", "Jay", "#ffffc8"),
        ("joe", "Joe", "#ffc8ff"),
        ("john", "John Hon", "#c8ffff"), # Daniel's father
        ("karin", "Karin", "#ffaa00"),
        ("alan", "Alan", "#aaffaa"),
        ("nitya", "Nitya", "#aaffff"),
        ("volkov", "Volkov", "#ff0000"),
        ("dmitri", "Dmitri", "#aa0000"),
        ("misha", "Misha", "#cc0000"),
        ("anya", "Anya", "#aaaa00"),
        ("bailieboy", "BailieBoy", "#cc6600"),
        ("grahamcracker", "GrahamCracker", "#66cc00"),
        ("reesie", "Reesie", "#990099"),
        ("sam", "Sam", "#009999"),
    ]

    try:
        with open(input_md_file, "r", encoding="utf-8") as f:
            md_content = f.read()

        rpy_script_content = markdown_to_renpy(md_content, characters)

        # Ensure the output directory exists
        os.makedirs(os.path.dirname(output_rpy_file), exist_ok=True)

        with open(output_rpy_file, "w", encoding="utf-8") as f:
            f.write(rpy_script_content)

        print(f"Successfully converted '{input_md_file}' to '{output_rpy_file}'")

    except FileNotFoundError:
        print(f"Error: Input file '{input_md_file}' not found.")
        sys.exit(1)
    except Exception as e:
        print(f"An error occurred: {e}")
        sys.exit(1)
