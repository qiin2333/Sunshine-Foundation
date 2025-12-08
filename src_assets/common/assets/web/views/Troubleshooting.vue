<template>
  <div>
    <Navbar />
    <div class="container py-4">
      <div class="page-header mb-4">
        <h1 class="page-title">
          <i class="fas fa-tools me-3"></i>
          {{ $t('troubleshooting.troubleshooting') }}
        </h1>
      </div>

      <div class="row">
        <!-- Left Column -->
        <div class="col-lg-6">
          <!-- Force Close App -->
          <TroubleshootingCard
            icon="fa-times-circle"
            color="warning"
            :title="$t('troubleshooting.force_close')"
            :description="$t('troubleshooting.force_close_desc')"
          >
            <template #alerts>
              <div class="alert alert-success d-flex align-items-center" v-if="closeAppStatus === true">
                <i class="fas fa-check-circle me-2"></i>
                {{ $t('troubleshooting.force_close_success') }}
              </div>
              <div class="alert alert-danger d-flex align-items-center" v-if="closeAppStatus === false">
                <i class="fas fa-exclamation-circle me-2"></i>
                {{ $t('troubleshooting.force_close_error') }}
              </div>
            </template>
            <button class="btn btn-warning" :disabled="closeAppPressed" @click="closeApp">
              <i class="fas fa-times me-2"></i>
              {{ $t('troubleshooting.force_close') }}
            </button>
          </TroubleshootingCard>

          <!-- Restart Sunshine -->
          <TroubleshootingCard
            icon="fa-sync-alt"
            color="info"
            :title="$t('troubleshooting.restart_sunshine')"
            :description="$t('troubleshooting.restart_sunshine_desc')"
          >
            <template #alerts>
              <div class="alert alert-success d-flex align-items-center" v-if="restartPressed">
                <i class="fas fa-check-circle me-2"></i>
                {{ $t('troubleshooting.restart_sunshine_success') }}
              </div>
            </template>
            <button class="btn btn-info text-white" :disabled="restartPressed" @click="restart">
              <i class="fas fa-redo me-2"></i>
              {{ $t('troubleshooting.restart_sunshine') }}
            </button>
          </TroubleshootingCard>

          <!-- Boom Sunshine -->
          <TroubleshootingCard
            icon="fa-bomb"
            color="danger"
            :title="$t('troubleshooting.boom_sunshine')"
            :description="$t('troubleshooting.boom_sunshine_desc')"
          >
            <template #alerts>
              <div class="alert alert-success d-flex align-items-center" v-if="boomPressed">
                <i class="fas fa-check-circle me-2"></i>
                {{ $t('troubleshooting.boom_sunshine_success') }}
              </div>
            </template>
            <button class="btn btn-danger" :disabled="boomPressed" @click="showBoomModal">
              <i class="fas fa-bomb me-2"></i>
              {{ $t('troubleshooting.boom_sunshine') }}
            </button>
          </TroubleshootingCard>
        </div>

        <!-- Right Column -->
        <div class="col-lg-6">
          <!-- Reset persistent display device settings -->
          <TroubleshootingCard
            v-if="platform === 'windows'"
            icon="fa-desktop"
            color="secondary"
            :title="$t('troubleshooting.reset_display_device_windows')"
            :description="$t('troubleshooting.reset_display_device_desc_windows')"
            pre-line
          >
            <template #alerts>
              <div class="alert alert-success d-flex align-items-center" v-if="resetDisplayDeviceStatus === true">
                <i class="fas fa-check-circle me-2"></i>
                {{ $t('troubleshooting.reset_display_device_success_windows') }}
              </div>
              <div class="alert alert-danger d-flex align-items-center" v-if="resetDisplayDeviceStatus === false">
                <i class="fas fa-exclamation-circle me-2"></i>
                {{ $t('troubleshooting.reset_display_device_error_windows') }}
              </div>
            </template>
            <button
              class="btn btn-secondary"
              :disabled="resetDisplayDevicePressed"
              @click="resetDisplayDevicePersistence"
            >
              <i class="fas fa-undo me-2"></i>
              {{ $t('troubleshooting.reset_display_device_windows') }}
            </button>
          </TroubleshootingCard>

          <!-- Reopen Setup Wizard -->
          <TroubleshootingCard
            icon="fa-magic"
            color="primary"
            :title="$t('troubleshooting.reopen_setup_wizard')"
            :description="$t('troubleshooting.reopen_setup_wizard_desc')"
          >
            <button class="btn btn-primary" @click="handleReopenSetupWizard">
              <i class="fas fa-redo me-2"></i>
              {{ $t('troubleshooting.reopen_setup_wizard') }}
            </button>
          </TroubleshootingCard>
        </div>
      </div>

      <!-- Logs Section - Full Width -->
      <LogsSection
        v-model:logFilter="logFilter"
        v-model:matchMode="matchMode"
        v-model:ignoreCase="ignoreCase"
        :actualLogs="actualLogs"
        :copyLogs="copyLogs"
        :copyConfig="handleCopyConfig"
      />
    </div>

    <!-- Boom Confirm Modal -->
    <div id="boomConfirmModal" class="modal" :class="{ show: showBoomConfirmModal }" @click.self="closeBoomModal">
      <div class="modal-content" @click.stop>
        <div class="modal-header">
          <h5>
            <i class="fas fa-bomb me-2"></i>{{ $t('troubleshooting.confirm_boom') }}
          </h5>
          <span class="close" @click="closeBoomModal">&times;</span>
        </div>
        <div class="modal-body">
          <p>{{ $t('troubleshooting.confirm_boom_desc') }}</p>
        </div>
        <div class="modal-footer">
          <button type="button" class="btn btn-secondary" @click="closeBoomModal">
            {{ $t('_common.cancel') }}
          </button>
          <button type="button" class="btn btn-danger" @click="confirmBoom">
            <i class="fas fa-bomb me-2"></i>{{ $t('troubleshooting.boom_sunshine') }}
          </button>
        </div>
      </div>
    </div>
  </div>
