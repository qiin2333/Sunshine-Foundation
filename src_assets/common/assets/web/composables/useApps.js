import { ref, computed } from 'vue'
import { AppService } from '../services/appService.js'
import { APP_CONSTANTS, ENV_VARS_CONFIG } from '../utils/constants.js'
import { debounce, deepClone } from '../utils/helpers.js'
import { trackEvents } from '../config/firebase.js'
import { searchSteamCovers } from '../utils/steamApi.js'

// 消息显示时长
const MESSAGE_DURATION = 3000

/**
 * 应用管理组合式函数
 */
export function useApps() {
  const apps = ref([])
  const filteredApps = ref([])
  const searchQuery = ref('')
  const editingApp = ref(null)
  const platform = ref('')
  const isSaving = ref(false)
  const isDragging = ref(false)
  const viewMode = ref('grid')
  const message = ref('')
  const messageType = ref('success')
  const envVars = ref({})
  const debouncedSearch = ref(null)
  // 扫描相关状态
  const isScanning = ref(false)
  const scannedApps = ref([])
  const showScanResult = ref(false)

  // 计算属性
  const messageClass = computed(() => ({
    'alert-success': messageType.value === 'success',
    'alert-error': messageType.value === 'error',
    'alert-warning': messageType.value === 'warning',
    'alert-info': messageType.value === 'info',
  }))

  // 工具函数
  const showMessage = (msg, type = APP_CONSTANTS.MESSAGE_TYPES.SUCCESS) => {
    message.value = msg
    messageType.value = type
    setTimeout(() => {
      message.value = ''
    }, MESSAGE_DURATION)
  }

  const getMessageIcon = () => {
    const icons = {
      success: 'fa-check-circle',
      error: 'fa-exclamation-circle',
      warning: 'fa-exclamation-triangle',
      info: 'fa-info-circle',
    }
    return icons[messageType.value] || icons.success
  }

  const createDefaultApp = (overrides = {}) => ({
    ...APP_CONSTANTS.DEFAULT_APP,
    index: -1,
    ...overrides,
  })

  // 初始化
  const initEnvVars = (t) => {
    envVars.value = Object.fromEntries(
      Object.entries(ENV_VARS_CONFIG).map(([key, translationKey]) => [key, t(translationKey)])
    )
  }

  const init = (t) => {
    initEnvVars(t)
    debouncedSearch.value = debounce(performSearch, APP_CONSTANTS.SEARCH_DEBOUNCE_TIME)
  }

  // 数据加载
  const loadApps = async () => {
    try {
      apps.value = await AppService.getApps()
      filteredApps.value = [...apps.value]
    } catch (error) {
      console.error('加载应用失败:', error)
      showMessage('加载应用失败', APP_CONSTANTS.MESSAGE_TYPES.ERROR)
    }
  }

  const loadPlatform = async () => {
    try {
      platform.value = await AppService.getPlatform()
    } catch (error) {
      console.error('加载平台信息失败:', error)
      platform.value = APP_CONSTANTS.PLATFORMS.WINDOWS
    }
  }

  // 搜索
  const performSearch = () => {
    filteredApps.value = AppService.searchApps(apps.value, searchQuery.value)
  }

  const clearSearch = () => {
    searchQuery.value = ''
    performSearch()
  }

  // 应用操作
  const getOriginalIndex = (app) => apps.value.findIndex((a) => a === app)

  const newApp = () => {
    trackEvents.userAction('new_app_clicked')
    editingApp.value = createDefaultApp()
  }

  const editApp = (index) => {
    editingApp.value = { ...deepClone(apps.value[index]), index }
  }

  const closeAppEditor = () => {
    editingApp.value = null
  }

  const handleSaveApp = async (appData) => {
    try {
      isSaving.value = true
      await AppService.saveApps(apps.value, appData)
      await loadApps()
      editingApp.value = null
      showMessage('应用保存成功', APP_CONSTANTS.MESSAGE_TYPES.SUCCESS)
    } catch (error) {
      console.error('保存应用失败:', error)
      showMessage('保存应用失败', APP_CONSTANTS.MESSAGE_TYPES.ERROR)
    } finally {
      isSaving.value = false
    }
  }

  const showDeleteForm = async (index) => {
    if (await confirm(`确定要删除应用 "${apps.value[index].name}" 吗？`)) {
      await deleteApp(index)
    }
  }

  const deleteApp = async (index) => {
    const appName = apps.value[index]?.name || 'unknown'
    try {
      apps.value.splice(index, 1)
      await AppService.saveApps(apps.value, null)
      await loadApps()
      showMessage('应用删除成功', APP_CONSTANTS.MESSAGE_TYPES.SUCCESS)
      trackEvents.appDeleted(appName)
    } catch (error) {
      console.error('删除应用失败:', error)
      showMessage('删除应用失败', APP_CONSTANTS.MESSAGE_TYPES.ERROR)
    }
  }

  const save = async () => {
    try {
      isSaving.value = true
      await AppService.saveApps(apps.value, null)
      showMessage('应用列表保存成功', APP_CONSTANTS.MESSAGE_TYPES.SUCCESS)
      trackEvents.userAction('apps_saved', { count: apps.value.length })
    } catch (error) {
      console.error('保存应用列表失败:', error)
      showMessage('保存应用列表失败', APP_CONSTANTS.MESSAGE_TYPES.ERROR)
    } finally {
      isSaving.value = false
    }
  }

  // 拖拽排序
  const onDragStart = () => {
    isDragging.value = true
  }

  const onDragEnd = async () => {
    isDragging.value = false
    await save()
  }

  // 缓存已加载的bucket数据
  const bucketCache = new Map()
  // 缓存已加载的游戏详情
  const gameCache = new Map()

  // 获取搜索bucket
  const getSearchBucket = (name) => {
    const bucket = name
      .substring(0, 2)
      .toLowerCase()
      .replaceAll(/[^a-z\d\u4e00-\u9fa5]/g, '')
    return bucket || '@'
  }

  // 搜索封面图片 - 同时使用 IGDB/GameDB 和 Steam API (参照 CoverFinder.vue)
  const searchCoverImage = async (appName) => {
    if (!appName) return ''

    const searchName = appName.replaceAll(/\s+/g, '.').toLowerCase()
    const bucket = getSearchBucket(appName)

    try {
      // 同时搜索 IGDB 和 Steam
      const [igdbResult, steamResult] = await Promise.allSettled([
        searchIGDBCover(searchName, bucket),
        searchSteamCovers(appName),
      ])

      // 优先返回 IGDB 结果，如果没有则返回 Steam 结果
      if (igdbResult.status === 'fulfilled' && igdbResult.value) {
        return igdbResult.value
      }
      if (steamResult.status === 'fulfilled' && steamResult.value) {
        return steamResult.value
      }
      return ''
    } catch (error) {
      console.warn(`搜索封面失败: ${appName}`, error)
      return ''
    }
  }

  // 搜索 IGDB 封面
  const searchIGDBCover = async (searchName, bucket) => {
    try {
      // 使用缓存的bucket数据
      let maps = bucketCache.get(bucket)
      if (!maps) {
        const response = await fetch(`https://lizardbyte.github.io/GameDB/buckets/${bucket}.json`)
        if (!response.ok) return ''
        maps = await response.json()
        bucketCache.set(bucket, maps)
      }

      // 查找匹配的游戏ID
      const matchedIds = Object.keys(maps)
        .filter((id) => maps[id].name.replaceAll(/\s+/g, '.').toLowerCase().startsWith(searchName))
        .slice(0, 1) // 只取第一个匹配

      if (matchedIds.length === 0) return ''

      const id = matchedIds[0]

      // 检查缓存
      let game = gameCache.get(id)
      if (!game) {
        const res = await fetch(`https://lizardbyte.github.io/GameDB/games/${id}.json`)
        if (!res.ok) return ''
        game = await res.json()
        gameCache.set(id, game)
      }

      if (!game?.cover?.url) return ''

      // 构建封面URL
      const thumb = game.cover.url
      const hash = thumb.substring(thumb.lastIndexOf('/') + 1, thumb.lastIndexOf('.'))
      return `https://images.igdb.com/igdb/image/upload/t_cover_big_2x/${hash}.png`
    } catch (error) {
      console.warn(`搜索IGDB封面失败: ${searchName}`, error)
      return ''
    }
  }

  // 批量搜索封面图片
  const batchSearchCoverImages = async (appList) => {
    const results = await Promise.allSettled(
      appList.map(async (app) => {
        const imagePath = await searchCoverImage(encodeURIComponent(app.name))
        return { ...app, 'image-path': imagePath }
      })
    )
    return results.map((result, index) => (result.status === 'fulfilled' ? result.value : appList[index]))
  }

  // 扫描目录功能
  const scanDirectory = async (extractIcons = true) => {
    if (!window.__TAURI__?.core?.invoke) {
      showMessage('扫描功能仅在 Tauri 环境下可用', APP_CONSTANTS.MESSAGE_TYPES.WARNING)
      return
    }

    // 使用 Tauri 文件对话框选择目录
    if (!window.__TAURI__?.dialog?.open) {
      showMessage('无法打开文件对话框', APP_CONSTANTS.MESSAGE_TYPES.ERROR)
      return
    }

    try {
      const selectedDir = await window.__TAURI__.dialog.open({
        directory: true,
        multiple: false,
        title: '选择要扫描的目录',
      })

      if (!selectedDir) {
        return // 用户取消了选择
      }

      isScanning.value = true
      showMessage('正在扫描目录...', APP_CONSTANTS.MESSAGE_TYPES.INFO)

      const foundApps = await window.__TAURI__.core.invoke('scan_directory_for_apps', {
        directory: selectedDir,
        extractIcons,
      })

      if (foundApps.length === 0) {
        scannedApps.value = foundApps
        showScanResult.value = true
        showMessage('未找到可添加的应用程序', APP_CONSTANTS.MESSAGE_TYPES.INFO)
      } else {
        showMessage(`找到 ${foundApps.length} 个应用程序，正在搜索封面...`, APP_CONSTANTS.MESSAGE_TYPES.INFO)

        // 批量搜索封面图片
        const appsWithCovers = await batchSearchCoverImages(foundApps)
        scannedApps.value = appsWithCovers
        showScanResult.value = true

        const coversFound = appsWithCovers.filter((app) => app['image-path']).length
        console.log(appsWithCovers)
        showMessage(
          `找到 ${foundApps.length} 个应用程序，已匹配 ${coversFound} 个封面`,
          APP_CONSTANTS.MESSAGE_TYPES.SUCCESS
        )
      }

      trackEvents.userAction('directory_scanned', { count: foundApps.length, extractIcons })
    } catch (error) {
      console.error('扫描目录失败:', error)
      showMessage(`扫描失败: ${error}`, APP_CONSTANTS.MESSAGE_TYPES.ERROR)
    } finally {
      isScanning.value = false
    }
  }

  // 获取扫描应用的字段值（兼容不同的字段名）
  const getScannedAppField = (app, field) => {
    // 尝试带横线的字段名和下划线的字段名
    return app[field] || app[field.replace(/-/g, '_')] || ''
  }

  // 获取扫描应用的图片路径
  const getScannedAppImage = (app) => {
    return getScannedAppField(app, 'image-path')
  }

  // 从扫描结果添加单个应用（打开编辑器）
  const addScannedApp = (scannedApp) => {
    editingApp.value = createDefaultApp({
      name: scannedApp.name,
      cmd: scannedApp.cmd,
      'working-dir': getScannedAppField(scannedApp, 'working-dir'),
      'image-path': getScannedAppField(scannedApp, 'image-path'),
    })

    // 从扫描列表中移除
    const index = scannedApps.value.findIndex((a) => a.source_path === scannedApp.source_path)
    if (index !== -1) {
      scannedApps.value.splice(index, 1)
    }

    showMessage(`正在编辑应用: ${scannedApp.name}`, APP_CONSTANTS.MESSAGE_TYPES.INFO)
    trackEvents.userAction('scanned_app_edit', { name: scannedApp.name })
  }

  // 快速添加扫描到的应用（不打开编辑器，直接保存）
  const quickAddScannedApp = async (scannedApp, index) => {
    try {
      const newApp = {
        ...APP_CONSTANTS.DEFAULT_APP,
        name: scannedApp.name,
        cmd: scannedApp.cmd,
        'working-dir': getScannedAppField(scannedApp, 'working-dir'),
        'image-path': getScannedAppField(scannedApp, 'image-path'),
      }

      apps.value.push(newApp)
      await AppService.saveApps(apps.value, null)
      await loadApps()

      // 从扫描列表中移除
      scannedApps.value.splice(index, 1)
      if (scannedApps.value.length === 0) {
        showScanResult.value = false
      }

      showMessage(`已添加应用: ${scannedApp.name}`, APP_CONSTANTS.MESSAGE_TYPES.SUCCESS)
      trackEvents.userAction('scanned_app_quick_added', { name: scannedApp.name })
    } catch (error) {
      console.error('快速添加应用失败:', error)
      showMessage('添加失败', APP_CONSTANTS.MESSAGE_TYPES.ERROR)
    }
  }

  // 批量添加所有扫描到的应用
  const addAllScannedApps = async () => {
    if (scannedApps.value.length === 0) return

    try {
      isSaving.value = true
      const appsToAdd = scannedApps.value.map((scannedApp) => ({
        ...APP_CONSTANTS.DEFAULT_APP,
        name: scannedApp.name,
        cmd: scannedApp.cmd,
        'working-dir': getScannedAppField(scannedApp, 'working-dir'),
        'image-path': getScannedAppField(scannedApp, 'image-path'),
      }))

      // 添加到应用列表
      apps.value.push(...appsToAdd)
      await AppService.saveApps(apps.value, null)
      await loadApps()

      showMessage(`已添加 ${appsToAdd.length} 个应用`, APP_CONSTANTS.MESSAGE_TYPES.SUCCESS)
      trackEvents.userAction('scanned_apps_batch_added', { count: appsToAdd.length })

      // 清空扫描结果
      scannedApps.value = []
      showScanResult.value = false
    } catch (error) {
      console.error('批量添加应用失败:', error)
      showMessage('批量添加失败', APP_CONSTANTS.MESSAGE_TYPES.ERROR)
    } finally {
      isSaving.value = false
    }
  }

  // 关闭扫描结果
  const closeScanResult = () => {
    showScanResult.value = false
    scannedApps.value = []
  }

  // 从扫描结果中移除应用
  const removeScannedApp = (index) => {
    scannedApps.value.splice(index, 1)
    if (scannedApps.value.length === 0) {
      showScanResult.value = false
    }
  }

  // 为单个扫描应用搜索封面
  const searchCoverForScannedApp = async (index) => {
    const app = scannedApps.value[index]
    if (!app) return

    try {
      showMessage(`正在搜索封面: ${app.name}`, APP_CONSTANTS.MESSAGE_TYPES.INFO)
      const imagePath = await searchCoverImage(app.name)
      if (imagePath) {
        scannedApps.value[index] = { ...app, 'image-path': imagePath }
        showMessage(`已找到封面: ${app.name}`, APP_CONSTANTS.MESSAGE_TYPES.SUCCESS)
      } else {
        showMessage(`未找到封面: ${app.name}`, APP_CONSTANTS.MESSAGE_TYPES.WARNING)
      }
    } catch (error) {
      console.error('搜索封面失败:', error)
      showMessage('搜索封面失败', APP_CONSTANTS.MESSAGE_TYPES.ERROR)
    }
  }

  // 复制处理
  const handleCopySuccess = () => showMessage('复制成功', APP_CONSTANTS.MESSAGE_TYPES.SUCCESS)
  const handleCopyError = () => showMessage('复制失败', APP_CONSTANTS.MESSAGE_TYPES.ERROR)

  // 检查是否在 Tauri 环境
  const isTauriEnv = () => !!window.__TAURI__?.core?.invoke

  return {
    // 状态
    apps,
    filteredApps,
    searchQuery,
    editingApp,
    platform,
    isSaving,
    isDragging,
    viewMode,
    message,
    messageType,
    envVars,
    debouncedSearch,
    // 扫描相关状态
    isScanning,
    scannedApps,
    showScanResult,
    // 计算属性
    messageClass,
    // 方法
    init,
    loadApps,
    loadPlatform,
    performSearch,
    clearSearch,
    getOriginalIndex,
    newApp,
    editApp,
    closeAppEditor,
    handleSaveApp,
    showDeleteForm,
    deleteApp,
    save,
    onDragStart,
    onDragEnd,
    // 扫描相关方法
    scanDirectory,
    addScannedApp,
    quickAddScannedApp,
    addAllScannedApps,
    closeScanResult,
    removeScannedApp,
    getScannedAppImage,
    searchCoverForScannedApp,
    isTauriEnv,
    showMessage,
    getMessageIcon,
    handleCopySuccess,
    handleCopyError,
  }
}
