<script setup>
import { computed, ref } from 'vue'
import { useI18n } from 'vue-i18n'

const props = defineProps([
  'platform',
  'config'
])

const { t } = useI18n()

const defaultMoonlightPort = 47989

const config = ref(props.config)
const effectivePort = computed(() => +config.value?.port ?? defaultMoonlightPort)
const showCurlModal = ref(false)
const copied = ref(false)

// 生成 curl 命令
const curlCommand = computed(() => {
  if (!config.value.webhook_url) {
    return ''
  }
  
  const url = config.value.webhook_url
  const payload = JSON.stringify({
    msgtype: 'text',
    text: {
      content: 'Hello, Sunshine Foundation Webhook'
    }
  })
  
  // 转义 JSON 中的特殊字符，用于 shell（单引号需要特殊处理）
  // 使用单引号包裹，并将单引号转义为 '\''
  const escapedPayload = payload.replace(/'/g, "'\\''")
  
  return `curl -X POST '${url}' \\\n  -H 'Content-Type: application/json' \\\n  -d '${escapedPayload}'`
})

// 显示 curl 命令弹窗
const showCurlCommand = () => {
  showCurlModal.value = true
  copied.value = false
}

// 关闭 curl 命令弹窗
const closeCurlModal = () => {
  showCurlModal.value = false
  copied.value = false
}

// 复制 curl 命令到剪贴板
const copyCurlCommand = async () => {
  try {
    await navigator.clipboard.writeText(curlCommand.value)
    copied.value = true
    setTimeout(() => {
      copied.value = false
    }, 2000)
  } catch (error) {
    // 降级方案：使用传统方法
    const textArea = document.createElement('textarea')
    textArea.value = curlCommand.value
    textArea.style.position = 'fixed'
    textArea.style.opacity = '0'
    document.body.appendChild(textArea)
    textArea.select()
    try {
      document.execCommand('copy')
      copied.value = true
      setTimeout(() => {
        copied.value = false
      }, 2000)
    } catch (err) {
      alert(t('config.webhook_curl_copy_failed') || '复制失败，请手动选择并复制')
    }
    document.body.removeChild(textArea)
  }
}

// Webhook test function
const testWebhook = async () => {
  if (!config.value.webhook_url) {
    alert(t('config.webhook_test_url_required'))
    return
  }

  // Check if URL is valid
  let webhookUrl
  try {
    webhookUrl = new URL(config.value.webhook_url)
  } catch (error) {
    alert(t('config.webhook_test_failed') + ': Invalid URL format')
    return
  }

  // 方案：使用 no-cors 模式绕过 CORS 限制
  // 注意：no-cors 模式下无法读取响应状态，但请求会被发送
  try {
    const testPayload = JSON.stringify({
      msgtype: 'text',
      text: {
        content: 'Sunshine Webhook Test - This is a test message from Sunshine configuration page'
      }
    })

    // 先尝试正常请求（如果服务器支持 CORS）
    try {
      const response = await fetch(config.value.webhook_url, {
        method: 'POST',
        headers: {
          'Content-Type': 'application/json',
        },
        body: testPayload
      })

      if (response.ok) {
        alert(t('config.webhook_test_success'))
        return
      } else {
        throw new Error(`HTTP ${response.status}`)
      }
    } catch (corsError) {
      // 如果遇到 CORS 错误，使用 no-cors 模式
      if (corsError.name === 'TypeError' && corsError.message.includes('Failed to fetch')) {
        // 使用 no-cors 模式发送请求（绕过 CORS，但无法读取响应）
        await fetch(config.value.webhook_url, {
          method: 'POST',
          mode: 'no-cors', // 绕过 CORS 检查
          headers: {
            'Content-Type': 'application/json',
          },
          body: testPayload
        })

        // no-cors 模式下无法读取响应，但请求已发送
        alert(t('config.webhook_test_success') + '\n\n提示：由于 CORS 限制，无法确认服务器响应状态。\n请求已发送，如果 webhook 配置正确，消息应该已送达。\n\n建议：在浏览器开发者工具的 Network 标签页中查看请求详情。')
      } else {
        throw corsError
      }
    }
  } catch (error) {
    alert(`${t('config.webhook_test_failed')}: ${error.message || 'Unknown error'}\n\n提示：请检查 URL 是否正确，或查看浏览器控制台获取更多信息。`)
  }
}
</script>

<template>
  <div id="network" class="config-page">
    <!-- UPnP -->
    <div class="mb-3">
      <label for="upnp" class="form-label">{{ $t('config.upnp') }}</label>
      <select id="upnp" class="form-select" v-model="config.upnp">
        <option value="disabled">{{ $t('_common.disabled_def') }}</option>
        <option value="enabled">{{ $t('_common.enabled') }}</option>
      </select>
      <div class="form-text">{{ $t('config.upnp_desc') }}</div>
    </div>

    <!-- Address family -->
    <div class="mb-3">
      <label for="address_family" class="form-label">{{ $t('config.address_family') }}</label>
      <select id="address_family" class="form-select" v-model="config.address_family">
        <option value="ipv4">{{ $t('config.address_family_ipv4') }}</option>
        <option value="both">{{ $t('config.address_family_both') }}</option>
      </select>
      <div class="form-text">{{ $t('config.address_family_desc') }}</div>
    </div>

    <!-- Port family -->
    <div class="mb-3">
      <label for="port" class="form-label">{{ $t('config.port') }}</label>
      <input type="number" min="1029" max="65514" class="form-control" id="port" :placeholder="defaultMoonlightPort"
             v-model="config.port" />
      <div class="form-text">{{ $t('config.port_desc') }}</div>
      <!-- Add warning if any port is less than 1024 -->
      <div class="alert alert-danger" v-if="(+effectivePort - 5) < 1024">
        <i class="fa-solid fa-xl fa-triangle-exclamation"></i> {{ $t('config.port_alert_1') }}
      </div>
      <!-- Add warning if any port is above 65535 -->
      <div class="alert alert-danger" v-if="(+effectivePort + 21) > 65535">
        <i class="fa-solid fa-xl fa-triangle-exclamation"></i> {{ $t('config.port_alert_2') }}
      </div>
      <!-- Create a port table for the various ports needed by Sunshine -->
      <table class="table">
        <thead>
        <tr>
          <th scope="col">{{ $t('config.port_protocol') }}</th>
          <th scope="col">{{ $t('config.port_port') }}</th>
          <th scope="col">{{ $t('config.port_note') }}</th>
        </tr>
        </thead>
        <tbody>
        <tr>
          <!-- HTTPS -->
          <td>{{ $t('config.port_tcp') }}</td>
          <td>{{+effectivePort - 5}}</td>
          <td></td>
        </tr>
        <tr>
          <!-- HTTP -->
          <td>{{ $t('config.port_tcp') }}</td>
          <td>{{+effectivePort}}</td>
          <td>
            <div class="alert alert-primary" role="alert" v-if="+effectivePort !== defaultMoonlightPort">
              <i class="fa-solid fa-xl fa-circle-info"></i> {{ $t('config.port_http_port_note') }}
            </div>
          </td>
        </tr>
        <tr>
          <!-- Web UI -->
          <td>{{ $t('config.port_tcp') }}</td>
          <td>{{+effectivePort + 1}}</td>
          <td>{{ $t('config.port_web_ui') }}</td>
        </tr>
        <tr>
          <!-- RTSP -->
          <td>{{ $t('config.port_tcp') }}</td>
          <td>{{+effectivePort + 21}}</td>
          <td></td>
        </tr>
        <tr>
          <!-- Video, Control, Audio, Mic -->
          <td>{{ $t('config.port_udp') }}</td>
          <td>{{+effectivePort + 9}} - {{+effectivePort + 12}}</td>
          <td></td>
        </tr>
        </tbody>
      </table>
      <!-- add warning about exposing web ui to the internet -->
      <div class="alert alert-warning" v-if="config.origin_web_ui_allowed === 'wan'">
        <i class="fa-solid fa-xl fa-triangle-exclamation"></i> {{ $t('config.port_warning') }}
      </div>
    </div>

    <!-- Origin Web UI Allowed -->
    <div class="mb-3">
      <label for="origin_web_ui_allowed" class="form-label">{{ $t('config.origin_web_ui_allowed') }}</label>
      <select id="origin_web_ui_allowed" class="form-select" v-model="config.origin_web_ui_allowed">
        <option value="pc">{{ $t('config.origin_web_ui_allowed_pc') }}</option>
        <option value="lan">{{ $t('config.origin_web_ui_allowed_lan') }}</option>
        <option value="wan">{{ $t('config.origin_web_ui_allowed_wan') }}</option>
      </select>
      <div class="form-text">{{ $t('config.origin_web_ui_allowed_desc') }}</div>
    </div>

    <!-- External IP -->
    <div class="mb-3">
      <label for="external_ip" class="form-label">{{ $t('config.external_ip') }}</label>
      <input type="text" class="form-control" id="external_ip" placeholder="123.456.789.12" v-model="config.external_ip" />
      <div class="form-text">{{ $t('config.external_ip_desc') }}</div>
    </div>

    <!-- LAN Encryption Mode -->
    <div class="mb-3">
      <label for="lan_encryption_mode" class="form-label">{{ $t('config.lan_encryption_mode') }}</label>
      <select id="lan_encryption_mode" class="form-select" v-model="config.lan_encryption_mode">
        <option value="0">{{ $t('_common.disabled_def') }}</option>
        <option value="1">{{ $t('config.lan_encryption_mode_1') }}</option>
        <option value="2">{{ $t('config.lan_encryption_mode_2') }}</option>
      </select>
      <div class="form-text">{{ $t('config.lan_encryption_mode_desc') }}</div>
    </div>

    <!-- WAN Encryption Mode -->
    <div class="mb-3">
      <label for="wan_encryption_mode" class="form-label">{{ $t('config.wan_encryption_mode') }}</label>
      <select id="wan_encryption_mode" class="form-select" v-model="config.wan_encryption_mode">
        <option value="0">{{ $t('_common.disabled') }}</option>
        <option value="1">{{ $t('config.wan_encryption_mode_1') }}</option>
        <option value="2">{{ $t('config.wan_encryption_mode_2') }}</option>
      </select>
      <div class="form-text">{{ $t('config.wan_encryption_mode_desc') }}</div>
    </div>

    <!-- CLOSE VERIFY SAFE -->
    <div class="mb-3">
      <label for="close_verify_safe" class="form-label">{{ $t('config.close_verify_safe') }}</label>
      <select id="close_verify_safe" class="form-select" v-model="config.close_verify_safe">
        <option value="disabled">{{ $t('_common.disabled_def') }}</option>
        <option value="enabled">{{ $t('_common.enabled') }}</option>
      </select>
      <div class="form-text">{{ $t('config.close_verify_safe_desc') }}</div>
    </div>

    <!-- MDNS BROADCAST -->
    <div class="mb-3">
      <label for="mdns_broadcast" class="form-label">{{ $t('config.mdns_broadcast') }}</label>
      <select id="mdns_broadcast" class="form-select" v-model="config.mdns_broadcast">
        <option value="disabled">{{ $t('_common.disabled') }}</option>
        <option value="enabled">{{ $t('_common.enabled_def') }}</option>
      </select>
      <div class="form-text">{{ $t('config.mdns_broadcast_desc') }}</div>
    </div>

    <!-- Ping Timeout -->
    <div class="mb-3">
      <label for="ping_timeout" class="form-label">{{ $t('config.ping_timeout') }}</label>
      <input type="text" class="form-control" id="ping_timeout" placeholder="10000" v-model="config.ping_timeout" />
      <div class="form-text">{{ $t('config.ping_timeout_desc') }}</div>
    </div>

    <!-- Webhook Settings -->
    <div class="accordion">
      <div class="accordion-item">
        <h2 class="accordion-header">
          <button class="accordion-button" type="button" data-bs-toggle="collapse"
                  data-bs-target="#webhook-collapse">
            {{ $t('config.webhook_group') }}
          </button>
        </h2>
        <div id="webhook-collapse" class="accordion-collapse collapse show">
          <div class="accordion-body">
            <!-- Webhook Enable -->
            <div class="mb-3">
              <label for="webhook_enabled" class="form-label">{{ $t('config.webhook_enabled') }}</label>
              <select id="webhook_enabled" class="form-select" v-model="config.webhook_enabled">
                <option value="disabled">{{ $t('_common.disabled_def') }}</option>
                <option value="enabled">{{ $t('_common.enabled') }}</option>
              </select>
              <div class="form-text">{{ $t('config.webhook_enabled_desc') }}</div>
            </div>

            <!-- Webhook URL -->
            <div class="mb-3" v-if="config.webhook_enabled === 'enabled'">
              <label for="webhook_url" class="form-label">{{ $t('config.webhook_url') }}</label>
              <div class="input-group">
                <input type="url" class="form-control" id="webhook_url" placeholder="https://example.com/webhook" v-model="config.webhook_url" />
                <button class="btn btn-outline-secondary" type="button" @click="testWebhook" :disabled="!config.webhook_url || config.webhook_enabled !== 'enabled'">
                  {{ $t('config.webhook_test') }}
                </button>
                <button class="btn btn-outline-info" type="button" @click="showCurlCommand" :disabled="!config.webhook_url || config.webhook_enabled !== 'enabled'">
                  <i class="fas fa-terminal me-1"></i>{{ $t('config.webhook_curl_command') || '命令' }}
                </button>
              </div>
              <div class="form-text">{{ $t('config.webhook_url_desc') }}</div>
            </div>

            <!-- Skip SSL Verify -->
            <div class="mb-3" v-if="config.webhook_enabled === 'enabled'">
              <label for="webhook_skip_ssl_verify" class="form-label">{{ $t('config.webhook_skip_ssl_verify') }}</label>
              <select id="webhook_skip_ssl_verify" class="form-select" v-model="config.webhook_skip_ssl_verify">
                <option value="disabled">{{ $t('_common.disabled_def') }}</option>
                <option value="enabled">{{ $t('_common.enabled') }}</option>
              </select>
              <div class="form-text">{{ $t('config.webhook_skip_ssl_verify_desc') }}</div>
            </div>

            <!-- Webhook Timeout -->
            <div class="mb-3" v-if="config.webhook_enabled === 'enabled'">
              <label for="webhook_timeout" class="form-label">{{ $t('config.webhook_timeout') }}</label>
              <input type="number" min="100" max="5000" class="form-control" id="webhook_timeout" placeholder="1000" v-model="config.webhook_timeout" />
              <div class="form-text">{{ $t('config.webhook_timeout_desc') }}</div>
            </div>
          </div>
        </div>
      </div>
    </div>

  </div>

  <!-- Curl Command Modal -->
  <div id="curlCommandModal" class="modal" :class="{ show: showCurlModal }" @click.self="closeCurlModal">
    <div class="modal-content" @click.stop>
      <div class="modal-header">
        <h5><i class="fas fa-terminal me-2"></i>{{ $t('config.webhook_curl_command') || 'Curl 命令' }}</h5>
        <span class="close" @click="closeCurlModal">&times;</span>
      </div>
      <div class="modal-body">
        <p class="text-muted mb-3">{{ $t('config.webhook_curl_command_desc') || '复制以下命令到终端中执行，可以测试 webhook 是否正常工作：' }}</p>
        <div class="curl-command-container">
          <pre class="curl-command" id="curlCommandText">{{ curlCommand }}</pre>
          <button class="btn btn-sm btn-primary copy-btn" @click="copyCurlCommand" type="button">
            <i class="fas fa-copy me-1"></i>{{ $t('_common.copy') || '复制' }}
          </button>
        </div>
        <div class="alert alert-info mt-3" v-if="copied">
          <i class="fas fa-check-circle me-2"></i>{{ $t('_common.copied') || '已复制到剪贴板' }}
        </div>
      </div>
      <div class="modal-footer">
        <button type="button" class="btn btn-secondary" @click="closeCurlModal">{{ $t('_common.close') || '关闭' }}</button>
      </div>
    </div>
  </div>
</template>

<style scoped>
/* Modal styles (参考 SetupWizard.vue) */
.modal {
  display: none;
  position: fixed;
  z-index: 1000;
  left: 0;
  top: 0;
  width: 100%;
  height: 100%;
  overflow: auto;
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
  max-width: 700px;
  box-shadow: 0 4px 6px rgba(0, 0, 0, 0.1);
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

.modal-header h5 {
  margin: 0;
  font-size: 1.1rem;
  font-weight: 600;
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

.modal-body {
  padding: 20px;
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

/* Curl command container */
.curl-command-container {
  position: relative;
  background-color: #f8f9fa;
  border: 1px solid #dee2e6;
  border-radius: 4px;
  padding: 15px;
}

.curl-command {
  margin: 0;
  padding: 0;
  font-family: 'Courier New', monospace;
  font-size: 0.9rem;
  color: #212529;
  background: transparent;
  border: none;
  white-space: pre-wrap;
  word-break: break-all;
  overflow-x: auto;
  max-height: 300px;
  overflow-y: auto;
}

.copy-btn {
  position: absolute;
  top: 10px;
  right: 10px;
}
</style>