</template>

<script setup>
import { onMounted, ref } from 'vue'
import { useI18n } from 'vue-i18n'
import Navbar from '../components/layout/Navbar.vue'
import TroubleshootingCard from '../components/TroubleshootingCard.vue'
import LogsSection from '../components/LogsSection.vue'
import { useTroubleshooting } from '../composables/useTroubleshooting.js'
import { useModalScrollLock } from '../composables/useModalScrollLock.js'

const { t } = useI18n()

const {
  platform,
  closeAppPressed,
  closeAppStatus,
  restartPressed,
  boomPressed,
  resetDisplayDevicePressed,
  resetDisplayDeviceStatus,
  logFilter,
  matchMode,
  ignoreCase,
  actualLogs,
  refreshLogs,
  closeApp,
  restart,
  boom,
  resetDisplayDevicePersistence,
  copyLogs,
  copyConfig,
  reopenSetupWizard,
  loadPlatform,
  startLogRefresh,
} = useTroubleshooting()

const showBoomConfirmModal = ref(false)

// 使用滚动锁定 composable，禁用滚动到顶部以保持模态窗口在视口中心
useModalScrollLock(showBoomConfirmModal, { scrollToTop: false })

const showBoomModal = () => {
  showBoomConfirmModal.value = true
}

const closeBoomModal = () => {
  showBoomConfirmModal.value = false
}

const confirmBoom = () => {
  closeBoomModal()
  boom()
}

const handleCopyConfig = () => copyConfig(t)

const handleReopenSetupWizard = () => reopenSetupWizard(t)

onMounted(async () => {
  await Promise.all([loadPlatform(), refreshLogs()])
  startLogRefresh()
})
</script>

<style>
@import '../styles/global.less';
</style>

<style scoped>
.btn {
  border-radius: 8px;
  padding: 0.5rem 1rem;
  font-weight: 500;
  transition: all 0.2s ease;
}

.btn:hover {
  transform: translateY(-1px);
}

.alert {
  border-radius: 8px;
  font-size: 0.9rem;
  padding: 0.75rem 1rem;
}

.modal {
  display: none;
  position: fixed;
  z-index: 1000;
  left: 0;
  top: 0;
  width: 100%;
  height: 100%;
  overflow: hidden;
  background-color: rgba(0, 0, 0, 0.5);
  align-items: center;
  justify-content: center;
}

.modal.show {
  display: flex;
}

.modal-content {
  background-color: #ffffff;
  margin: auto;
  padding: 0;
  border: 1px solid #888;
  border-radius: 8px;
  width: 90%;
  max-width: 500px;
  max-height: 90vh;
  display: flex;
  flex-direction: column;
  box-shadow: 0 4px 6px rgba(0, 0, 0, 0.1);
}

[data-bs-theme='dark'] .modal-content {
  background-color: #212529;
  border-color: #495057;
  color: #fff;
}

.modal-header {
  display: flex;
  justify-content: space-between;
  align-items: center;
  padding: 15px 20px;
  border-bottom: 1px solid #dee2e6;
  background-color: #f8f9fa;
  border-radius: 8px 8px 0 0;
}

[data-bs-theme='dark'] .modal-header {
  background-color: #343a40;
  border-bottom-color: #495057;
}

.modal-header h5 {
  margin: 0;
  font-size: 1.1rem;
  font-weight: 600;
}

[data-bs-theme='dark'] .modal-header h5 {
  color: #fff;
}

.close {
  color: #aaa;
  font-size: 28px;
  font-weight: bold;
  cursor: pointer;
  line-height: 1;
}

.close:hover,
.close:focus {
  color: #000;
}

[data-bs-theme='dark'] .close:hover,
[data-bs-theme='dark'] .close:focus {
  color: #fff;
}

.modal-body {
  padding: 20px;
  font-size: 0.95rem;
  line-height: 1.5;
  overflow-y: auto;
  flex: 1;
}

[data-bs-theme='dark'] .modal-body {
  color: #e9ecef;
}

.modal-footer {
  display: flex;
  justify-content: flex-end;
  gap: 10px;
  padding: 15px 20px;
  border-top: 1px solid #dee2e6;
  background-color: #f8f9fa;
  border-radius: 0 0 8px 8px;
}

[data-bs-theme='dark'] .modal-footer {
  background-color: #343a40;
  border-top-color: #495057;
}

.modal-footer button {
  padding: 8px 16px;
  font-size: 0.9rem;
}

@media (max-width: 991.98px) {
  .page-title {
    font-size: 1.5rem;
  }
}
</style>
