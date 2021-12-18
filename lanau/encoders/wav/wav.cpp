//
//  wav.cpp
//  lanau
//
//  Created by René Descartes Domingos Muala on 15/05/21.
//

#include "wav.hpp"
#include <iostream>

bool ewav::fopen_wb(const char * filename){
    if(file != nullptr) fclose(file);
    return (file = fopen(filename, "wb"));
}

bool ewav::pushData(void *data, size_t size, size_t n){
    if(file){
        return fwrite(data, size, n, file);
    } throw_error(file__nullptr, "Unable to write in file");
    return false;
}

bool ewav::checkInfoSubChunk(InfoSubChunk::SubChunk &chunk){
    return (chunk.size = (int)strlen(chunk.data)) > 0;
}

int ewav::getInfoSubChunkTotalSize(InfoSubChunk::SubChunk &chunk){
    return chunk.size + sizeof(int) + 4;
}

bool ewav::writeInfoSubChunk(const char * chunkName, InfoSubChunk::SubChunk &chunk){
    std::cout << "writing " << chunkName << " size: " << chunk.size << " data: " << chunk.data << std::endl;
    return pushData((void*)chunkName, sizeof(char), 4) && pushData((void*)&chunk.size, sizeof(int), 1)  && pushData((void*)&chunk.data, sizeof(char), chunk.size);
}

bool ewav::writeChunkDescriptor() {
    return pushData((void*)&wavFile.chunkDescriptor, sizeof(wavFile.chunkDescriptor), 1);
}

bool ewav::writeFmtSubChunk(){
    return pushData((void*)&wavFile.fmtSubChunk, sizeof(wavFile.fmtSubChunk), 1);
}

bool ewav::writeDataChunk(){
    if(pushData((void*)&wavFile.dataChunk.data, sizeof(char), 4) && pushData((void*)&wavFile.dataChunk.SubChunk2Size, sizeof(int), 1)){
        int len = this->streamConfiguration->get_audioLen();
        int sample_unit_size = 0;
        switch (wavFile.fmtSubChunk.bitsPerSample) {
            case 8: sample_unit_size = sizeof(uint8_t)*wavFile.fmtSubChunk.numChannels; break;
            case 16: sample_unit_size = sizeof(int16_t)*wavFile.fmtSubChunk.numChannels; break;
            default: throw_error(pull__invalid_fmt_chunk, "Unsopported bits per sample"); break;
        }
        uint8_t * data_8_p = (uint8_t*)wavFile.dataChunk.samples;
        int16_t * data_16_p = (int16_t*)wavFile.dataChunk.samples;
        int * data_32_p = (int*)wavFile.dataChunk.samples;
        for(int i = 0 ; i < len ; i++)
        // 8 bit mono audio
            if(sample_unit_size == sizeof(uint8_t))
                pushData((void*)&(data_8_p[i]),sample_unit_size, 1);
        // 16 bit mono audio
            else if (sample_unit_size == sizeof(int16_t))
                pushData((void*)&(data_16_p[i]),sample_unit_size, 1);
        // 16 bit stereo audio
            else if (sample_unit_size == sizeof(int16_t)*2)
                pushData((void*)&(data_32_p[i]),sample_unit_size, 1);
        return true;
    } return false;
}

