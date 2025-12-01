import { APP_CONSTANTS } from './constants.js';

/**
 * Unified error handler class
 * Note: This class is not currently used but is prepared for future use with i18n support
 */
export class ErrorHandler {
  /**
   * Handle network errors
   * @param {Error} error Error object
   * @param {string} context Error context (will be translated via i18n)
   * @param {Function} t Translation function (optional, for i18n support)
   * @returns {string} User-friendly error message
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
   * Handle validation errors
   * @param {Array} errors Error array
   * @param {Function} t Translation function (optional)
   * @returns {string} Formatted error message
   */
  static handleValidationErrors(errors, t = null) {
    const getText = (key, fallback) => t ? t(key) : fallback;
    
    if (!Array.isArray(errors) || errors.length === 0) {
      return getText('apps.validation_failed', 'Validation failed');
    }
    
    return errors.join(t ? t('apps.error_separator') || '; ' : '; ');
  }

  /**
   * Handle application operation errors
   * @param {Error} error Error object
   * @param {string} operation Operation type
   * @param {string} appName Application name
   * @param {Function} t Translation function (optional)
   * @returns {string} Formatted error message
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
   * Show error dialog
   * @param {string} message Error message
   * @param {string} title Title
   * @param {Function} t Translation function (optional)
   */
  static showErrorDialog(message, title = null, t = null) {
    const getText = (key, fallback) => t ? t(key) : fallback;
    const dialogTitle = title || getText('_common.error', 'Error');
    // If a more complex error dialog is needed, implement it here
    // Currently using simple alert
    alert(`${dialogTitle}\n\n${message}`);
  }

  /**
   * Show confirmation dialog
   * @param {string} message Confirmation message
   * @param {string} title Title
   * @param {Function} t Translation function (optional)
   * @returns {boolean} Whether user confirmed
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