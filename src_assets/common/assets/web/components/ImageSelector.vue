<template>
  <div class="form-group-enhanced">
    <label for="appImagePath" class="form-label-enhanced">{{ $t('apps.image') }}</label>
    
    <!-- 使用桌面图片选项 -->
    <div class="form-check mb-3">
      <input 
        type="checkbox" 
        class="form-check-input" 
        id="useDesktopImage" 
        v-model="isDesktopImage"
        @change="handleDesktopImageChange"
      />
      <label for="useDesktopImage" class="form-check-label">{{ $t('apps.use_desktop_image') }}</label>
    </div>
    
    <!-- 图片路径输入 -->
    <div class="input-group" v-if="!isDesktopImage">
      <input 
        type="file" 
        class="form-control" 
        @change="handleFileSelect" 
        accept="image/png,image/jpg,image/jpeg,image/gif,image/bmp,image/webp"
        style="width: 90px; flex: none;"
      />
      <input 
        type="text" 
        class="form-control form-control-enhanced monospace" 
        id="appImagePath" 
        :value="imagePath"
        @input="updateImagePath"
        @dragenter="handleDragEnter"
        @dragleave="handleDragLeave" 
        @dragover.prevent="" 
        @drop.prevent.stop="handleDrop"
        placeholder="选择图片文件或拖拽到此处"
      />
      <button 
        class="btn btn-outline-secondary" 
        type="button" 
        @click="openCoverFinder"
        :disabled="!appName"
      >
        <i class="fas fa-search me-1"></i>{{ $t('apps.find_cover') }}
      </button>
    </div>
    
    <!-- 图片预览 -->
    <div v-if="!isDesktopImage && imagePath" class="image-preview mt-3">
      <img :src="getImagePreviewUrl()" alt="图片预览" @error="handleImageError" />
    </div>
    
    <div class="field-hint">{{ $t('apps.image_desc') }}</div>
    
    <!-- 封面查找器 -->
    <CoverFinder 
      :visible="showCoverFinder"
      :search-term="appName"
      @close="closeCoverFinder"
      @cover-selected="handleCoverSelected"
      @loading="handleCoverLoading"
      @error="handleCoverError"
    />
  </div>
</template>

<script>
import CoverFinder from './CoverFinder.vue';
import { validateFile } from '../utils/validation.js';
import { getImagePreviewUrl } from '../utils/imageUtils.js';

export default {
  name: 'ImageSelector',
  components: {
    CoverFinder
  },
  props: {
    imagePath: {
      type: String,
      default: ''
    },
    appName: {
      type: String,
      default: ''
    }
  },
  data() {
    return {
      showCoverFinder: false,
      coverLoading: false,
      dragCounter: 0
    };
  },
  computed: {
    isDesktopImage() {
      return this.imagePath === 'desktop';
    }
  },
  methods: {
    /**
     * 处理桌面图片选择变化
     */
    handleDesktopImageChange(event) {
      const useDesktop = event.target.checked;
      this.$emit('update-image', useDesktop ? 'desktop' : '');
    },
    
    /**
     * 更新图片路径
     */
    updateImagePath(event) {
      this.$emit('update-image', event.target.value);
    },
    
    /**
     * 处理文件选择
     */
    handleFileSelect(event) {
      const file = event.target.files[0];
      if (file) {
        const validation = validateFile(file);
        if (validation.isValid) {
          // 如果是Electron环境，使用webUtils获取路径
          if (window.electron?.webUtils?.getPathForFile) {
            this.$emit('update-image', window.electron.webUtils.getPathForFile(file));
          } else {
            // 否则创建临时URL用于预览
            const url = URL.createObjectURL(file);
            this.$emit('update-image', url);
            this.$emit('image-error', '浏览器环境下无法获取完整路径，请检查并手动调整路径');
          }
        } else {
          this.$emit('image-error', validation.message);
        }
      }
    },
    
    /**
     * 处理拖拽进入
     */
    handleDragEnter(event) {
      event.preventDefault();
      this.dragCounter++;
      this.$emit('image-error', '杂鱼~快放进来呀~');
    },
    
    /**
     * 处理拖拽离开
     */
    handleDragLeave(event) {
      event.preventDefault();
      this.dragCounter--;
      if (this.dragCounter === 0) {
        this.$emit('image-error', '');
      }
    },
    
    /**
     * 处理拖拽放置
     */
    handleDrop(event) {
      event.preventDefault();
      this.dragCounter = 0;
      
      const file = event.dataTransfer.files[0];
      if (file) {
        const validation = validateFile(file);
        if (validation.isValid) {
          if (window.electron?.webUtils?.getPathForFile) {
            this.$emit('update-image', window.electron.webUtils.getPathForFile(file));
          } else {
            const url = URL.createObjectURL(file);
            this.$emit('update-image', url);
          }
          this.$emit('image-error', '');
        } else {
          this.$emit('image-error', validation.message);
        }
      } else {
        this.$emit('image-error', '其他地方不可以！');
      }
    },
    
    /**
     * 获取图片预览URL
     */
    getImagePreviewUrl() {
      return getImagePreviewUrl(this.imagePath);
    },
    
    /**
     * 处理图片加载错误
     */
    handleImageError() {
      this.$emit('image-error', '图片加载失败，请检查文件路径');
    },
    
    /**
     * 打开封面查找器
     */
    openCoverFinder() {
      if (!this.appName) {
        this.$emit('image-error', '请先输入应用名称');
        return;
      }
      
      this.showCoverFinder = true;
    },
    
    /**
     * 关闭封面查找器
     */
    closeCoverFinder() {
      this.showCoverFinder = false;
    },
    
    /**
     * 处理封面选择
     */
    handleCoverSelected(coverData) {
      this.$emit('update-image', coverData.path);
      this.showCoverFinder = false;
    },
    
    /**
     * 处理封面加载状态
     */
    handleCoverLoading(loading) {
      this.coverLoading = loading;
    },
    
    /**
     * 处理封面错误
     */
    handleCoverError(error) {
      this.$emit('image-error', error);
    }
  }
};
</script>

<style scoped>
.monospace {
  font-family: monospace;
}

.image-preview {
  max-width: 300px;
  max-height: 200px;
  border: 2px dashed #dee2e6;
  border-radius: 0.375rem;
  padding: 1rem;
  text-align: center;
  background-color: #f8f9fa;
}

.image-preview img {
  max-width: 100%;
  max-height: 150px;
  object-fit: contain;
  border-radius: 0.25rem;
  box-shadow: 0 2px 4px rgba(0, 0, 0, 0.1);
}

.input-group .form-control[type="file"] {
  border-top-right-radius: 0;
  border-bottom-right-radius: 0;
}

.input-group .form-control:not([type="file"]) {
  border-left: none;
  border-right: none;
  border-radius: 0;
}

.input-group .btn {
  border-top-left-radius: 0;
  border-bottom-left-radius: 0;
}

.btn:disabled {
  opacity: 0.6;
  cursor: not-allowed;
}

/* 拖拽状态样式 */
.form-control-enhanced[data-dragging="true"] {
  border-color: #0d6efd;
  background-color: #e7f1ff;
}

/* 响应式设计 */
@media (max-width: 768px) {
  .input-group {
    flex-direction: column;
  }
  
  .input-group .form-control,
  .input-group .btn {
    border-radius: 0.375rem !important;
    margin-bottom: 0.5rem;
  }
  
  .input-group .form-control:not(:last-child) {
    margin-bottom: 0.5rem;
  }
  
  .image-preview {
    max-width: 100%;
  }
}
</style> 