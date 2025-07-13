/**
 * 图片工具函数
 * 用于处理应用图片URL的标准化逻辑
 */

/**
 * 根据应用和图片路径获取正确的图片URL
 * @param {Object} app 应用对象
 * @param {string} imagePath 图片路径
 * @returns {string} 处理后的图片URL
 */
export function getAppImageUrl(app, imagePath) {
  if (!imagePath) {
    return '';
  }
  
  // 如果是网络URL，直接返回
  if (imagePath.startsWith('http://') || imagePath.startsWith('https://')) {
    return imagePath;
  }
  
  // 如果是blob URL或data URL，直接返回
  if (imagePath.startsWith('blob:') || imagePath.startsWith('data:')) {
    return imagePath;
  }
  
  // 对于本地路径，使用后端的 /boxart 接口
  if (app && imagePath) {
    return `/boxart/${imagePath}`;
  }
  
  // 如果没有ID，回退到原始路径（可能不会工作，但提供了一个回退）
  return imagePath;
}

/**
 * 获取图片预览URL（用于编辑器预览）
 * @param {string} imagePath 图片路径
 * @returns {string} 预览URL
 */
export function getImagePreviewUrl(imagePath) {
  if (!imagePath) {
    return '';
  }
  
  // 如果是blob URL或data URL，直接返回
  if (imagePath.startsWith('blob:') || imagePath.startsWith('data:')) {
    return imagePath;
  }
  
  // 如果是网络URL，直接返回
  if (imagePath.startsWith('http://') || imagePath.startsWith('https://')) {
    return imagePath;
  }
  
  // 对于本地文件路径，在预览中尝试使用文件协议
  // 这可能不工作，但给用户一个视觉提示
  return `file://${imagePath}`;
}

/**
 * 检查图片路径是否为本地文件路径
 * @param {string} imagePath 图片路径
 * @returns {boolean} 是否为本地文件路径
 */
export function isLocalImagePath(imagePath) {
  if (!imagePath) {
    return false;
  }
  
  // 如果是网络URL或blob/data URL，不是本地路径
  if (imagePath.startsWith('http://') || 
      imagePath.startsWith('https://') ||
      imagePath.startsWith('blob:') ||
      imagePath.startsWith('data:')) {
    return false;
  }
  
  return true;
} 