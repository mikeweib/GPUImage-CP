
#ifndef OF_3D_ENGINE_GPCP_COMMON_H
#define OF_3D_ENGINE_GPCP_COMMON_H


#include <memory>
#include <list>
#include <unordered_map>
#include <vector>
#include <set>
#include <stdint.h>
#include <cstdio>
#include <cstdarg>
#include <cmath>
#include <ctime>
#include <cstdlib>
#include <string>
#include <cstring>
#include <fstream>
#include <algorithm>
#include <cfloat>
#include <stack>
#include <unordered_map>

#define GPCP_SAFE_DELETE(ptr) { if((ptr) != nullptr) { delete (ptr); (ptr) = nullptr; } }
#define GPCP_SAFE_DELETE_ARRAY(array)  { if((array) != nullptr) { delete [] array; (array) = nullptr;} }
#define GPCP_SAFE_FREE(p)             do { if(p) { free(p); (p) = nullptr; } } while(0)



// 引用 opengles 头文件
#if TARGET_OS_IPHONE // IOS

#import <OpenGLES/ES1/gl.h>
#import <OpenGLES/ES1/glext.h>

#import <OpenGLES/ES2/gl.h>
#import <OpenGLES/ES2/glext.h>

#elif __ANDROID__ // Android

#include <GLES/gl.h>
#include <GLES/glext.h>

#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>

#endif



// 引用 android log 头文件
#if defined (__ANDROID__)

#include <android/log.h>

#endif



// 定义命名空间
#ifdef __cplusplus
#define NS_GPCP_BEGIN                     namespace GPCP {
#define NS_GPCP_END                       }
#define USING_NS_GPCP                     using namespace GPCP
#else
#define NS_CME_BEGIN
#define NS_CME_END
#define USING_NS_CME
#endif



// 使用标准库命名空间
using namespace std;


#ifdef __cplusplus
extern "C" {
#endif

// 定义区分平台的宏定义
#if TARGET_OF_IOS
int gettid();  // 定义 ios gettid.

NS_GPCP_BEGIN
class GPUImageContext; // 定义GPUImageContext, 为下面 ios 接口使用
NS_GPCP_END

void runOnMainQueueWithoutDeadlocking(void (^block)(void));
void runSynchronouslyOnVideoProcessingQueue(void (^block)(void));
void runAsynchronouslyOnVideoProcessingQueue(void (^block)(void));
void runSynchronouslyOnContextQueue(GPUImageContext *context, void (^block)(void));
void runAsynchronouslyOnContextQueue(GPUImageContext *context, void (^block)(void));

#define runOnMainQueue                          runOnMainQueueWithoutDeadlocking(^{
#define runSynchronouslyOnVideoProcessingStart  runSynchronouslyOnVideoProcessingQueue(^{
#define runAsynchronouslyOnVideoProcessingStart runAsynchronouslyOnVideoProcessingQueue(^{
#define runSynchronouslyOnContextStart          runSynchronouslyOnContextQueue(^{
#define runAsynchronouslyOnContextStart         runAsynchronouslyOnContextQueue(^{

#define runProcessingEnd });

#define __BLOCK __block

#elif __ANDROID__ // Android

#define runOnMainQueue
#define runSynchronouslyOnVideoProcessingStart
#define runAsynchronouslyOnVideoProcessingStart
#define runSynchronouslyOnContextStart
#define runAsynchronouslyOnContextStart
#define runProcessingEnd

#define __BLOCK

#endif // TARGET_OF_IOS && Android


void logInfo(const char *format, ...);


#ifdef __cplusplus
}
#endif



// 定义 LOG
#ifdef GPCP_DEBUG
#define LOGD(format, ...) logInfo(format, ##__VA_ARGS__)
#else
#define LOGD(...) do {} while (0)
#endif // CM_DEBUG


#define GPCP_EPSILON 0.0000001f


#endif // OF_3D_ENGINE_GPCP_COMMON_H