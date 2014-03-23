#include "soundmanager.h"

QVector <QSoundEffect *> SoundManager::sounds;
int SoundManager::nSounds = 17;
bool SoundManager::muted = false;

SoundManager::SoundManager()
{
}

void SoundManager::LoadSounds()
{

    if (!sounds.empty()) {
        return;
    }

    sounds.resize(nSounds);

    for (int i=0; i<nSounds; ++i) {
        sounds[i] = new QSoundEffect();      
    }

    sounds[0]->setSource(QUrl::fromLocalFile("assets/sounds/0_wind_mark_diangelo.wav"));
    sounds[1]->setSource(QUrl::fromLocalFile("assets/sounds/1_firebox_rift.wav"));
    sounds[2]->setSource(QUrl::fromLocalFile("assets/sounds/2_firebox_norift.wav"));
    sounds[3]->setSource(QUrl::fromLocalFile("assets/sounds/3_loading.wav"));
    sounds[4]->setSource(QUrl::fromLocalFile("assets/sounds/4_specifyurl.wav"));
    sounds[5]->setSource(QUrl::fromLocalFile("assets/sounds/5_newentrance.wav"));
    sounds[6]->setSource(QUrl::fromLocalFile("assets/sounds/6_flight.wav"));
    sounds[7]->setSource(QUrl::fromLocalFile("assets/sounds/7_noflight.wav"));
    sounds[8]->setSource(QUrl::fromLocalFile("assets/sounds/8_room_deadend.wav"));
    sounds[9]->setSource(QUrl::fromLocalFile("assets/sounds/9_room_singleentrance.wav"));
    sounds[10]->setSource(QUrl::fromLocalFile("assets/sounds/10_room_gallery.wav"));
    sounds[11]->setSource(QUrl::fromLocalFile("assets/sounds/11_room_text.wav"));
    sounds[12]->setSource(QUrl::fromLocalFile("assets/sounds/12_room_entrances.wav"));
    sounds[13]->setSource(QUrl::fromLocalFile("assets/sounds/13_room_entrancesimages.wav"));
    sounds[14]->setSource(QUrl::fromLocalFile("assets/sounds/14_room.wav"));
    sounds[15]->setSource(QUrl::fromLocalFile("assets/sounds/15_reset.wav"));
    sounds[16]->setSource(QUrl::fromLocalFile("assets/sounds/16_back.wav"));


}

void SoundManager::FreeSounds() {

    if (sounds.empty()) {
        return;
    }

    for (int i=0; i<nSounds; ++i) {
        delete sounds[i];
    }

    sounds.clear();

}

void SoundManager::Play(const int index, const bool loop)
{

    if (loop) {
        //sounds[index]->setLoopCount(QSoundEffect::Infinite); //seems to be a bug with this (defined as -2)
        sounds[index]->setLoopCount(100);
    }
    else {
        sounds[index]->setLoopCount(0);
    }

    if (sounds[index]->volume() == 0.0 && !muted) {
        sounds[index]->setVolume(0.75);
        sounds[index]->setMuted(false);
    }

    sounds[index]->play();

}

void SoundManager::Stop(const int index)
{

    sounds[index]->stop();
    //sounds[index]->setMuted(true);

}

void SoundManager::ToggleMute()
{

    muted = !muted;

    if (muted) {
        for (int i=0; i<nSounds; ++i) {
            sounds[i]->setVolume(0.0);
        }
    }
    else {
        for (int i=0; i<nSounds; ++i) {
            sounds[i]->setVolume(1.0);
        }
    }

}
