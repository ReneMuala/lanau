//
//  wav.hpp
//  lanau
//
//  Created by René Descartes Domingos Muala on 15/05/21.
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
#ifndef ewav_hpp
#define ewav_hpp

#include "../wav/../../decoders/wav/wav.hpp"
#include <iostream>
/// WAVE encoder, The WAVE file format is a subset of Microsoft's RIFF specification for the storage of multimedia files. A RIFF file starts out with a file header followed by a sequence of data chunks. A WAVE file is often just a RIFF file with a single "WAVE" chunk which consists of two sub-chunks -- a "fmt " chunk specifying the data format and a "data" chunk containing the actual sample data. (13/05/2021 -  http://soundfile.sapp.org/doc/WaveFormat/)
class ewav : public dwav {
    
private:
    
    bool fopen_wb(const char * filename);
    
    bool pushData(void * data, size_t size, size_t n);
    
    bool checkInfoSubChunk (InfoSubChunk::SubChunk & chunk);
    int  getInfoSubChunkTotalSize (InfoSubChunk::SubChunk & chunk);
    bool writeInfoSubChunk (const char * chunkName, InfoSubChunk::SubChunk & chunk);
    
public:
    bool writeChunkDescriptor();
    bool writeFmtSubChunk();
    bool writeDataChunk();
    bool writeListInfoChunk();
    
    /// Pushes the wav data to a file
    /// @arg filename the file 
    bool push(const char * filename);
    
    void set_chunk_descriptor(ChunkDecriptor descriptor) {
        wavFile.chunkDescriptor = descriptor;
    }
    
    void set_fmt_chunk(FmtSubChunk fmt){
        wavFile.fmtSubChunk = fmt;
    }
    
    void set_data_chunk(DataSubChunk data){
        wavFile.dataChunk = data;
    }
    
    void set_info_chunk(InfoSubChunk info){
        wavFile.infoSubChunk = info;
    }
    
    void set_track_tittle(const char * title){
        strncpy(wavFile.infoSubChunk.inamSubChunk.inam, "INAM", 4);
        wavFile.infoSubChunk.inamSubChunk.size = (int)strlen(title);
        strncpy(wavFile.infoSubChunk.inamSubChunk.data, title, wavFile.infoSubChunk.inamSubChunk.size);
    }
    
    void set_album_title(const char * title){
        strncpy(wavFile.infoSubChunk.iprdSubChunk.iprd, "IPRD", 4);
        wavFile.infoSubChunk.iprdSubChunk.size = (int)strlen(title);
        strncpy(wavFile.infoSubChunk.iprdSubChunk.data, title, wavFile.infoSubChunk.iprdSubChunk.size);
    }
    
    void set_artist(const char * artist) {
        strncpy(wavFile.infoSubChunk.iartSubChunk.iart, "IART", 4);
        wavFile.infoSubChunk.iartSubChunk.size = (int)strlen(artist);
        strncpy(wavFile.infoSubChunk.iartSubChunk.data, artist, wavFile.infoSubChunk.iartSubChunk.size);
    }
    
    void set_creation_date(const char * date){
        strncpy(wavFile.infoSubChunk.icrdSubChunk.icrd, "ICRD", 4);
        wavFile.infoSubChunk.icrdSubChunk.size = (int)strlen(date);
        strncpy(wavFile.infoSubChunk.icrdSubChunk.data, date, wavFile.infoSubChunk.icrdSubChunk.size);
    }
    
    void set_track_number(const char * number){
        strncpy(wavFile.infoSubChunk.itrkSubChunk.itrk, "ITRK", 4);
        wavFile.infoSubChunk.itrkSubChunk.size = (int)strlen(number);
        strncpy(wavFile.infoSubChunk.itrkSubChunk.data, number, wavFile.infoSubChunk.itrkSubChunk.size);
    }
    
    void set_text_comment(const char * comment){
        strncpy(wavFile.infoSubChunk.icmtSubChunk.icmt, "ICMT", 4);
        wavFile.infoSubChunk.icmtSubChunk.size = (int)strlen(comment);
        strncpy(wavFile.infoSubChunk.icmtSubChunk.data, comment, wavFile.infoSubChunk.icmtSubChunk.size);
    }
    
    void set_keywords(const char * keywords){
        strncpy(wavFile.infoSubChunk.ikeySubChunk.ikey, "IKEY", 4);
        wavFile.infoSubChunk.ikeySubChunk.size = (int)strlen(keywords);
        strncpy(wavFile.infoSubChunk.ikeySubChunk.data, keywords, wavFile.infoSubChunk.ikeySubChunk.size);
    }
    
    void set_software(const char * software){
        strncpy(wavFile.infoSubChunk.isftSubChunk.isft, software, 4);
        wavFile.infoSubChunk.isftSubChunk.size = (int)strlen(software);
        strncpy(wavFile.infoSubChunk.isftSubChunk.data, software, wavFile.infoSubChunk.isftSubChunk.size);
    }
    
    void set_engineer(const char * engineer){
        strncpy(wavFile.infoSubChunk.iengSubChunk.ieng, "IENG", 4);
        wavFile.infoSubChunk.iengSubChunk.size = (int)strlen(engineer);
        strncpy(wavFile.infoSubChunk.iengSubChunk.data, engineer, wavFile.infoSubChunk.iengSubChunk.size);
    }
    
    void set_technician(const char * technician){
        strncpy(wavFile.infoSubChunk.itchSubChunk.itch, "ITCH", 4);
        wavFile.infoSubChunk.itchSubChunk.size = (int)strlen(technician);
        strncpy(wavFile.infoSubChunk.itchSubChunk.data, technician, wavFile.infoSubChunk.itchSubChunk.size);
    }
    
    void set_genre(const char * genre){
        strncpy(wavFile.infoSubChunk.ignrSubChunk.ignr, "IGEN", 4);
        wavFile.infoSubChunk.ignrSubChunk.size = (int)strlen(genre);
        strncpy(wavFile.infoSubChunk.ignrSubChunk.data, genre, wavFile.infoSubChunk.ignrSubChunk.size);
    }
    
    void set_copyright(const char * copyright){
        strncpy(wavFile.infoSubChunk.icopSubChunk.icop, "ICOP", 4);
        wavFile.infoSubChunk.icopSubChunk.size = (int)strlen(copyright);
        strncpy(wavFile.infoSubChunk.icopSubChunk.data, copyright, wavFile.infoSubChunk.icopSubChunk.size);
    }
    
    void set_source(const char * source){
        strncpy(wavFile.infoSubChunk.isrcSubChunk.isrc, "ISRC", 4);
        wavFile.infoSubChunk.isrcSubChunk.size = (int)strlen(source);
        strncpy(wavFile.infoSubChunk.isrcSubChunk.data, source, wavFile.infoSubChunk.isrcSubChunk.size);
    }
};

#endif /* wav_hpp */
