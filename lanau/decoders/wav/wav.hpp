//
//  wav.hpp
//  lanau
//
//  Created by René Descartes Domingos Muala on 12/05/21.
//
/*
 Copyright (c) 2021 René Descartes Muala

 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:

 The above copyright notice and this permission notice shall be included in all
 copies or substantial portions of the Software.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 SOFTWARE.
 */
#ifndef dwav_hpp
#define dwav_hpp

#include <stdio.h>
#include <string>

/// WAVE decoder, The WAVE file format is a subset of Microsoft's RIFF specification for the storage of multimedia files. A RIFF file starts out with a file header followed by a sequence of data chunks. A WAVE file is often just a RIFF file with a single "WAVE" chunk which consists of two sub-chunks -- a "fmt " chunk specifying the data format and a "data" chunk containing the actual sample data. (13/05/2021 -  http://soundfile.sapp.org/doc/WaveFormat/)
class dwav {
public:
        
    // Wave instance dependent configuration, for audio IO libraries, based on RtAudio.
    struct RtAudioStreamConfiguration {
        RtAudioStreamConfiguration(dwav&);
        /// Decoder/encoder
        dwav * context;
        /// Channel number
        const int get_nChannels();
        /// Stream rate
        const int get_sampleRate();
        /// Buffer frame / rate
        const int get_bufferFrame();
        /// Audio length
        const int get_audioLen();
        /// audio samples
        const void * get_audioSamples();
        /// Stream options
        const unsigned int get_RtAudio_StreamOptions();
        /// Audio format
        const unsigned long get_RtAudio_AudioFormat();
    };
    
    typedef RtAudioStreamConfiguration StreamConfiguration;
    
    /// RIFF header
    struct ChunkDecriptor {
        char riff [4];      // ASCII "RIFF"
        /// This is the size of the entire file in bytes minus 8 bytes
        int chunkSize;      // 36 + SubChunk2Size
        char wave [4];      // ASCII "WAVE"
    };
    
    /// FMT header
    struct FmtSubChunk {
        char fmt_ [4];      // ASCII "fmt"+32
        /// This is the size of the rest of fmt subChunk
        int SubChunk1Size;  // (16)
        short audioFormat;  // 1 - PCM
        short numChannels;   // 1 - mono, 2 - stereo
        int sampleRate;     // Samples per second (eg: 44100)
        /// Bytes per second
        int byteRate;       // sampleRate * numChannels * bitsPerSample/8
        short blockAlign;   // numChannels * bitsPerSample/8
        short bitsPerSample;// if 8 : data is stored as unsigned ints else if  16 : data is stored as 2 complement signed ints, ...
    };
    
    /// DATA header and samples
    struct DataSubChunk {
        char data [4]; // "DATA"
        /// Number of bytes in data
        int SubChunk2Size; // numSamples * numChannels * bitsPerSample/8
        void * samples;
    };

    /// LIST header
    struct ListChunk {
        char list[4];
        int chunkSize = 0;
    };
    
    /// INFO header
    struct InfoSubChunk {
        
        const static int max_size = 512;
        
        struct SubChunk {
            int size = 0;
            char data[max_size] = {0};
        };
        
        char info [4]; // "INFO"
        
        /// Unknow information
        struct IUKN : public SubChunk {
            char iukn [4] = {0};
        } iuknSubChunk;
        
        /// Track title.
        struct INAM : public SubChunk {
            char inam [4] = {0};
        } inamSubChunk;
        
        /// The album title.
        struct IPRD : public SubChunk {
            char iprd [4] = {0};
        } iprdSubChunk;
        
        /// The artist.
        struct IART : public SubChunk {
            char iart [4] = {0};
        } iartSubChunk;
        
        /// The creation date in YYYY-MM-DD or just YYYY format.
        struct ICRD : public SubChunk {
            char icrd [4] = {0};
        } icrdSubChunk;
        
        /// Track number.
        struct ITRK : public SubChunk {
            char itrk [4] = {0};
        } itrkSubChunk;
        
        /// A text comment.
        struct ICMT : public SubChunk {
            char icmt [4] = {0};
        } icmtSubChunk;

        /// The keywords for the project or file.
        struct IKEY : public SubChunk {
            char ikey [4] = {0};
        } ikeySubChunk;
        
        /// The software used to create the file.
        struct ISFT : public SubChunk {
            char isft [4] = {0};
        } isftSubChunk;
        
        /// The name(s) of the engineer. Multiple names separated by a semicolon and a blank.
        struct IENG : public SubChunk {
            char ieng [4] = {0};
        } iengSubChunk;

