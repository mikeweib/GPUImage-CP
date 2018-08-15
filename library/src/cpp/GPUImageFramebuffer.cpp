//
// Created by Mike Pro on 2018/8/14.
//

#include "GPUImageFramebuffer.h"

#include "GPUImageContext.h"
#include "GPUImageFramebufferCache.h"

USING_NS_GPCP;

NS_GPCP_BEGIN

    GPUImageFramebuffer *GPUImageFramebuffer::create(float w, float h) {
        GPUImageFramebuffer *fb = new GPUImageFramebuffer();
        fb->initWithSize(w, h);
        return fb;
    }

    GPUImageFramebuffer *GPUImageFramebuffer::create(float w, float h, GPUTextureOptions fboTextureOptions, bool onlyGenerateTexture) {
        GPUImageFramebuffer *fb = new GPUImageFramebuffer();
        fb->initWithSize(w, h, fboTextureOptions, onlyGenerateTexture);
        return fb;
    }

    GPUImageFramebuffer *GPUImageFramebuffer::create(float w, float h, GLuint inputTexture) {
        GPUImageFramebuffer *fb = new GPUImageFramebuffer();
        fb->initWithSize(w, h, inputTexture);
        return fb;
    }

    GPUImageFramebuffer::GPUImageFramebuffer() {

    }

    GPUImageFramebuffer::~GPUImageFramebuffer() {
        destroyFramebuffer();
    }


#pragma mark -
#pragma mark Usage

    void GPUImageFramebuffer::activateFramebuffer() {
        glBindFramebuffer(GL_FRAMEBUFFER, _framebuffer);
        glViewport(0, 0, (int) _width, (int) _height);
    }


#pragma mark -
#pragma mark Reference counting

    void GPUImageFramebuffer::lock() {
        if (_referenceCountingDisabled) {
            return;
        }

        _framebufferReferenceCount++;
    }

    void GPUImageFramebuffer::unlock() {
        if (_referenceCountingDisabled) {
            return;
        }

        _framebufferReferenceCount--;
        if (_framebufferReferenceCount < 1) {
            GPUImageContext::sharedFramebufferCache()->returnFramebufferToCache(this);
        }
    }

    void GPUImageFramebuffer::clearAllLocks() {
        _framebufferReferenceCount = 0;
    }

    void GPUImageFramebuffer::disableReferenceCounting() {
        _referenceCountingDisabled = true;
    }

    void GPUImageFramebuffer::enableReferenceCounting() {
        _referenceCountingDisabled = false;
    }


#pragma mark -
#pragma mark Image capture

    void GPUImageFramebuffer::restoreRenderTarget() {
#if TARGET_OS_IPHONE
        unlockAfterReading();
        CFRelease(_renderTarget);
#else
#endif
    }

    void GPUImageFramebuffer::lockForReading() {
#if TARGET_OS_IPHONE
        if (GPUImageContext::supportsFastTextureUpload()) {
            if (_readLockCount == 0) {
                CVPixelBufferLockBaseAddress(_renderTarget, 0);
            }
            _readLockCount++;
        }
#endif
    }

    void GPUImageFramebuffer::unlockAfterReading() {
#if TARGET_OS_IPHONE
        if (GPUImageContext::supportsFastTextureUpload()) {
            _readLockCount--;
            if (_readLockCount == 0) {
                CVPixelBufferUnlockBaseAddress(_renderTarget, 0);
            }
        }
#endif
    }

    int GPUImageFramebuffer::bytesPerRow() {
        if (GPUImageContext::supportsFastTextureUpload()) {
#if TARGET_OS_IPHONE
            return CVPixelBufferGetBytesPerRow(_renderTarget);
#else
            return _width * 4; // TODO: do more with this on the non-texture-cache side
#endif
        } else {
            return _width * 4;
        }
    }

    GLubyte *GPUImageFramebuffer::byteBuffer() {
#if TARGET_OS_IPHONE
        lockForReading();
        GLubyte * bufferBytes = CVPixelBufferGetBaseAddress(_renderTarget);
        unlockAfterReading();
        return bufferBytes;
#else
        return nullptr; // TODO: do more with this on the non-texture-cache side
#endif
    }


