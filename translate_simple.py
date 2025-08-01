import os
import re
from googletrans import Translator

def protect_project_names(text):
    """保护项目名称不被翻译"""
    # 定义需要保护的项目名称
    protected_names = [
        "Sunshine 基地版",
        "Sunshine-Foundation",
        "LizardByte/Sunshine",
        "Moonlight",
        "Moonlight-PC",
        "Moonlight-Android",
        "Moonlight-iOS",
        "awesome-sunshine",
        "威力加强版",
        "王冠版",
        "真砖家版",
        "qiin2333",
        "WACrown",
        "TrueZhuangJia",
        "obs-amd",
        "VAAPI",
        "NVENC",
        "AMD",
        "Intel",
        "Nvidia",
        "Ryzen",
        "Core i3",
        "Core i5",
        "GeForce GTX 1080",
        "HD Graphics 510",
        "Video Coding Engine",
        "VCE",
        "802.11ac",
        "CAT5e",
        "HDR",
        "GameStream"
    ]
    
    # 创建占位符映射
    placeholders = {}
    protected_text = text
    
    for i, name in enumerate(protected_names):
        if name in protected_text:
            placeholder = f"__PROTECTED_NAME_{i}__"
            placeholders[placeholder] = name
            protected_text = protected_text.replace(name, placeholder)
    
    return protected_text, placeholders

def restore_project_names(text, placeholders):
    """恢复项目名称"""
    restored_text = text
    for placeholder, name in placeholders.items():
        restored_text = restored_text.replace(placeholder, name)
    return restored_text

def translate_readme():
    # Read the original README
    with open('README.md', 'r', encoding='utf-8') as f:
        content = f.read()
    
    # Initialize translator
    translator = Translator()
    
    # Languages to translate to
    languages = [
        ('en', 'English'),
        ('zh-CN', 'Chinese Simplified'),
        ('fr', 'French'),
        ('de', 'German'),
        ('ja', 'Japanese')
    ]
    
    for lang_code, lang_name in languages:
        try:
            if lang_code == 'en':
                # For English, copy the original content
                translated_content = content
            else:
                # Protect project names before translation
                protected_content, placeholders = protect_project_names(content)
                
                # Translate the protected content
                translated = translator.translate(protected_content, dest=lang_code)
                translated_text = translated.text
                
                # Restore project names after translation
                translated_content = restore_project_names(translated_text, placeholders)
            
            # Write translated file
            filename = f'README.{lang_code}.md'
            with open(filename, 'w', encoding='utf-8') as f:
                f.write(translated_content)
            
            print(f"✓ Translated to {lang_name} ({lang_code}) with project names protected")
            
        except Exception as e:
            print(f"✗ Failed to translate to {lang_name}: {e}")

if __name__ == "__main__":
    translate_readme()
