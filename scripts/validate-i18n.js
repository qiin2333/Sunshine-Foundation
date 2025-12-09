#!/usr/bin/env node
/**
 * i18n Translation Validation and Sync Script
 * 
 * This script validates that all locale files have the same keys as the base locale (en.json).
 * It can also automatically add missing keys with placeholder values.
 * 
 * Usage:
 *   node scripts/validate-i18n.js              # Validate only (report missing keys)
 *   node scripts/validate-i18n.js --sync       # Auto-sync missing keys with English values
 *   node scripts/validate-i18n.js --ci         # Exit with error code if validation fails (for CI)
 */

import fs from 'fs'
import path from 'path'
import { fileURLToPath } from 'url'

const __filename = fileURLToPath(import.meta.url)
const __dirname = path.dirname(__filename)

const localeDir = path.join(__dirname, '../src_assets/common/assets/web/public/assets/locale')
const baseLocale = 'en.json'

// Parse command line arguments
const args = process.argv.slice(2)
const syncMode = args.includes('--sync')
const ciMode = args.includes('--ci')

/**
 * Get all keys from a nested object
 */
function getAllKeys(obj, prefix = '') {
  const keys = []
  for (const key in obj) {
    const fullKey = prefix ? `${prefix}.${key}` : key
    if (typeof obj[key] === 'object' && obj[key] !== null && !Array.isArray(obj[key])) {
      keys.push(...getAllKeys(obj[key], fullKey))
    } else {
      keys.push(fullKey)
    }
  }
  return keys
}

/**
 * Get value from nested object using dot notation
 */
function getValue(obj, path) {
  return path.split('.').reduce((current, key) => current?.[key], obj)
}

/**
 * Set value in nested object using dot notation
 */
function setValue(obj, path, value) {
  const keys = path.split('.')
  const lastKey = keys.pop()
  const target = keys.reduce((current, key) => {
    if (!current[key] || typeof current[key] !== 'object') {
      current[key] = {}
    }
    return current[key]
  }, obj)
  target[lastKey] = value
}

/**
 * Main validation function
 */
function validateLocales() {
  console.log('🔍 Validating i18n translations...\n')
  
  // Read base locale
  const baseLocalePath = path.join(localeDir, baseLocale)
  if (!fs.existsSync(baseLocalePath)) {
    console.error(`❌ Base locale file not found: ${baseLocale}`)
    process.exit(1)
  }
  
  const baseContent = JSON.parse(fs.readFileSync(baseLocalePath, 'utf8'))
  const baseKeys = getAllKeys(baseContent).sort()
  
  console.log(`📋 Base locale (${baseLocale}) has ${baseKeys.length} keys\n`)
  
  // Get all locale files
  const localeFiles = fs.readdirSync(localeDir)
    .filter(file => file.endsWith('.json') && file !== baseLocale)
    .sort()
  
  let hasErrors = false
  const results = []
  
  for (const localeFile of localeFiles) {
    const localePath = path.join(localeDir, localeFile)
    let content
    
    try {
      content = JSON.parse(fs.readFileSync(localePath, 'utf8'))
    } catch (e) {
      console.error(`❌ Failed to parse ${localeFile}: ${e.message}`)
      hasErrors = true
      continue
    }
    
    const localeKeys = getAllKeys(content).sort()
    const missingKeys = baseKeys.filter(key => !localeKeys.includes(key))
    const extraKeys = localeKeys.filter(key => !baseKeys.includes(key))
    
    if (missingKeys.length === 0 && extraKeys.length === 0) {
      console.log(`✅ ${localeFile}: All keys present (${localeKeys.length} keys)`)
      results.push({ file: localeFile, status: 'ok', missing: 0, extra: 0 })
    } else {
      hasErrors = true
      console.log(`❌ ${localeFile}: Issues found`)
      
      if (missingKeys.length > 0) {
        console.log(`   Missing ${missingKeys.length} keys:`)
        missingKeys.slice(0, 5).forEach(key => console.log(`     - ${key}`))
        if (missingKeys.length > 5) {
          console.log(`     ... and ${missingKeys.length - 5} more`)
        }
      }
      
      if (extraKeys.length > 0) {
        console.log(`   Extra ${extraKeys.length} keys (not in base):`)
        extraKeys.slice(0, 5).forEach(key => console.log(`     - ${key}`))
        if (extraKeys.length > 5) {
          console.log(`     ... and ${extraKeys.length - 5} more`)
        }
      }
      
      results.push({ 
        file: localeFile, 
        status: 'error', 
        missing: missingKeys.length, 
        extra: extraKeys.length,
        missingKeys,
        content
      })
      
      // Auto-sync if requested
      if (syncMode && missingKeys.length > 0) {
        console.log(`   🔄 Syncing missing keys...`)
        for (const key of missingKeys) {
          const baseValue = getValue(baseContent, key)
          setValue(content, key, baseValue)
        }
        fs.writeFileSync(localePath, JSON.stringify(content, null, 2) + '\n', 'utf8')
        console.log(`   ✓ Added ${missingKeys.length} missing keys with English values`)
      }
    }
    console.log()
  }
  
  // Summary
  console.log('━'.repeat(60))
  console.log('📊 Summary:')
  console.log(`   Total locales checked: ${localeFiles.length}`)
  console.log(`   Locales with all keys: ${results.filter(r => r.status === 'ok').length}`)
  console.log(`   Locales with issues: ${results.filter(r => r.status === 'error').length}`)
  
  if (syncMode) {
    const synced = results.filter(r => r.status === 'error' && r.missing > 0)
    if (synced.length > 0) {
      console.log(`\n✅ Synced ${synced.length} locale files with missing keys`)
      console.log('   ⚠️  Remember to translate the English placeholder values!')
    }
  }
  
  console.log('━'.repeat(60))
  
  if (hasErrors && !syncMode) {
    console.log('\n💡 Tip: Run with --sync flag to automatically add missing keys')
  }
  
  if (ciMode && hasErrors && !syncMode) {
    console.error('\n❌ Validation failed in CI mode')
    process.exit(1)
  }
  
  if (hasErrors && !syncMode) {
    process.exit(1)
  }
}

// Run validation
validateLocales()
