#ifndef SOUNDMANAGER_H
#define SOUNDMANAGER_H

#include <QVector>
#include <QSoundEffect>

class SoundManager
{

public:

    SoundManager();

    static void LoadSounds();
    static void FreeSounds();
    static void Play(const int index, const bool loop);
    static void Stop(const int index);

    static void ToggleMute();

private:

    static bool muted;
    static int nSounds;
    static QVector <QSoundEffect *> sounds;

};

#endif // SOUNDMANAGER_H
