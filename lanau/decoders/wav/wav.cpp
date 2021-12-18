//
//  wav.cpp
//  lanau
//
//  Created by René Descartes Domingos Muala on 12/05/21.
//

#include "wav.hpp"
#include "../../lanau.hpp"
#include <memory>
#include <string>
#include <iostream>

dwav::RtAudioStreamConfiguration::RtAudioStreamConfiguration(dwav& context){
    this->context = &context;
}

const int dwav::RtAudioStreamConfiguration::get_nChannels(){
    return context->wavFile.fmtSubChunk.numChannels;
}

const int dwav::RtAudioStreamConfiguration::get_sampleRate(){
    return context->wavFile.fmtSubChunk.sampleRate;
}

const int dwav::RtAudioStreamConfiguration::get_bufferFrame() {
    return context->wavFile.fmtSubChunk.byteRate;
}

const int dwav::RtAudioStreamConfiguration::get_audioLen(){
    switch (context->wavFile.fmtSubChunk.bitsPerSample) {
        case 8:
            return context->wavFile.dataChunk.SubChunk2Size /(sizeof(int8_t)*get_nChannels());
            break;
        case 16:
            return context->wavFile.dataChunk.SubChunk2Size /(sizeof(int16_t)*get_nChannels());
            break;
        default:
            return 0;
            break;
    }
}

const void * dwav::StreamConfiguration::get_audioSamples(){
    return context->wavFile.dataChunk.samples;
}

const unsigned int dwav::StreamConfiguration::get_RtAudio_StreamOptions(){
#ifdef __RTAUDIO_H
    return RTAUDIO_MINIMIZE_LATENCY;
#else
    return 0x2;
#endif
}

const unsigned long dwav::StreamConfiguration::get_RtAudio_AudioFormat(){
#ifdef __RTAUDIO_H
    return RTAUDIO_SINT16;
#else
    return 0x2;
#endif
}

bool dwav::fopen_rb(const char * filename){
    if(file != nullptr) fclose(file);
    return (file = fopen(filename, "rb"));
}

void * dwav::popData(size_t size, size_t n){
    static void * data = nullptr;
    if(data != nullptr) {free(data); data = nullptr;}
        if(file){
        if(!feof(file)){
            data = malloc(size);
            fread(data, size, n, file);
            return data;
        } throw_error(pull__unexpected_EOF, "Unexpected EOF");
    } throw_error(file__nullptr, "Unable to pop file");
    return data;
}

dwav::ChunkDecriptor * dwav::getChunkDescriptor(){
    if(file) {
        // Read "RIFF" and check
        if(fread(wavFile.chunkDescriptor.riff, sizeof(char), 4, file) && strncmp(wavFile.chunkDescriptor.riff, "RIFF", 4) == 0){
            // get chunkSize
            wavFile.chunkDescriptor.chunkSize = *(int*)popData(sizeof(int), 1); //
            // Read "WAVE" and check
            if(fread(wavFile.chunkDescriptor.wave, sizeof(char), 4, file) && strncmp(wavFile.chunkDescriptor.wave, "WAVE", 4) == 0) {
                return &wavFile.chunkDescriptor;
            } throw_error(pull__invalid_chunk_descriptor, "Unable to read WAVE-str");
        } throw_error(pull__invalid_chunk_descriptor, "Unable to read RIFF-str");
    } throw_error(file__nullptr, "Unable to read file");
    return nullptr;
}

