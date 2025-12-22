import { ref, computed } from 'vue'
import { useI18n } from 'vue-i18n'

export function useUserSettings() {
  const { t } = useI18n()

  // State
  const loading = ref(false)
  const saving = ref(false)
  const error = ref(null)
  const success = ref(false)
  const config = ref(null)

  // User settings state
  const settings = ref({
    locale: '',
    notifyPreReleases: false,
    systemTray: true,
  })

  // Original settings for comparison
  const originalSettings = ref(null)

  // Computed: check if there are unsaved changes
  const hasChanges = computed(() => {
    if (!originalSettings.value) return false
    return JSON.stringify(settings.value) !== JSON.stringify(originalSettings.value)
  })

  // Load current settings from the server
  const loadSettings = async () => {
    loading.value = true
    error.value = null
    try {
      const response = await fetch('/api/config')
      if (!response.ok) {
        throw new Error(t('user_settings.load_error'))
      }
      config.value = await response.json()

      // Extract user-relevant settings
      settings.value = {
        locale: config.value.locale || '',
        notifyPreReleases: config.value.notify_pre_releases === 'true' || config.value.notify_pre_releases === true,
        systemTray: config.value.system_tray !== 'disabled',
      }

      // Store original settings for comparison
      originalSettings.value = { ...settings.value }
    } catch (err) {
      // In preview mode, API may not be available
      if (err?.message?.includes('JSON') || err?.message?.includes('<!DOCTYPE')) {
        console.warn('API not available in preview mode')
      } else {
        error.value = err.message || t('user_settings.load_error')
        console.error('Failed to load settings:', err)
      }
    } finally {
      loading.value = false
    }
  }

  // Save settings to the server
  const saveSettings = async () => {
    saving.value = true
    error.value = null
    success.value = false
    try {
      // Prepare the config payload
      const payload = {
        ...config.value,
        locale: settings.value.locale,
        notify_pre_releases: settings.value.notifyPreReleases ? 'true' : 'false',
        system_tray: settings.value.systemTray ? 'enabled' : 'disabled',
      }

      const response = await fetch('/api/config', {
        method: 'POST',
        headers: { 'Content-Type': 'application/json' },
        body: JSON.stringify(payload),
      })

      if (!response.ok) {
        const result = await response.json().catch(() => ({}))
        throw new Error(result.error || t('user_settings.save_error'))
      }

      const result = await response.json()
      if (result.status?.toString() !== 'true') {
        throw new Error(result.error || t('user_settings.save_error'))
      }

      // Update original settings on successful save
      originalSettings.value = { ...settings.value }
      success.value = true

      // Clear success message after 3 seconds
      setTimeout(() => {
        success.value = false
      }, 3000)
    } catch (err) {
      error.value = err.message || t('user_settings.save_error')
      console.error('Failed to save settings:', err)
    } finally {
      saving.value = false
    }
  }

  // Reset settings to original values
  const resetSettings = () => {
    if (originalSettings.value) {
      settings.value = { ...originalSettings.value }
    }
  }

  return {
    // State
    loading,
    saving,
    error,
    success,
    settings,
    hasChanges,

    // Actions
    loadSettings,
    saveSettings,
    resetSettings,
  }
}