bool ewav::writeListInfoChunk(){
    
    // Updating LIST header to avoid errors
    strncpy(wavFile.listInfo.list, "LIST", 4);
    wavFile.listInfo.chunkSize = sizeof(int);
    // Updating INFO header to avoid errors
    strncpy(wavFile.infoSubChunk.info, "INFO", 4);
    
    // chunkIDs for easy code maintenance
    enum InfoSubChunkID {
        s_inam,
        s_iprd,
        s_iart,
        s_icrd,
        s_itrk,
        s_icmt,
        s_ikey,
        s_isft,
        s_ieng,
        s_itch,
        s_ignr,
        s_icop,
        s_isbj,
        s_isrc,
        s_total,
    };
    
    bool atLeastOneChunk (false);
    bool isChunkAvaliable[s_total] = {false};
        
    // Mapping all subChunks to scan
    ewav::InfoSubChunk::SubChunk * chunks [s_total] = {
        &wavFile.infoSubChunk.inamSubChunk,
        &wavFile.infoSubChunk.iprdSubChunk,
        &wavFile.infoSubChunk.iartSubChunk,
        &wavFile.infoSubChunk.icrdSubChunk,
        &wavFile.infoSubChunk.itrkSubChunk,
        &wavFile.infoSubChunk.icmtSubChunk,
        &wavFile.infoSubChunk.ikeySubChunk,
        &wavFile.infoSubChunk.isftSubChunk,
        &wavFile.infoSubChunk.iengSubChunk,
        &wavFile.infoSubChunk.itchSubChunk,
        &wavFile.infoSubChunk.ignrSubChunk,
        &wavFile.infoSubChunk.icopSubChunk,
        &wavFile.infoSubChunk.isbjSubChunk,
        &wavFile.infoSubChunk.isrcSubChunk,
    };
    
    // Scanning subChunks and getting their sizes
    for (int i = s_inam ; i < s_total ; i ++){
        if ((isChunkAvaliable[i] = checkInfoSubChunk((InfoSubChunk::SubChunk&)*chunks[i]))) {
            if(!atLeastOneChunk) atLeastOneChunk = true;
            wavFile.listInfo.chunkSize += getInfoSubChunkTotalSize((InfoSubChunk::SubChunk &)*chunks[i]);
            isChunkAvaliable[i] = true;
        }
    }
    //std::cout << "SIZE: " << wavFile.listInfo.chunkSize << std::endl;
    // If at leaft one chunk is Avaliable, do the work!
    if(atLeastOneChunk){
        // Writing LIST header
        pushData(wavFile.listInfo.list, sizeof(char), 4);
        pushData((void*)&wavFile.listInfo.chunkSize, sizeof(int), 1);
        // Writing INFO header
        pushData(wavFile.infoSubChunk.info, sizeof(char), 4);
        
        // If a chunk is avaliable, write it
        
        if(isChunkAvaliable[s_inam])
            writeInfoSubChunk("INAM", (InfoSubChunk::SubChunk &)*chunks[s_inam]);
        
        if(isChunkAvaliable[s_iprd])
            writeInfoSubChunk("IPRD", (InfoSubChunk::SubChunk &)*chunks[s_iprd]);
        
        if(isChunkAvaliable[s_iart])
            writeInfoSubChunk("IART", (InfoSubChunk::SubChunk &)*chunks[s_iart]);
        
        if(isChunkAvaliable[s_icrd])
            writeInfoSubChunk("ICRD", (InfoSubChunk::SubChunk &)*chunks[s_icrd]);
        
        if(isChunkAvaliable[s_itrk])
            writeInfoSubChunk("ITRK", (InfoSubChunk::SubChunk &)*chunks[s_itrk]);
        
        if(isChunkAvaliable[s_icmt])
            writeInfoSubChunk("ICMT", (InfoSubChunk::SubChunk &)*chunks[s_icmt]);
        
        if(isChunkAvaliable[s_ikey])
            writeInfoSubChunk("IKEY", (InfoSubChunk::SubChunk &)*chunks[s_ikey]);
        
        if(isChunkAvaliable[s_isft])
            writeInfoSubChunk("ISFT", (InfoSubChunk::SubChunk &)*chunks[s_isft]);
        
        if(isChunkAvaliable[s_ieng])
            writeInfoSubChunk("IENG", (InfoSubChunk::SubChunk &)*chunks[s_ieng]);
        
        if(isChunkAvaliable[s_itch])
            writeInfoSubChunk("ITCH", (InfoSubChunk::SubChunk &)*chunks[s_itch]);
        
        if(isChunkAvaliable[s_ignr])
            writeInfoSubChunk("IGNR", (InfoSubChunk::SubChunk &)*chunks[s_ignr]);
        
        if(isChunkAvaliable[s_icop])
            writeInfoSubChunk("ICOP", (InfoSubChunk::SubChunk &)*chunks[s_icop]);
        
        if(isChunkAvaliable[s_isbj])
            writeInfoSubChunk("ISBJ", (InfoSubChunk::SubChunk &)*chunks[s_isbj]);
        
        if(isChunkAvaliable[s_isrc])
            writeInfoSubChunk("ISRC", (InfoSubChunk::SubChunk &)*chunks[s_isrc]);
        return true;
    }
    return false;
}

bool ewav::push(const char * filename){
    if(fopen_wb(filename)){
        if(writeChunkDescriptor()){
            if(writeFmtSubChunk()){
                if(writeDataChunk()){
                    writeListInfoChunk();
                    return true;
                } else
                    throw_error(push__invalid_data_chunk, "Unable to write data chunk");
            } else
                throw_error(push__invalid_fmt_chunk, "Unable to write fmt chunk");
        } else
            throw_error(push__invalid_chunk_descriptor, "Unable to write chunk descriptor");
    } throw_error(file__nullptr, "Unable to open file");
    return false;
}
