
with open('vol2DanielandBailieBoy_FoFandGraham.md', 'r') as f:
    content = f.read()

anchor_string = '''"Alright," Jay said, his voice hard. "So we don't just rescue Daniel. We gut them. We turn their own fortress into their tomb. Send me everything. Praetorian is going dark. Full spectrum, black ops. We're going to give them a game they'll never forget. For Daniel. And for Joe," he added, almost to himself.'''

# Find the index of the anchor string
index = content.find(anchor_string)

if index != -1:
    # Calculate the end of the anchor string
    end_index = index + len(anchor_string)
    
    # Truncate the content at this point, removing everything after it
    truncated_content = content[:end_index]
    
    with open('vol2DanielandBailieBoy_FoFandGraham.md', 'w') as f:
        f.write(truncated_content)
    print('Successfully removed duplicate content.')
else:
    print('Anchor string not found. No content removed.')
