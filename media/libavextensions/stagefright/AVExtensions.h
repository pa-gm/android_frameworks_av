/*
 * Copyright (c) 2013 - 2018, 2020 The Linux Foundation. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *     * Redistributions of source code must retain the above copyright
 *      notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above
 *       copyright notice, this list of conditions and the following
 *       disclaimer in the documentation and/or other materials provided
 *      with the distribution.
 *     * Neither the name of The Linux Foundation nor the names of its
 *      contributors may be used to endorse or promote products derived
 *       from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS
 * BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
 * BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
 * OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN
 * IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
#ifndef _AV_EXTENSIONS_H_
#define _AV_EXTENSIONS_H_

#include <media/DataSource.h>
#include <media/stagefright/MediaExtractor.h>
#include <SharedMemoryBuffer.h>
#include <common/AVExtensionsCommon.h>
#include <system/audio.h>
#include <media/IOMX.h>
#include <camera/android/hardware/ICamera.h>
#include <media/mediarecorder.h>
#include "mpeg2ts/ESQueue.h"

namespace android {

struct ACodec;
struct CodecBase;
struct MediaCodec;
struct ALooper;
class IMediaExtractor;
class MediaExtractor;
class AudioParameter;
class MetaData;
class CameraParameters;
class MediaBuffer;
class CameraSource;
class CameraSourceTimeLapse;
class ICameraRecordingProxy;
struct Size;
class MPEG4Writer;
struct AudioSource;

/*
 * Factory to create objects of base-classes in libstagefright
 */
struct AVFactory {
    virtual sp<ACodec> createACodec();
    virtual ElementaryStreamQueue* createESQueue(
            ElementaryStreamQueue::Mode mode, uint32_t flags = 0);
    virtual CameraSource *CreateCameraSourceFromCamera(
            const sp<hardware::ICamera> &camera,
            const sp<ICameraRecordingProxy> &proxy,
            int32_t cameraId,
            const String16& clientName,
            uid_t clientUid,
            pid_t clientPid,
            Size videoSize,
            int32_t frameRate,
            const sp<IGraphicBufferProducer>& surface,
            bool storeMetaDataInVideoBuffers = true);

    virtual CameraSourceTimeLapse *CreateCameraSourceTimeLapseFromCamera(
            const sp<hardware::ICamera> &camera,
            const sp<ICameraRecordingProxy> &proxy,
            int32_t cameraId,
            const String16& clientName,
            uid_t clientUid,
            pid_t clientPid,
            Size videoSize,
            int32_t videoFrameRate,
            const sp<IGraphicBufferProducer>& surface,
            int64_t timeBetweenFrameCaptureUs,
            bool storeMetaDataInVideoBuffers = true);
    virtual AudioSource* createAudioSource(
            const audio_attributes_t *attr,
            const content::AttributionSourceState& attributionSource,
            uint32_t sampleRate,
            uint32_t channels,
            uint32_t outSampleRate = 0,
            audio_port_handle_t selectedDeviceId = AUDIO_PORT_HANDLE_NONE,
            audio_microphone_direction_t selectedMicDirection = MIC_DIRECTION_UNSPECIFIED,
            float selectedMicFieldDimension = MIC_FIELD_DIMENSION_NORMAL);
    virtual MPEG4Writer *CreateMPEG4Writer(int fd);

    // ----- NO TRESSPASSING BEYOND THIS LINE ------
    DECLARE_LOADABLE_SINGLETON(AVFactory);
};

/*
 * Common delegate to the classes in libstagefright
 */
struct AVUtils {

