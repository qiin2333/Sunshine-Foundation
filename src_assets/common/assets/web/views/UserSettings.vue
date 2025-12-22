<template>
  <div>
    <Navbar />
    <div class="container py-4">
      <div class="row justify-content-center">
        <div class="col-lg-8 col-xl-7">
          <div class="text-center mb-4">
            <div class="icon-wrapper mb-2">
              <Icon name="cog" :size="32" icon-class="text-primary" />
            </div>
            <h1 class="h4 page-title">{{ $t('user_settings.title') }}</h1>
            <p class="text-muted small mb-0">{{ $t('user_settings.description') }}</p>
          </div>

          <!-- Loading State -->
          <div v-if="loading" class="text-center py-5">
            <div class="spinner-border text-primary" role="status">
              <span class="visually-hidden">{{ $t('_common.loading') || 'Loading...' }}</span>
            </div>
          </div>

          <!-- Settings Form -->
          <form v-else @submit.prevent="saveSettings">
            <div class="card border-0 shadow-sm rounded-3">
              <div class="card-body p-3 p-md-4">
                <!-- User Interface Section -->
                <div class="section-header d-flex align-items-center mb-3">
                  <div class="section-icon me-2">
                    <Icon name="desktop" :size="20" icon-class="text-primary" />
                  </div>
                  <h6 class="mb-0 fw-semibold">{{ $t('user_settings.interface_section') }}</h6>
                </div>

                <div class="row g-3 mb-4">
                  <!-- Locale Setting -->
                  <div class="col-12">
                    <label for="locale" class="form-label fw-medium small">
                      <i class="bi bi-globe me-1 text-muted"></i>
                      {{ $t('user_settings.locale') }}
                    </label>
                    <select id="locale" class="form-select" v-model="settings.locale">
                      <option value="">{{ $t('_common.autodetect') }}</option>
                      <option v-for="locale in availableLocales" :key="locale.code" :value="locale.code">
                        {{ locale.name }}
                      </option>
                    </select>
                    <div class="form-text small">{{ $t('user_settings.locale_desc') }}</div>
                  </div>

                  <!-- System Tray Setting -->
                  <div class="col-12">
                    <div class="form-check form-switch">
                      <input
                        class="form-check-input"
                        type="checkbox"
                        id="systemTray"
                        v-model="settings.systemTray"
                      />
                      <label class="form-check-label fw-medium small" for="systemTray">
                        <i class="bi bi-window-stack me-1 text-muted"></i>
                        {{ $t('user_settings.system_tray') }}
                      </label>
                    </div>
                    <div class="form-text small ms-4">{{ $t('user_settings.system_tray_desc') }}</div>
                  </div>
                </div>

                <hr class="my-3" />

                <!-- Notifications Section -->
                <div class="section-header d-flex align-items-center mb-3">
                  <div class="section-icon section-icon-info me-2">
                    <Icon name="bell" :size="20" icon-class="text-info" />
                  </div>
                  <h6 class="mb-0 fw-semibold">{{ $t('user_settings.notifications_section') }}</h6>
                </div>

                <div class="row g-3">
                  <!-- Notify Pre-Releases Setting -->
                  <div class="col-12">
                    <div class="form-check form-switch">
                      <input
                        class="form-check-input"
                        type="checkbox"
                        id="notifyPreReleases"
                        v-model="settings.notifyPreReleases"
                      />
                      <label class="form-check-label fw-medium small" for="notifyPreReleases">
                        <i class="bi bi-megaphone me-1 text-muted"></i>
                        {{ $t('user_settings.notify_pre_releases') }}
                      </label>
                    </div>
                    <div class="form-text small ms-4">{{ $t('user_settings.notify_pre_releases_desc') }}</div>
                  </div>
                </div>

                <!-- Alerts -->
                <div class="alert alert-danger d-flex align-items-center mt-3 rounded-3 py-2" v-if="error">
                  <i class="bi bi-exclamation-triangle-fill me-2"></i>
                  <div class="small">
                    <strong>{{ $t('_common.error') }}</strong> {{ error }}
                  </div>
                </div>
                <div class="alert alert-success d-flex align-items-center mt-3 rounded-3 py-2" v-if="success">
                  <i class="bi bi-check-circle-fill me-2"></i>
                  <div class="small">
                    <strong>{{ $t('_common.success') }}</strong> {{ $t('user_settings.save_success') }}
                  </div>
                </div>

                <!-- Action Buttons -->
                <div class="d-flex gap-2 mt-3">
                  <button
                    class="btn btn-outline-secondary flex-grow-1 py-2 rounded-3"
                    type="button"
                    :disabled="!hasChanges || saving"
                    @click="resetSettings"
                  >
                    <i class="bi bi-arrow-counterclockwise me-2"></i>{{ $t('user_settings.reset') }}
                  </button>
                  <button
                    class="btn btn-primary flex-grow-1 py-2 rounded-3"
                    type="submit"
                    :disabled="!hasChanges || saving"
                  >
                    <span v-if="saving" class="spinner-border spinner-border-sm me-2" role="status"></span>
                    <i v-else class="bi bi-save me-2"></i>{{ $t('_common.save') }}
                  </button>
                </div>
              </div>
            </div>
          </form>

          <!-- Help Card -->
          <div class="card border-0 shadow-sm rounded-3 mt-4">
            <div class="card-body p-3">
              <div class="d-flex align-items-start">
                <div class="section-icon section-icon-success me-3 flex-shrink-0">
                  <Icon name="info-circle" :size="20" icon-class="text-success" />
                </div>
                <div>
                  <h6 class="mb-1 fw-semibold">{{ $t('user_settings.help_title') }}</h6>
                  <p class="text-muted small mb-0">{{ $t('user_settings.help_desc') }}</p>
                </div>
              </div>
            </div>
          </div>
        </div>
      </div>
    </div>
  </div>
