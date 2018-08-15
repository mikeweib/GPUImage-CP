//
// Created by Mike Pro on 2018/8/14.
//

#ifndef GPUIMAGE_CP_GPUIMAGEFRAMEBUFFERCACHE_H
#define GPUIMAGE_CP_GPUIMAGEFRAMEBUFFERCACHE_H

#include "GPCP-Common.h"
#include "GPUImageFramebuffer.h"

NS_GPCP_BEGIN

    class GPUImageFramebuffer;

    class GPUImageFramebufferCache {
    public:
        GPUImageFramebufferCache();

        virtual ~GPUImageFramebufferCache();

        GPUImageFramebuffer *fetchFramebufferForSize(float w, float h, GPUTextureOptions textureOptions, bool onlyTexture);

        GPUImageFramebuffer *fetchFramebufferForSize(float w, float h, bool onlyTexture);

        void returnFramebufferToCache(GPUImageFramebuffer *framebuffer);

        void purgeAllUnassignedFramebuffers();

        void addFramebufferToActiveImageCaptureList(GPUImageFramebuffer *framebuffer);

        void removeFramebufferFromActiveImageCaptureList(GPUImageFramebuffer *framebuffer);

    private:
#if TARGET_OF_IOS
        dispatch_queue_t _framebufferCacheQueue;
#endif // TARGET_OF_IOS

        vector<GPUImageFramebuffer *> _activeImageCaptureList;
        unordered_map<string, GPUImageFramebuffer *> _framebufferCache;
        unordered_map<string, int> _framebufferTypeCounts;

        string _tempString;

        const string &hashForSize(float w, float h, GPUTextureOptions textureOptions, bool onlyTexture);
    };

NS_GPCP_END


#endif //GPUIMAGE_CP_GPUIMAGEFRAMEBUFFERCACHE_H