dwav::FmtSubChunk * dwav::getFmtSubchunk(){
    if(file){
        // buffer to store "fmt"+(char)32
        char fmt__buff_str[4];
        sprintf(fmt__buff_str, "fmt%c",32);
        // Read "fmt"+(char)32 and check
        if(fread(wavFile.fmtSubChunk.fmt_, sizeof(char), 4, file) && strncmp(wavFile.fmtSubChunk.fmt_, fmt__buff_str, 4) == 0){
            // read chunksSize
            wavFile.fmtSubChunk.SubChunk1Size = *(int*)popData(sizeof(int), 1);
            if(wavFile.fmtSubChunk.SubChunk1Size == 16){
                // read audio format
                wavFile.fmtSubChunk.audioFormat = *(short*)popData(sizeof(short), 1);
                if(wavFile.fmtSubChunk.audioFormat == 1){
                    // read nchannets
                    wavFile.fmtSubChunk.numChannels = *(short*)popData(sizeof(short), 1);
                    // read sampleRate
                    wavFile.fmtSubChunk.sampleRate = *(int*)popData(sizeof(int), 1);
                    // read byteRate
                    wavFile.fmtSubChunk.byteRate = *(int*)popData(sizeof(int), 1);
                    // read blockAlign
                    wavFile.fmtSubChunk.blockAlign = *(short*)popData(sizeof(short), 1);
                    // read bitsPerSample
                    wavFile.fmtSubChunk.bitsPerSample = *(short*)popData(sizeof(short), 1);
                    if(wavFile.fmtSubChunk.bitsPerSample == 8 || wavFile.fmtSubChunk.bitsPerSample == 16) {
                        return &wavFile.fmtSubChunk;
                    } throw_error(pull__invalid_fmt_chunk, "Unsopported bits per sample");
                } throw_error(pull__invalid_fmt_chunk, "Unsopported audio format");
            } throw_error(pull__invalid_fmt_chunk, "Unsopported chunk size");
        } throw_error(pull__invalid_fmt_chunk, "Unable to read fmt?-str");
    } throw_error(file__nullptr, "Unable to read file");
    return nullptr;
}

dwav::DataSubChunk * dwav::getDataChunk(){
    if(file) {
        size_t sample_unit_size = 0;
        // decide the best way to store samples
        switch (wavFile.fmtSubChunk.bitsPerSample) {
            case 8: sample_unit_size = sizeof(uint8_t)*wavFile.fmtSubChunk.numChannels; break;
            case 16: sample_unit_size = sizeof(int16_t)*wavFile.fmtSubChunk.numChannels; break;
            default: throw_error(pull__invalid_fmt_chunk, "Unsopported bits per sample"); break;
        }
        // read "DATA" and check
        if(fread(wavFile.dataChunk.data, sizeof(char), 4, file) && strncmp(wavFile.dataChunk.data, "DATA", 0) == 0){
            // read chunk size and check
            if ((wavFile.dataChunk.SubChunk2Size = *(int*)popData(sizeof(int), 1))){
                // allocate the necessary memory to store samples
                wavFile.dataChunk.samples = new int[wavFile.dataChunk.SubChunk2Size/sample_unit_size];
                //(void**)malloc((wavFile.dataChunk.SubChunk2Size/sample_unit_size));
                std::cout << "# samples size: " << wavFile.dataChunk.SubChunk2Size << ", sample packs: " << (wavFile.dataChunk.SubChunk2Size/sample_unit_size) << std::endl;
                // read samples
                int *_p = (int*)wavFile.dataChunk.samples;
                for (int i = 0 ; i < (wavFile.dataChunk.SubChunk2Size/sample_unit_size) ; i++){
                    _p[i] = *(int*)popData(sample_unit_size, 1);
//                    std::cout
//                    << (int&)_p[i]
//                    << " (" << i << " / " << (wavFile.dataChunk.SubChunk2Size/sample_unit_size) << ")"<< std::endl;
                } return &wavFile.dataChunk;
            } throw_error(pull__invalid_fmt_chunk, "Invalid chunk size");
        } throw_error(pull__invalid_data_chunk, "Unable to read DATA-str");
    } throw_error(file__nullptr, "Unable to read file");
    return nullptr;
}

dwav::ListChunk * dwav::getListInfoChunk(){
    if(file){
        // read "LIST" and check
        if(fread(wavFile.listInfo.list , sizeof(char), 4, file) && strncmp(wavFile.listInfo.list, "LIST", 4) == 0){
            // read chunk size and check
            wavFile.listInfo.chunkSize = *(int*)popData(sizeof(int), 1);
            // std::cout << "SIZE: " << wavFile.listInfo.chunkSize << std::endl;            // ignore if not LIST/INFO
            prevNextChunk();
            if(strncmp(wavFile.infoSubChunk.iuknSubChunk.iukn, "INFO", 4) != 0)
                ignoreChunk(wavFile.listInfo.chunkSize);
            return &wavFile.listInfo;
        } throw_error(pull__invalid_data_chunk, "Unable to read LIST-str");
    } throw_error(file__nullptr, "Unable to read file");
    return nullptr;
}

