//
//  main.cpp
//  lanau
//
//  Created by René Descartes Domingos Muala on 10/05/21.
//

#include <iostream>
#include "decoders/wav/wav.hpp"
#include "RtAudio.h"

// Two channel sawtooth wave generator
int saw(void * outputBuffer, void * inputBuffer, unsigned int nBufferFrames, double streamTime, RtAudioStreamStatus status, void * userData){
    int * buffer = (int*) outputBuffer;
    int * value = (int*)userData;
    static int stage = 0;
    if(stage >= 6703104) {
        return 1;
    } if(status)
        std::cout << "Stream underflow detected!" << std::endl;
    // Write interleaved data
    for (int i = 0 ; i < nBufferFrames; i++){
        // resolve bug {only righr }
            *buffer++ = value[stage + i];
            *buffer++ = value[stage + i+2];
            //std::cout << (int)value[i] << ", " << stage << std::endl;
    } stage += nBufferFrames;
    return 0;
}

int main(int argc, const char * argv[]) {
   
    wav music;
    
    music.pull("Music2.wav");
    
    std::cout << "Genre: " << music.get_genre() << std::endl;
    std::cout << "Album Title: " << music.get_album_title() << std::endl;
    std::cout << "Artist: " << music.get_artist() << std::endl;
    std::cout << "Copyright: " << music.get_copyright() << std::endl;
    std::cout << "Date: " << music.get_creation_date() << std::endl;
    std::cout << "Engineer: " << music.get_engineer() << std::endl;
    std::cout << "Keywords: " << music.get_keywords() << std::endl;
    std::cout << "Software: " << music.get_software() << std::endl;
    std::cout << "Source: " << music.get_source() << std::endl;
    std::cout << "Technician: " << music.get_technician() << std::endl;
    std::cout << "Comment: " << music.get_text_comment() << std::endl;
    std::cout << "Track Number: " << music.get_track_number() << std::endl;
    std::cout << "Track Title: " << music.get_track_tittle() << std::endl;
    
    RtAudio handler;
    
    if (handler.getDeviceCount() == 0) {
        std::cout << "Unable to find devices" << std::endl;
        exit(0);
    }
    
    RtAudio::StreamParameters paramenters;
    paramenters.deviceId = handler.getDefaultOutputDevice();
    paramenters.nChannels = music.get_fmt_chunck()->numChannels;
    paramenters.firstChannel = 0;

    std::cout << music.get_data_chunck()->SubChunk2Size << std::endl;
    std::cout << music.get_fmt_chunck()->sampleRate << std::endl;
    std::cout << music.get_fmt_chunck()->byteRate << std::endl;
    std::cout << music.get_fmt_chunck()->bitsPerSample << std::endl;
    getchar();
    
    int sampleRate = 44100;
    unsigned int bufferFrame = (176400)/2;
    
    RtAudio::StreamOptions options;
    options.flags = RTAUDIO_MINIMIZE_LATENCY;
    options.flags = RTAUDIO_SCHEDULE_REALTIME;
//    options.flags = RTAUDIO_NONINTERLEAVED;
    
    try {
        handler.openStream(&paramenters, nullptr, RTAUDIO_SINT32, sampleRate, &bufferFrame, saw, music.get_data_chunck()->samples, &options);
        handler.startStream();
    } catch (RtAudioError& e) {
        e.printMessage();
        exit(0);
    }
    
//    std::cout << "playing, press <enter> to stop" << std::endl;
    getchar();
    
    try {
        handler.stopStream();
    } catch (RtAudioError& e) {
        e.printMessage();
    }
    
    if (handler.isStreamOpen()) handler.closeStream();
    
    return 0;
    
}

