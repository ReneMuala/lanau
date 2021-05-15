//
//  mp3.hpp
//  lanau
//
//  Created by René Descartes Domingos Muala on 10/05/21.
//

#ifndef mp3_hpp
#define mp3_hpp

#include <iostream>

class mp3 {
public:
    /// The Enhanced tag is an extra data block before an ID3v1 tag, which extends the title, artist and album fields to 60 bytes each, offers a freetext genre, a one-byte (values 0–5) speed and the start and stop time of the music in the MP3 file, e.g., for fading in. If none of the fields are used, it will be automatically omitted.
    struct TAGplus {
        char header [4]; // "TAG+"
        char title  [60];
        char artist [60];
        char album  [60];
        /// 0=unset, 1=slow, 2= medium, 3=fast, 4=hardcore
        int speed; // (not implemented by lanau)
        char genre [30];
        /// the start of the music as mmm:ss
        char start_time[6]; // (not implemented by lanau)
        /// the start of the music as mmm:ss
        char end_time[6]; // (not implemented by lanau)
    };
    typedef TAGplus eTAG;
    
    typedef enum {
            #include "../../genres/ID3"
            #include "../../genres/Winamp"
            #include "../../genres/Lanau"
            none = 255
    } GenresList;
    
    /// ID3 is a metadata container most often used in conjunction with the MP3 audio file format. It allows information such as the title, artist, album, track number, and other information about the file to be stored in the file itself. (https://en.wikipedia.org/wiki/ID3)
    struct ID3v1 {
        char header [3]; // "ID3"
        char title  [30];
        char artist [30];
        char album  [30];
        char year   [4];
        char comment[28+2];
        /// If a track number is stored, this byte contains a binary 0.
        bool zero_byte;
        /// The number of the track on the album, or 0. Invalid, if previous byte is not a binary 0.
        bool track;
        GenresList genre;
    };
private:
    
    FILE * file;
    eTAG  etagMetadata;
    ID3v1 id3v1Metadata;
    
public:
    mp3();
    
    ~mp3();
};


#endif /* mp3_hpp */
