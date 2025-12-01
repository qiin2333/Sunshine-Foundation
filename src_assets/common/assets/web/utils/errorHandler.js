import { APP_CONSTANTS } from './constants.js';

/**
 * 统一错误处理类
 * Note: This class is not currently used but is prepared for future use with i18n support
 */
export class ErrorHandler {
  /**
   * 处理网络错误
   * @param {Error} error 错误对象
   * @param {string} context 错误上下文（将通过 i18n 翻译）
   * @param {Function} t 翻译函数（可选，用于 i18n 支持）
   * @returns {string} 用户友好的错误信息
   */
  static handleNetworkError(error, context = '', t = null) {
    const getText = (key, fallback) => t ? t(key) : fallback;
    
    console.error(`${context || 'Operation'} failed:`, error);
    
    if (error.name === 'TypeError' && error.message.includes('Failed to fetch')) {
      return getText('apps.network_connection_failed', 'Network connection failed, please check your connection and try again');
    }
    
    if (error.message.includes('404')) {
      return getText('apps.resource_not_found', `${context || 'Operation'} failed: Requested resource not found`);
    }
    
    if (error.message.includes('500')) {
      return getText('apps.server_error', `${context || 'Operation'} failed: Internal server error`);
    }
    
    if (error.message.includes('403')) {
      return getText('apps.permission_denied', `${context || 'Operation'} failed: Permission denied`);
    }
    
    return getText('apps.operation_failed', `${context || 'Operation'} failed: ${error.message || 'Unknown error'}`);
  }

  /**
   * 处理验证错误
   * @param {Array} errors 错误数组
   * @param {Function} t 翻译函数（可选）
   * @returns {string} 格式化的错误信息
   */
  static handleValidationErrors(errors, t = null) {
    const getText = (key, fallback) => t ? t(key) : fallback;
    
    if (!Array.isArray(errors) || errors.length === 0) {
      return getText('apps.validation_failed', 'Validation failed');
    }
    
    return errors.join(t ? t('apps.error_separator') || '; ' : '; ');
  }

  /**
   * 处理应用操作错误
   * @param {Error} error 错误对象
   * @param {string} operation 操作类型
   * @param {string} appName 应用名称
   * @param {Function} t 翻译函数（可选）
   * @returns {string} 格式化的错误信息
   */
  static handleAppError(error, operation, appName = '', t = null) {
    const appContext = appName ? `"${appName}"` : '';
    const getText = (key, fallback) => t ? t(key) : fallback;
    
    switch(operation) {
      case 'save':
        return this.handleNetworkError(error, getText('apps.save_app', `Save application${appContext}`), t);
      case 'delete':
        return this.handleNetworkError(error, getText('apps.delete_app', `Delete application${appContext}`), t);
      case 'load':
        return this.handleNetworkError(error, getText('apps.load_app', `Load application${appContext}`), t);
      default:
        return this.handleNetworkError(error, getText('apps.operate_app', `Operate application${appContext}`), t);
    }
  }

  /**
   * 创建错误弹窗
   * @param {string} message 错误信息
   * @param {string} title 标题
   * @param {Function} t 翻译函数（可选）
   */
  static showErrorDialog(message, title = null, t = null) {
    const getText = (key, fallback) => t ? t(key) : fallback;
    const dialogTitle = title || getText('_common.error', 'Error');
    // 如果需要更复杂的错误弹窗，可以在这里实现
    // 目前使用简单的 alert
    alert(`${dialogTitle}\n\n${message}`);
  }

  /**
   * 创建确认弹窗
   * @param {string} message 确认信息
   * @param {string} title 标题
   * @param {Function} t 翻译函数（可选）
   * @returns {boolean} 用户是否确认
   */
  static showConfirmDialog(message, title = null, t = null) {
    const getText = (key, fallback) => t ? t(key) : fallback;
    const dialogTitle = title || getText('apps.confirm', 'Confirm');
    return confirm(`${dialogTitle}\n\n${message}`);
  }

  /**
   * 记录错误日志
   * @param {Error} error 错误对象
   * @param {string} context 错误上下文
   * @param {Object} metadata 额外的元数据
   */
  static logError(error, context = '', metadata = {}) {
    const errorInfo = {
      message: error.message,
      stack: error.stack,
      context,
      timestamp: new Date().toISOString(),
      ...metadata
    };
    
    console.error('应用错误:', errorInfo);
    
    // 如果需要发送到日志服务，可以在这里实现
    // this.sendToLogService(errorInfo);
  }

  /**
   * 处理异步操作错误
   * @param {Promise} promise Promise对象
   * @param {string} context 错误上下文
   * @returns {Promise} 包装后的Promise
   */
  static async handleAsyncError(promise, context = '') {
    try {
      return await promise;
    } catch (error) {
      this.logError(error, context);
      throw error;
    }
  }
} 