dwav::InfoSubChunk * dwav::getInfoChunk(){
    if(file){
        // read "INFO" and check
        if(fread(wavFile.infoSubChunk.info , sizeof(char), 4, file) && strncmp(wavFile.infoSubChunk.info, "INFO", 4) == 0){
            return &wavFile.infoSubChunk;
        } throw_error(pull__invalid_data_chunk, "Unable to read INFO-str");
    } throw_error(file__nullptr, "Unable to read file");
    return nullptr;
}

dwav::InfoSubChunk::SubChunk * dwav::getInfoSubChunk(const char *chunkName, char * chunkNameAddr, InfoSubChunk::SubChunk &chunk){
    //std::cout << "Reading: " << chunkName;
    if(file){
        // read "${chunkName}" and check
        if(fread(chunkNameAddr, sizeof(char), 4, file) && strncmp(chunkName, chunkNameAddr, 4) == 0){
            // read chunk size and check
            chunk.size = *(int*)popData(sizeof(int), 1);
            //std::cout << " size: " << chunk.size;
//            fpos_t test;
//            fgetpos(file, &test);
            if(chunk.size){
                // read chunk data
                fread(chunk.data, sizeof(char), chunk.size, file);
                //std::cout << " data: " << (char*)chunk.data << std::endl;;
                //ignoreChunk(1);
                return &chunk;
            } throw_error(pull__invalid_LIST_chunk, "Invalid chunk size");
        } throw_error(pull__invalid_data_chunk, ("Unable to read " + std::string(chunkName)+"-str").c_str());
    } throw_error(file__nullptr, "Unable to read file");
    return nullptr;
}

bool dwav::prevNextChunk(){
    if(file && !feof(file)){
        fgetpos(file, &hpos);
        fread(wavFile.infoSubChunk.iuknSubChunk.iukn, sizeof(char), 4, file);
        if(feof(file)) return false;
        wavFile.infoSubChunk.iuknSubChunk.size = *(int*) popData(sizeof(int), 1);
        fsetpos(file, &hpos);
        return true;
    } return false;
}

void dwav::ignoreChunk(size_t size){
    void ** buffer = (void**)calloc(size, 1);
    fread(buffer, 1, size, file);
    free(buffer);
}

dwav::dwav(){
    streamConfiguration = new StreamConfiguration(*this);
    file = nullptr;
}

void dwav::throw_error(ErrorId error_id, const char *msg){
    std::string error_message = "LANAU " + std::string(LanauApi::version) + " ";
    switch (error_id) {
        case file__nullptr:
            error_message += "(file.nullptr - std::runtime_error)";
            break;
        case pull__unexpected_EOF:
            error_message += "(pull.unexpected_EOF - std::runtime_error)";
            break;
        case push__invalid_chunk_descriptor:
            error_message += "(push.invalid_chunk_descriptor - std::runtime_error)";
            break;
        case push__invalid_fmt_chunk:
            error_message += "(push.invalid_fmt_chunk - std::runtime_error)";
            break;
        case push__invalid_data_chunk:
            error_message += "(push.invalid_data_chunk - std::runtime_error)";
            break;
        case pull__invalid_chunk_descriptor:
            error_message += "(pull.invalid_chunk_descriptor - std::runtime_error)";
            break;
        case pull__invalid_fmt_chunk:
            error_message += "(pull.invalid_fmt_chunk - std::runtime_error)";
            break;
        case pull__invalid_data_chunk:
            error_message += "(pull.invalid_data_chunk - std::runtime_error)";
            break;
        case pull__invalid_LIST_chunk:
            error_message += "(pull.invalid_LIST_chunk - std::runtime_error)";
            break;
        default:
            break;
    }
    error_message += "\"";
    error_message += msg;
    error_message += "\"";
    throw std::runtime_error(error_message);
}

