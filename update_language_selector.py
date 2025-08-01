import re

def update_readme_with_language_selector():
    # Read the original README
    with open('README.md', 'r', encoding='utf-8') as f:
        content = f.read()
    
    # Check if language selector already exists
    if "多语言支持" in content or "Multi-language Support" in content:
        print("Language selector already exists, skipping...")
        return
    
    # Language selector content
    language_selector = '''
## 🌐 多语言支持 / Multi-language Support

<div align="center">

[![English](https://img.shields.io/badge/English-README.en.md-blue?style=for-the-badge)](README.en.md)
[![中文简体](https://img.shields.io/badge/中文简体-README.zh--CN.md-red?style=for-the-badge)](README.zh-CN.md)
[![Français](https://img.shields.io/badge/Français-README.fr.md-green?style=for-the-badge)](README.fr.md)
[![Deutsch](https://img.shields.io/badge/Deutsch-README.de.md-yellow?style=for-the-badge)](README.de.md)
[![日本語](https://img.shields.io/badge/日本語-README.ja.md-purple?style=for-the-badge)](README.ja.md)

</div>

---

'''
    
    # Find the position after the first heading
    lines = content.split('\n')
    insert_position = 0
    
    for i, line in enumerate(lines):
        if line.startswith('# ') and i > 0:
            insert_position = i + 1
            break
    
    # Insert language selector
    if insert_position > 0:
        lines.insert(insert_position, language_selector)
        new_content = '\n'.join(lines)
        
        # Write updated content
        with open('README.md', 'w', encoding='utf-8') as f:
            f.write(new_content)
        
        print("✓ Added language selector to main README")
    else:
        print("Could not find suitable position to insert language selector")

if __name__ == "__main__":
    update_readme_with_language_selector()
