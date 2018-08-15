//
// Created by Mike Pro on 2018/8/14.
//

#include "GPUImageFramebufferCache.h"

#include "GPUImageFramebuffer.h"

USING_NS_GPCP;

NS_GPCP_BEGIN

    GPUImageFramebufferCache::GPUImageFramebufferCache() {

    }

    GPUImageFramebufferCache::~GPUImageFramebufferCache() {

    }

    GPUImageFramebuffer *GPUImageFramebufferCache::fetchFramebufferForSize(
            float w, float h, GPUTextureOptions textureOptions, bool onlyTexture) {

        __BLOCK GPUImageFramebuffer *framebufferFromCache = nullptr;
        runSynchronouslyOnVideoProcessingStart

        string lookupHash = hashForSize(w, h, textureOptions, onlyTexture);

        unordered_map<string, int>::iterator iter = _framebufferTypeCounts.find(lookupHash);

        if (iter == _framebufferTypeCounts.end()) {
            // Nothing in the cache, create a new framebuffer to use
            framebufferFromCache = GPUImageFramebuffer::create(w, h, textureOptions, onlyTexture);

        } else {
            int numberOfMatchingTextures = iter->second;

            // Something found, pull the old framebuffer and decrement the count
            int currentTextureID = (numberOfMatchingTextures - 1);

            while ((framebufferFromCache == nullptr) && (currentTextureID >= 0)) {
                char buf[256];
                sprintf(buf, "%s-%ld", lookupHash.c_str(), (long) currentTextureID);

                unordered_map<string, GPUImageFramebuffer *>::iterator it = _framebufferCache.find(buf);

                // Test the values in the cache first, to see if they got invalidated behind our back
                if (it != _framebufferCache.end()) {
                    framebufferFromCache = it->second;

                    _framebufferCache.erase(it);
                }
                currentTextureID--;
            }

            currentTextureID++;

            _framebufferTypeCounts[lookupHash] = currentTextureID;

            if (framebufferFromCache == nullptr) {
                framebufferFromCache = GPUImageFramebuffer::create(w, h, textureOptions, onlyTexture);
            }
        }

        runProcessingEnd

        framebufferFromCache->lock();
        return framebufferFromCache;
    }

    GPUImageFramebuffer *GPUImageFramebufferCache::fetchFramebufferForSize(float w, float h, bool onlyTexture) {
        GPUTextureOptions defaultTextureOptions;
        defaultTextureOptions.minFilter = GL_LINEAR;
        defaultTextureOptions.magFilter = GL_LINEAR;
        defaultTextureOptions.wrapS = GL_CLAMP_TO_EDGE;
        defaultTextureOptions.wrapT = GL_CLAMP_TO_EDGE;
        defaultTextureOptions.internalFormat = GL_RGBA;
        defaultTextureOptions.format = GL_BGRA_EXT;
        defaultTextureOptions.type = GL_UNSIGNED_BYTE;

        return fetchFramebufferForSize(w, h, defaultTextureOptions, onlyTexture);
    }

    void GPUImageFramebufferCache::returnFramebufferToCache(GPUImageFramebuffer *framebuffer) {
        framebuffer->clearAllLocks();

        runSynchronouslyOnVideoProcessingStart

        string lookupHash = hashForSize(framebuffer->_width, framebuffer->_height, framebuffer->_textureOptions, framebuffer->_missingFramebuffer);

        unordered_map<string, int>::iterator iter = _framebufferTypeCounts.find(lookupHash);

        if (iter != _framebufferTypeCounts.end()) {
            int numberOfMatchingTextures = iter->second;
            char buf[256];
            sprintf(buf, "%s-%ld", lookupHash.c_str(), (long) numberOfMatchingTextures);

            _framebufferCache[buf] = framebuffer;
            _framebufferTypeCounts[lookupHash] = numberOfMatchingTextures + 1;
        }

        runProcessingEnd
    }

    void GPUImageFramebufferCache::purgeAllUnassignedFramebuffers() {
        runAsynchronouslyOnVideoProcessingStart

        _framebufferCache.clear();
        _framebufferTypeCounts.clear;

#if TARGET_OS_IPHONE
        CVOpenGLESTextureCacheFlush([[GPUImageContext sharedImageProcessingContext] coreVideoTextureCache], 0);
#else

        runProcessingEnd
    }

    void GPUImageFramebufferCache::addFramebufferToActiveImageCaptureList(GPUImageFramebuffer *framebuffer) {
        runAsynchronouslyOnVideoProcessingStart

        _activeImageCaptureList.emplace_back(framebuffer);

        runProcessingEnd
    }

    void GPUImageFramebufferCache::removeFramebufferFromActiveImageCaptureList(GPUImageFramebuffer *framebuffer) {
        runAsynchronouslyOnVideoProcessingStart

        vector<GPUImageFramebuffer *>::iterator iter = find(_activeImageCaptureList.begin(), _activeImageCaptureList.end(), framebuffer);
        if (iter != _activeImageCaptureList.end()) {
            _activeImageCaptureList.erase(iter);
        }

        runProcessingEnd
    }


#pragma mark -
#pragma mark Private

    const string &GPUImageFramebufferCache::hashForSize(float w, float h, GPUTextureOptions textureOptions, bool onlyTexture) {
        char buf[256];
        if (onlyTexture) {
            sprintf(buf, "%.1fx%.1f-%d:%d:%d:%d:%d:%d:%d-NOFB",
                    w, h, textureOptions.minFilter, textureOptions.magFilter, textureOptions.wrapS, textureOptions.wrapT, textureOptions.internalFormat,
                    textureOptions.format, textureOptions.type);
        } else {
            sprintf(buf, "%.1fx%.1f-%d:%d:%d:%d:%d:%d:%d",
                    w, h, textureOptions.minFilter, textureOptions.magFilter, textureOptions.wrapS, textureOptions.wrapT, textureOptions.internalFormat,
                    textureOptions.format, textureOptions.type);
        }
        _tempString = buf;
        return _tempString;
    }

NS_GPCP_END