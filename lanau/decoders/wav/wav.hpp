//
//  wav.hpp
//  lanau
//
//  Created by René Descartes Domingos Muala on 12/05/21.
//

#ifndef wav_hpp
#define wav_hpp

#include <stdio.h>
#include <string>

/// The WAVE file format is a subset of Microsoft's RIFF specification for the storage of multimedia files. A RIFF file starts out with a file header followed by a sequence of data chunks. A WAVE file is often just a RIFF file with a single "WAVE" chunk which consists of two sub-chunks -- a "fmt " chunk specifying the data format and a "data" chunk containing the actual sample data. (13/05/2021 -  http://soundfile.sapp.org/doc/WaveFormat/)
class wav {
public:
    struct ChunckDecriptor {
        char riff [4];      // ASCII "RIFF"
        /// This is the size of the entire file in bytes minus 8 bytes
        int chunkSize;      // 36 + SubChunk2Size
        char wave [4];      // ASCII "WAVE"
    };
    
    struct FmtSubchunk {
        char fmt_ [4];      // ASCII "fmt"+32
        /// This is the size of the rest of fmt subChunck
        int SubChunk1Size;  // (16)
        short audioFormat;  // 1 - PCM
        short numChannels;   // 1 - mono, 2 - stereo
        int sampleRate;     // Samples per second (eg: 44100)
        /// Bytes per second
        int byteRate;       // sampleRate * numChannels * bitsPerSample/8
        short blockAlign;   // numChannels * bitsPerSample/8
        short bitsPerSample;// if 8 : data is stored as unsigned ints else if  16 : data is stored as 2 complement signed ints, ...
    };
    
    struct DataSubChunck {
        char data [4]; // "DATA"
        /// Number of bytes in data
        int SubChunk2Size; // numSamples * numChannels * bitsPerSample/8
        void * samples;
    };

    /// LIST
    struct ListChunck {
        char list[4];
        int chunckSize = 0;
    };
    
    /// LIST....INFO
    /// # INAM....TRACK TITLE
    /// # IPRD....ALBUM TITLE
    /// # IART....ARTIST NAME
    /// # ICMT....COMMENTS
    /// # ICRD....YEAR
    /// # IGNR....GENRE
    /// # ITRK....TRACK NUMBER
    struct InfoSubChunk {
        
        const static int max_size = 512;
        
        struct SubChunck {
            int size;
            char data[max_size] = {0};
        };
        
        char info [4]; // "INFO"
        
        /// Unknow information
        struct IUKN : public SubChunck {
            char iukn [4];
        } iuknSubChunk;
        
        /// Track title.
        struct INAM : public SubChunck {
            char inam [4];
        } inamSubChunk;
        
        /// The album title.
        struct IPRD : public SubChunck {
            char iprd [4];
        } iprdSubChunk;
        
        /// The artist.
        struct IART : public SubChunck {
            char iart [4];
        } iartSubChunk;
        
        /// The creation date in YYYY-MM-DD or just YYYY format.
        struct ICRD : public SubChunck {
            char icrd [4];
        } icrdSubChunk;
        
        /// Track number.
        struct ITRK : public SubChunck {
            char itrk [4];
        } itrkSubChunk;
        
        /// A text comment.
        struct ICMT : public SubChunck {
            char itcmt [4];
        } itcmtSubChunk;

        /// The keywords for the project or file.
        struct IKEY : public SubChunck {
            char ikey [4];
        } ikeySubChunk;
        
        /// The software used to create the file.
        struct ISFT : public SubChunck {
            char isft [4];
        } isftSubChunk;
        
        /// The name(s) of the engineer. Multiple names separated by a semicolon and a blank.
        struct IENG : public SubChunck {
            char ieng [4];
        } iengSubChunk;

        /// The technician.
        struct ITCH : public SubChunck {
            char itch [4];
        } itchSubChunk;
        
        /// Genre of content.
        struct IGNR : public SubChunck {
            char ignr [4];
        } ignrSubChunk;
        