</template>

<script setup>
import { onMounted, ref } from 'vue'
import Navbar from '../components/layout/Navbar.vue'
import Icon from '../components/common/Icon.vue'
import { useUserSettings } from '../composables/useUserSettings.js'
import { trackEvents } from '../config/firebase.js'

const {
  loading,
  saving,
  error,
  success,
  settings,
  hasChanges,
  loadSettings,
  saveSettings,
  resetSettings,
} = useUserSettings()

// Available locales
const availableLocales = ref([
  { code: 'en', name: 'English' },
  { code: 'en_GB', name: 'English (UK)' },
  { code: 'en_US', name: 'English (US)' },
  { code: 'zh', name: '简体中文' },
  { code: 'zh_TW', name: '繁體中文' },
  { code: 'ja', name: '日本語' },
  { code: 'ko', name: '한국어' },
  { code: 'de', name: 'Deutsch' },
  { code: 'fr', name: 'Français' },
  { code: 'es', name: 'Español' },
  { code: 'pt', name: 'Português' },
  { code: 'pt_BR', name: 'Português (Brasil)' },
  { code: 'it', name: 'Italiano' },
  { code: 'ru', name: 'Русский' },
  { code: 'uk', name: 'Українська' },
  { code: 'pl', name: 'Polski' },
  { code: 'cs', name: 'Čeština' },
  { code: 'tr', name: 'Türkçe' },
  { code: 'sv', name: 'Svenska' },
  { code: 'bg', name: 'Български' },
])

onMounted(async () => {
  trackEvents.pageView('user_settings')
  await loadSettings()
})
</script>

<style>
@import '../styles/global.less';
</style>

<style lang="less" scoped>
@transition-duration: 0.2s;
@transition-timing: ease;
@primary-bg-opacity: 0.1;
@success-bg-opacity: 0.1;
@info-bg-opacity: 0.1;

.icon-wrapper {
  display: inline-flex;
  align-items: center;
  justify-content: center;
  width: 56px;
  height: 56px;
  background: rgba(var(--bs-primary-rgb), @primary-bg-opacity);
  border-radius: 50%;
}

.section-icon {
  display: flex;
  align-items: center;
  justify-content: center;
  width: 32px;
  height: 32px;
  background: rgba(var(--bs-primary-rgb), @primary-bg-opacity);
  border-radius: 8px;
  font-size: 1rem;

  &-success {
    background: rgba(var(--bs-success-rgb), @success-bg-opacity);
  }

  &-info {
    background: rgba(var(--bs-info-rgb), @info-bg-opacity);
  }
}

.form-control,
.form-select {
  padding: 0.5rem 0.75rem;
  border-radius: 0.375rem;
  transition: border-color @transition-duration @transition-timing,
              box-shadow @transition-duration @transition-timing;

  &:focus {
    box-shadow: 0 0 0 0.15rem rgba(var(--bs-primary-rgb), 0.15);
  }
}

.form-check-input {
  cursor: pointer;

  &:checked {
    background-color: var(--bs-primary);
    border-color: var(--bs-primary);
  }
}

.form-check-label {
  cursor: pointer;
}

.card {
  transition: transform @transition-duration @transition-timing;
}

.btn-primary {
  transition: transform @transition-duration @transition-timing,
              box-shadow @transition-duration @transition-timing;

  &:hover:not(:disabled) {
    transform: translateY(-1px);
    box-shadow: 0 4px 12px rgba(var(--bs-primary-rgb), 0.35);
  }
}

.btn-outline-secondary {
  transition: transform @transition-duration @transition-timing;

  &:hover:not(:disabled) {
    transform: translateY(-1px);
  }
}

hr {
  opacity: 0.1;
}
</style>
