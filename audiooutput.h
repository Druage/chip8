#pragma once

#include <QObject>

class QSoundEffect;

class AudioOutput : public QObject
{
    Q_OBJECT
public:
    explicit AudioOutput( QObject *parent = nullptr );

signals:

public slots:
    void renderAudioFrame();

private:
    QSoundEffect *m_soundEffect;
};