    virtual status_t convertMetaDataToMessage(
            const MetaDataBase *meta, sp<AMessage> *format);
    virtual status_t convertMetaDataToMessage(
            const sp<MetaData> &meta, sp<AMessage> *format);
    virtual status_t convertMessageToMetaData(
            const sp<AMessage> &msg, sp<MetaData> &meta);
    virtual status_t mapMimeToAudioFormat( audio_format_t& format, const char* mime);
    virtual status_t sendMetaDataToHal(const sp<MetaData>& meta, AudioParameter *param);
    virtual sp<MediaCodec> createCustomComponentByName(const sp<ALooper> &looper,
                const char* mime, bool encoder, const sp<AMessage> &format);
    virtual bool isEnhancedExtension(const char *extension);
    virtual bool isAudioSourceAggregate(const audio_attributes_t *, uint32_t channelCount);

    virtual bool hasAudioSampleBits(const sp<MetaData> &);
    virtual bool hasAudioSampleBits(const sp<AMessage> &);
    virtual int getAudioSampleBits(const sp<MetaData> &);
    virtual int getAudioSampleBits(const sp<AMessage> &);
    virtual audio_format_t updateAudioFormat(audio_format_t audioFormat,
            const sp<MetaData> &);

    virtual audio_format_t updateAudioFormat(audio_format_t audioFormat,
            const sp<AMessage> &);
    virtual bool canOffloadStream(const sp<MetaData> &meta);
    virtual bool useQCHWEncoder(const sp<AMessage> &,Vector<AString> *) { return false; }

    virtual int32_t getAudioMaxInputBufferSize(audio_format_t audioFormat,
            const sp<AMessage> &);

    virtual bool mapAACProfileToAudioFormat(const sp<MetaData> &,
            audio_format_t &,
            uint64_t /*eAacProfile*/);

    virtual bool mapAACProfileToAudioFormat(const sp<AMessage> &,
            audio_format_t &,
            uint64_t /*eAacProfile*/);

    virtual void extractCustomCameraKeys(
            const CameraParameters& /*params*/, sp<MetaData> &/*meta*/) {}
    virtual void printFileName(int /*fd*/) {}

    // deprecate these two use one with 3 arguments
    virtual void addDecodingTimesFromBatch(MediaBuffer * /*buf*/,
            List<int64_t> &/*decodeTimeQueue*/) {}
    virtual void addDecodingTimesFromBatch(MediaBufferBase * /*buf*/,
            List<int64_t> &/*decodeTimeQueue*/) {}

    virtual void addDecodingTimesFromBatch(MediaBufferBase * /*buf*/,
            List<int64_t> &/*decodeTimeQueue*/, int64_t /*time-offset-us*/) {}

    virtual bool canDeferRelease(const sp<MetaData> &/*meta*/) { return false; }
    virtual void setDeferRelease(sp<MetaData> &/*meta*/) {}

    virtual bool isAudioMuxFormatSupported(const char *mime);
    virtual void cacheCaptureBuffers(sp<hardware::ICamera> camera, video_encoder encoder);
    virtual void getHFRParams(bool*, int32_t*, sp<AMessage>);
    virtual int64_t overwriteTimeOffset(bool, int64_t, int64_t *, int64_t, int32_t);
    virtual void  getCustomCodecsLocation(std::string *outPath);
    virtual void  getCustomCodecsPerformanceLocation(std::string *outPath);

    virtual void setIntraPeriod(
                int nPFrames, int nBFrames, sp<IOMXNode> mOMXNode);

    virtual const char *getComponentRole(bool isEncoder, const char *mime);


    // Used by ATSParser
    virtual bool IsHevcIDR(const sp<ABuffer> &accessUnit);

    virtual sp<DataSource> wrapTraceDataSource(const sp<DataSource> &dataSource);
    virtual sp<IMediaExtractor> wrapTraceMediaExtractor(const sp<IMediaExtractor> &extractor);
    virtual sp<AMessage> fillExtradata(sp<MediaCodecBuffer>&, sp<AMessage> &format);

    // ----- NO TRESSPASSING BEYOND THIS LINE ------
    DECLARE_LOADABLE_SINGLETON(AVUtils);
};

}

#endif // _AV_EXTENSIONS__H_