#pragma mark -
#pragma mark initWithSize

    void GPUImageFramebuffer::initWithSize(float w, float h) {
        GPUTextureOptions defaultTextureOptions;
        defaultTextureOptions.minFilter = GL_LINEAR;
        defaultTextureOptions.magFilter = GL_LINEAR;
        defaultTextureOptions.wrapS = GL_CLAMP_TO_EDGE;
        defaultTextureOptions.wrapT = GL_CLAMP_TO_EDGE;
        defaultTextureOptions.internalFormat = GL_RGBA;
        defaultTextureOptions.format = GL_BGRA_EXT;
        defaultTextureOptions.type = GL_UNSIGNED_BYTE;

        initWithSize(w, h, defaultTextureOptions, false);
    }

    void GPUImageFramebuffer::initWithSize(float w, float h, GPUTextureOptions fboTextureOptions, bool onlyGenerateTexture) {
        _textureOptions = fboTextureOptions;
        _width = w;
        _height = h;
        _framebufferReferenceCount = 0;
        _referenceCountingDisabled = false;
        _missingFramebuffer = onlyGenerateTexture;

        if (_missingFramebuffer) {
            runSynchronouslyOnVideoProcessingStart

            GPUImageContext::useImageProcessingContext();
            generateTexture();
            _framebuffer = 0;

            runProcessingEnd

        } else {
            generateFramebuffer();
        }
    }

    void GPUImageFramebuffer::initWithSize(float w, float h, GLuint inputTexture) {
        GPUTextureOptions defaultTextureOptions;
        defaultTextureOptions.minFilter = GL_LINEAR;
        defaultTextureOptions.magFilter = GL_LINEAR;
        defaultTextureOptions.wrapS = GL_CLAMP_TO_EDGE;
        defaultTextureOptions.wrapT = GL_CLAMP_TO_EDGE;
        defaultTextureOptions.internalFormat = GL_RGBA;
        defaultTextureOptions.format = GL_BGRA_EXT;
        defaultTextureOptions.type = GL_UNSIGNED_BYTE;

        _textureOptions = defaultTextureOptions;
        _width = w;
        _height = h;
        _framebufferReferenceCount = 0;
        _referenceCountingDisabled = true;

        _texture = inputTexture;
    }

    void GPUImageFramebuffer::generateFramebuffer() {
        runSynchronouslyOnVideoProcessingStart

        GPUImageContext::useImageProcessingContext();

        glGenFramebuffers(1, &_framebuffer);
        glBindFramebuffer(GL_FRAMEBUFFER, _framebuffer);

        // By default, all framebuffers on iOS 5.0+ devices are backed by texture caches, using one shared cache
        if (GPUImageContext::supportsFastTextureUpload()) {
#if TARGET_OS_IPHONE
            CVOpenGLESTextureCacheRef coreVideoTextureCache = [[GPUImageContext sharedImageProcessingContext] coreVideoTextureCache];
        // Code originally sourced from http://allmybrain.com/2011/12/08/rendering-to-a-texture-with-ios-5-texture-cache-api/

        CFDictionaryRef empty; // empty value for attr value.
        CFMutableDictionaryRef attrs;
        empty = CFDictionaryCreate(kCFAllocatorDefault, NULL, NULL, 0, &kCFTypeDictionaryKeyCallBacks, &kCFTypeDictionaryValueCallBacks); // our empty IOSurface properties dictionary
        attrs = CFDictionaryCreateMutable(kCFAllocatorDefault, 1, &kCFTypeDictionaryKeyCallBacks, &kCFTypeDictionaryValueCallBacks);
        CFDictionarySetValue(attrs, kCVPixelBufferIOSurfacePropertiesKey, empty);

        CVReturn err = CVPixelBufferCreate(kCFAllocatorDefault, (int)_size.width, (int)_size.height, kCVPixelFormatType_32BGRA, attrs, &renderTarget);
        if (err)
        {
            NSLog(@"FBO size: %f, %f", _size.width, _size.height);
            NSAssert(NO, @"Error at CVPixelBufferCreate %d", err);
        }

        err = CVOpenGLESTextureCacheCreateTextureFromImage (kCFAllocatorDefault, coreVideoTextureCache, renderTarget,
                                                            NULL, // texture attributes
                                                            GL_TEXTURE_2D,
                                                            _textureOptions.internalFormat, // opengl format
                                                            (int)_size.width,
                                                            (int)_size.height,
                                                            _textureOptions.format, // native iOS format
                                                            _textureOptions.type,
                                                            0,
                                                            &renderTexture);
        if (err)
        {
            NSAssert(NO, @"Error at CVOpenGLESTextureCacheCreateTextureFromImage %d", err);
        }

        CFRelease(attrs);
        CFRelease(empty);

        glBindTexture(CVOpenGLESTextureGetTarget(renderTexture), CVOpenGLESTextureGetName(renderTexture));
        _texture = CVOpenGLESTextureGetName(renderTexture);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, _textureOptions.wrapS);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, _textureOptions.wrapT);

        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, CVOpenGLESTextureGetName(renderTexture), 0);
#endif
        } else {
            generateTexture();

            glBindTexture(GL_TEXTURE_2D, _texture);

            glTexImage2D(GL_TEXTURE_2D, 0, _textureOptions.internalFormat, (int) _width, (int) _height, 0, _textureOptions.format,
                         _textureOptions.type, 0);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, _texture, 0);
        }

        glBindTexture(GL_TEXTURE_2D, 0);
        runProcessingEnd
    }

    void GPUImageFramebuffer::generateTexture() {
        glActiveTexture(GL_TEXTURE1);
        glGenTextures(1, &_texture);
        glBindTexture(GL_TEXTURE_2D, _texture);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, _textureOptions.minFilter);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, _textureOptions.magFilter);

        // This is necessary for non-power-of-two textures
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, _textureOptions.wrapS);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, _textureOptions.wrapT);
    }

    void GPUImageFramebuffer::destroyFramebuffer() {
        runSynchronouslyOnVideoProcessingStart

        GPUImageContext::useImageProcessingContext();

        if (_framebuffer) {
            glDeleteFramebuffers(1, &_framebuffer);
            _framebuffer = 0;
        }


        if (GPUImageContext::supportsFastTextureUpload() && (!_missingFramebuffer)) {
#if TARGET_OS_IPHONE
            if (_renderTarget) {
                CFRelease(_renderTarget);
                _renderTarget = nullptr;
            }

            if (_renderTexture) {
                CFRelease(_renderTexture);
                _renderTexture = nullptr;
         }
#endif
        } else {
            glDeleteTextures(1, &_texture);
        }

        runProcessingEnd
    }

NS_GPCP_END