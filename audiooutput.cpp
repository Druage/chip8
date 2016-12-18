#include "audiooutput.h"

#include <QSoundEffect>
#include <QFile>
#include <QDebug>

AudioOutput::AudioOutput(QObject *parent)
    : QObject(parent),
      m_soundEffect( new QSoundEffect( this ) )
{
    if ( QFile::exists( "beep.wav" ) ) {
        m_soundEffect->setSource( QUrl::fromLocalFile( "beep.wav" ));
    } else {
        qDebug() << "beep.wav could not be found. Sound effects will not work!";
    }
}

void AudioOutput::renderAudioFrame() {

    m_soundEffect->play();
}