        /// The copyright information.
        struct ICOP : public SubChunck {
            char icop [4];
        } icopSubChunkv;
        
        /// The subject.
        struct ISBJ : public SubChunck {
            char isbj [4];
        } isbjSubChunk;
        
        /// Source: the name of the person or organization that supplied the original subject of the file.
        struct ISRC : public SubChunck {
            char isrc [4];
        } isrcSubChunk;
    };
    
    struct WavFileInfo {
        ChunckDecriptor chunckDescriptor;
            FmtSubchunk fmtSubChunck;
            DataSubChunck dataChunck;
        ListChunck listInfo;
            InfoSubChunk infoSubChunk;
    };
    
    typedef enum {
        file__nullptr,
        pull__unexpected_EOF,
        pull__invalid_chunck_descriptor,
        pull__invalid_fmt_chunck,
        pull__invalid_data_chunck,
        pull__invalid_metadata_chunck,
    } ErrorId;
    
private:
    
    WavFileInfo     wavFileInfo;
    FILE    *   file;
    fpos_t      hpos;

private:
    
    bool fopen_rb(const char *);
    bool fopen_wb(const char *);
    
    void throw_error(ErrorId id, const char * msg);
    
    void * popData(size_t size, size_t n);
        
    ChunckDecriptor * getChunckDescriptor();
    FmtSubchunk * getFmtSubchunk();
    DataSubChunck * getDataChunck();
    ListChunck * getListInfoChunk();
    InfoSubChunk * getInfoChunck();
    InfoSubChunk::SubChunck * getinfoSubChunck (const char * chunckName, char * chunckNameAddr, InfoSubChunk::SubChunck & chunk);

    /// Preview next chunck: make a time travel to get the next chunk name and size.
    bool prevNextChunck();
    
    void ignoreChunck(size_t size);
    
public:
    wav();
    bool push(const char *);
    
    ChunckDecriptor * get_chunck_descriptor() {
        return &wavFileInfo.chunckDescriptor;
    }
    
    FmtSubchunk * get_fmt_chunck(){
        return &wavFileInfo.fmtSubChunck;
    }
    
    DataSubChunck * get_data_chunck(){
        return &wavFileInfo.dataChunck;
    }
    
    InfoSubChunk * get_info_chunck(){
        return &wavFileInfo.infoSubChunk;
    }
    
    const char * get_track_tittle(){
        return wavFileInfo.infoSubChunk.inamSubChunk.data;
    }
    
    const char * get_album_title(){
        return wavFileInfo.infoSubChunk.iprdSubChunk.data;
    }
    
    const char * get_artist() {
        return wavFileInfo.infoSubChunk.iartSubChunk.data;
    }
    
    const char * get_creation_date(){
        return wavFileInfo.infoSubChunk.icrdSubChunk.data;
    }
    
    const char * get_track_number(){
        return wavFileInfo.infoSubChunk.itrkSubChunk.data;
    }
    
    const char * get_text_comment(){
        return wavFileInfo.infoSubChunk.itcmtSubChunk.data;
    }
    
    const char * get_keywords(){
        return wavFileInfo.infoSubChunk.ikeySubChunk.data;
    }
    
    const char * get_software(){
        return wavFileInfo.infoSubChunk.isftSubChunk.data;
    }
    
    const char * get_engineer(){
        return wavFileInfo.infoSubChunk.iengSubChunk.data;
    }
    
    const char * get_technician(){
        return wavFileInfo.infoSubChunk.itchSubChunk.data;
    }
    
    const char * get_genre(){
        return wavFileInfo.infoSubChunk.ignrSubChunk.data;
    }
    
    const char * get_copyright(){
        return wavFileInfo.infoSubChunk.icopSubChunkv.data;
    }
    
    const char * get_source(){
        return wavFileInfo.infoSubChunk.isrcSubChunk.data;
    }
    
    bool pull(const char *);
    ~wav();
};

#endif /* wav_hpp */