        /// The technician.
        struct ITCH : public SubChunk {
            char itch [4] = {0};
        } itchSubChunk;
        
        /// Genre of content.
        struct IGNR : public SubChunk {
            char ignr [4];
        } ignrSubChunk;
        
        /// The copyright information.
        struct ICOP : public SubChunk {
            char icop [4];
        } icopSubChunk;
        
        /// The subject.
        struct ISBJ : public SubChunk {
            char isbj [4];
        } isbjSubChunk;
        
        /// Source: the name of the person or organization that supplied the original subject of the file.
        struct ISRC : public SubChunk {
            char isrc [4];
        } isrcSubChunk;
    };
    
    struct WavFile {
        ChunkDecriptor chunkDescriptor;
            FmtSubChunk fmtSubChunk;
            DataSubChunk dataChunk;
        ListChunk listInfo;
            InfoSubChunk infoSubChunk;
    };
    
    typedef enum {
        file__nullptr,
        pull__unexpected_EOF,
        push__invalid_chunk_descriptor,
        push__invalid_fmt_chunk,
        push__invalid_data_chunk,
        // push__invalid_LIST_chunk,
        pull__invalid_chunk_descriptor,
        pull__invalid_fmt_chunk,
        pull__invalid_data_chunk,
        pull__invalid_LIST_chunk,
    } ErrorId;
    
private:
    
    fpos_t      hpos;

private:
    
    bool fopen_rb(const char *);
        
    void * popData(size_t size, size_t n);
        
    ChunkDecriptor * getChunkDescriptor();
    FmtSubChunk * getFmtSubchunk();
    DataSubChunk * getDataChunk();
    ListChunk * getListInfoChunk();
    InfoSubChunk * getInfoChunk();
    InfoSubChunk::SubChunk * getInfoSubChunk (const char * chunkName, char * chunkNameAddr, InfoSubChunk::SubChunk & chunk);

    bool prevNextChunk();
    
    void ignoreChunk(size_t size);
  
public:
    
    // The processed wave file
    WavFile     wavFile;
    /// Stream configuration, based in the current wave instance
    StreamConfiguration * streamConfiguration = nullptr;
    
    // The wave file
    FILE    *   file = nullptr;

public:
    dwav();
    
    /// Throws a std::runtime_error
    /// @arg id The Lanau Error type
    /// @arg msg Error Message
    void throw_error(ErrorId id, const char * msg);
    
    const ChunkDecriptor * get_chunk_descriptor() {
        return &wavFile.chunkDescriptor;
    }
    
    const FmtSubChunk * get_fmt_chunk(){
        return &wavFile.fmtSubChunk;
    }
    
    const DataSubChunk * get_data_chunk(){
        return &wavFile.dataChunk;
    }
    
    const InfoSubChunk * get_info_chunk(){
        return &wavFile.infoSubChunk;
    }
    
    const char * get_track_tittle(){
        return wavFile.infoSubChunk.inamSubChunk.data;
    }
    
    const char * get_album_title(){
        return wavFile.infoSubChunk.iprdSubChunk.data;
    }
    
    const char * get_artist() {
        return wavFile.infoSubChunk.iartSubChunk.data;
    }
    
    const char * get_creation_date(){
        return wavFile.infoSubChunk.icrdSubChunk.data;
    }
    
    const char * get_track_number(){
        return wavFile.infoSubChunk.itrkSubChunk.data;
    }
    
    const char * get_text_comment(){
        return wavFile.infoSubChunk.icmtSubChunk.data;
    }
    
    const char * get_keywords(){
        return wavFile.infoSubChunk.ikeySubChunk.data;
    }
    
    const char * get_software(){
        return wavFile.infoSubChunk.isftSubChunk.data;
    }
    
    const char * get_engineer(){
        return wavFile.infoSubChunk.iengSubChunk.data;
    }
    
    const char * get_technician(){
        return wavFile.infoSubChunk.itchSubChunk.data;
    }
    
    const char * get_genre(){
        return wavFile.infoSubChunk.ignrSubChunk.data;
    }
    
    const char * get_copyright(){
        return wavFile.infoSubChunk.icopSubChunk.data;
    }
    
    const char * get_source(){
        return wavFile.infoSubChunk.isrcSubChunk.data;
    }
    
    /// Pulls all suported chunks from a file
    /// @arg filename The file
    bool pull(const char * filename);
    ~dwav();
};

typedef dwav decoder_wav;

#endif /* wav_hpp */
