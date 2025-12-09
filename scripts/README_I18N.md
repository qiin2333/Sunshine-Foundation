# i18n Tools

This directory contains scripts for managing internationalization (i18n) in the Sunshine project.

## Overview

The i18n tools ensure that all locale files maintain consistency with the base English locale file (`en.json`), and that all files are properly formatted to minimize Git conflicts and improve code review.

## Available Scripts

### 1. validate-i18n.js

Validates that all locale files have the same keys as the base locale file.

**Usage:**
```bash
# Check all locale files for missing/extra keys
npm run i18n:validate

# Auto-sync missing keys (adds English values as placeholders)
npm run i18n:sync

# Validate with CI exit code (for CI/CD pipelines)
npm run i18n:validate:ci
```

**What it does:**
- Compares all locale files against `en.json` (base file)
- Reports missing keys that exist in `en.json` but not in other locales
- Reports extra keys that exist in other locales but not in `en.json`
- In `--sync` mode, automatically adds missing keys with English values

**Example output:**
```
🔍 Validating i18n translations...

📋 Base locale (en.json) has 606 keys

✅ zh.json: All keys present (606 keys)
❌ fr.json: Issues found
   Missing 5 keys:
     - config.new_setting
     - apps.new_feature
     ...
```

### 2. format-i18n.js

Formats and sorts all locale JSON files alphabetically.

**Usage:**
```bash
# Format all locale files
npm run i18n:format

# Check if files are properly formatted (for CI/CD)
npm run i18n:format:check
```

**What it does:**
- Sorts all keys alphabetically (recursively for nested objects)
- Applies consistent 2-space indentation
- Ensures trailing newline
- Works on all `.json` files in the locale directory

**Why this is important:**
- Alphabetical sorting reduces merge conflicts when multiple developers add keys
- Consistent formatting makes code review easier
- Makes it easy to spot missing translations by comparing files side-by-side

## Workflow for Adding New Translations

### Step 1: Add keys to en.json

Always add new translation keys to `en.json` first:

```json
{
  "myapp": {
    "new_feature": "New Feature Description"
  }
}
```

### Step 2: Sync to other locales

Run the sync command to add the new key to all other locale files:

```bash
npm run i18n:sync
```

This will add the key with the English value as a placeholder in all locale files.

### Step 3: Format all files

Ensure consistent formatting:

```bash
npm run i18n:format
```

### Step 4: Translate placeholders

Manually translate the English placeholders in each locale file to the appropriate language.

### Step 5: Validate

Before committing, verify everything is correct:

```bash
npm run i18n:validate
```

### Step 6: Commit changes

Commit all locale file changes together:

```bash
git add src_assets/common/assets/web/public/assets/locale/*.json
git commit -m "Add translation keys for new feature"
```

## CI/CD Integration

The i18n validation is integrated into the GitHub Actions CI pipeline via `.github/workflows/i18n-validation.yml`.

**The CI workflow will:**
1. Check that all locale files are properly formatted
2. Validate that all locale files have the same keys as `en.json`
3. Fail the PR if validation fails
4. Post a helpful comment on the PR with instructions to fix issues

**To pass the CI checks:**
- Run `npm run i18n:format` before committing
- Run `npm run i18n:validate` to ensure no missing keys
- Or run `npm run i18n:sync` to auto-add missing keys

## Best Practices

1. **Always edit en.json first**: Never add keys directly to other locale files
2. **Use npm scripts**: Don't run the scripts directly, use the npm scripts
3. **Format before committing**: Run `npm run i18n:format` before every commit
4. **Validate before pushing**: Run `npm run i18n:validate` before pushing
5. **Translate manually**: Don't use auto-translation for the synced placeholders - work with native speakers or use CrowdIn

## File Structure

```
src_assets/common/assets/web/public/assets/locale/
├── en.json       # Base locale (English) - source of truth
├── zh.json       # Chinese (Simplified)
├── zh_TW.json    # Chinese (Traditional)
├── fr.json       # French
├── de.json       # German
├── es.json       # Spanish
├── ja.json       # Japanese
└── ...           # Other locales
```

## Troubleshooting

### Missing keys error

**Problem:** `npm run i18n:validate` reports missing keys

**Solution:** 
```bash
npm run i18n:sync  # Auto-add missing keys
# Then manually translate the placeholders
```

### Formatting check failed

**Problem:** CI reports formatting issues

**Solution:**
```bash
npm run i18n:format  # Auto-format all files
git add src_assets/common/assets/web/public/assets/locale/*.json
git commit -m "Format locale files"
```

### Extra keys warning

**Problem:** Locale file has keys not in `en.json`

**Solution:** This usually means old keys that were removed from `en.json` but not from other locales. Remove them manually or update `en.json` if they should be kept.

## Technical Details

### Base Locale

The base locale is hardcoded as `en.json` in the scripts. All other locale files are compared against this file.

### Key Comparison

Keys are compared using dot notation (e.g., `config.general.name`). Nested objects are flattened for comparison.

### Formatting Rules

- 2-space indentation
- No trailing commas
- Unix line endings (LF)
- Trailing newline at end of file
- Alphabetically sorted keys at all levels

## Contributing

When contributing to i18n:

1. Follow the workflow above
2. Test your changes with the validation scripts
3. Ensure CI passes before requesting review
4. Document any new translation keys in your PR description

## Related Documentation

- [WEBUI_DEVELOPMENT.md](../../docs/WEBUI_DEVELOPMENT.md) - Comprehensive WebUI development guide
- [contributing.md](../../docs/contributing.md) - General contribution guidelines