bool dwav::pull(const char * filename){
    if(fopen_rb(filename)){
        bool chunkDescriptorFound (false),
        fmtChunkFound(false),
        dataChunkFound(false);
        char * chunkName = wavFile.infoSubChunk.iuknSubChunk.iukn;
        int * chunkSize = &wavFile.infoSubChunk.iuknSubChunk.size;
        InfoSubChunk * infoSubChunk = &wavFile.infoSubChunk;
        while (prevNextChunk()) {
            //std::cout << "chunk name: " << chunkName << std::endl;
            if(strncmp(chunkName, "RIFF", 4) == 0) {
                getChunkDescriptor();
                chunkDescriptorFound = true;
            } else if(strncmp(chunkName, "fmt", 3) == 0) {
                getFmtSubchunk();
                fmtChunkFound = true;
            } else if(strncmp(chunkName, "data", 4) == 0) {
                getDataChunk();
                dataChunkFound = true;
            }  else if(strncmp(chunkName, "LIST", 4) == 0)
                getListInfoChunk();
            else if(strncmp(chunkName, "INFO", 4) == 0)
                getInfoChunk();
            else if(strncmp(chunkName, "INAM", 4) == 0)
                getInfoSubChunk("INAM", infoSubChunk->inamSubChunk.inam, infoSubChunk->inamSubChunk);
            else if(strncmp(chunkName, "IPRD", 4) == 0)
                getInfoSubChunk("IPRD", infoSubChunk->iprdSubChunk.iprd, infoSubChunk->iprdSubChunk);
            else if(strncmp(chunkName, "IART", 4) == 0)
                getInfoSubChunk("IART", infoSubChunk->iartSubChunk.iart, infoSubChunk->iartSubChunk);
            else if(strncmp(chunkName, "ICRD", 4) == 0)
                getInfoSubChunk("ICRD", infoSubChunk->icrdSubChunk.icrd, infoSubChunk->icrdSubChunk);
            else if(strncmp(chunkName, "ITRK", 4) == 0)
                getInfoSubChunk("ITRK", infoSubChunk->itrkSubChunk.itrk, infoSubChunk->itrkSubChunk);
            else if(strncmp(chunkName, "ICMT", 4) == 0)
            getInfoSubChunk("ICMT", infoSubChunk->icmtSubChunk.icmt, infoSubChunk->icmtSubChunk);
            else if(strncmp(chunkName, "IKEY", 4) == 0)
                getInfoSubChunk("IKEY", infoSubChunk->ikeySubChunk.ikey, infoSubChunk->ikeySubChunk);
            else if(strncmp(chunkName, "ISFT", 4) == 0)
                getInfoSubChunk("ISFT", infoSubChunk->isftSubChunk.isft, infoSubChunk->isftSubChunk);
            else if(strncmp(chunkName, "IENG", 4) == 0)
            getInfoSubChunk("IENG", infoSubChunk->iengSubChunk.ieng, infoSubChunk->iengSubChunk);
            else if(strncmp(chunkName, "IGNR", 4) == 0)
                getInfoSubChunk("IGNR", infoSubChunk->ignrSubChunk.ignr, infoSubChunk->ignrSubChunk);
            else if(strncmp(chunkName, "ICOP", 4) == 0)
            getInfoSubChunk("ICOP", infoSubChunk->icopSubChunk.icop, infoSubChunk->icopSubChunk);
            else if(strncmp(chunkName, "ISBJ", 4) == 0)
                getInfoSubChunk("ISBJ", infoSubChunk->isbjSubChunk.isbj, infoSubChunk->isbjSubChunk);
            else if(strncmp(chunkName, "ISRC", 4) == 0)
            getInfoSubChunk("ISRC", infoSubChunk->isrcSubChunk.isrc, infoSubChunk->isrcSubChunk);
            else {
                if(strlen(chunkName) == 4 && isascii(chunkName[0])) {
                    ignoreChunk((int&)*chunkSize);
                } else
                    ignoreChunk(1);
            }
        } return chunkDescriptorFound && dataChunkFound && fmtChunkFound;
    }
    throw_error(file__nullptr, "Unable to open file");
    return false;
}

dwav::~dwav(){
    delete streamConfiguration;
    if(file != nullptr)
        fclose(file);
}
