﻿#ifndef GSTPLAYER_H
#define GSTPLAYER_H

#include "overlay.h"
#include "videothread.h"
#include <Fact/Fact.h>
#include <QAbstractVideoSurface>
#include <QtCore>

class GstPlayer : public Fact
{
    Q_OBJECT
    Q_PROPERTY(QAbstractVideoSurface *videoSurface MEMBER m_videoSurface)
    Q_PROPERTY(ConnectionState connectionState READ getConnectionState NOTIFY connectionStateChanged)

public:
    static const int THREAD_STOP_TIMEOUT = 500;
    static const int RECONNECT_TIMEOUT = 15000;
    enum ConnectionState { STATE_UNCONNECTED, STATE_CONNECTING, STATE_CONNECTED };
    enum MediaType { mtImage, mtVideo };
    enum SourceType { stUri, stRtsp, stTcp, stUdp, stWebcam };
    enum CodecType { ctH264, ctH265 };

    Q_ENUM(ConnectionState)
    explicit GstPlayer(Fact *parent = nullptr);
    ~GstPlayer() override;

    Fact *f_tune;
    Fact *f_active;
    Fact *f_record;
    Fact *f_reencoding;
    Fact *f_lowLatency;
    Overlay *f_overlay;

    Fact *f_sourceType;

    Fact *f_uriInput;
    Fact *f_rtspInput;
    Fact *f_rtspTcpForce;
    Fact *f_tcpInput;
    Fact *f_tcpPortInput;
    Fact *f_udpInput;
    Fact *f_udpCodecInput;
    Fact *f_webcamInput;

    ConnectionState getConnectionState() const;

    Q_INVOKABLE void snapshot() const;

    static QString getMediaFileName(MediaType type);

private:
    QAbstractVideoSurface *m_videoSurface = nullptr;
    VideoThread m_videoThread;
    QImage m_lastFrame;
    ConnectionState m_connectionState = STATE_UNCONNECTED;
    QTimer m_reconnectTimer;

    void setConnectionState(ConnectionState cs);

    void play();
    void stop();

    QString inputToUri();

    QStringList getAvailableWebcams();

private slots:
    void stopAndPlay();

    void onFrameReceived(const QImage &image);
    void onActiveValueChanged();
    void onRecordValueChanged();
    void onSourceTypeChanged();
    void onErrorOccured(const QString &error) const;
    void onReconnectTimerTimeout();

signals:
    void connectionStateChanged();
};

#endif //GSTPLAYER_H