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

bool wav::fopen_rb(const char * filename){
    if(file != nullptr) fclose(file);
    return (file = fopen(filename, "rb"));
}

bool wav::fopen_wb(const char * filename){
    if(file != nullptr) fclose(file);
    return (file = fopen(filename, "wb"));
}

void wav::throw_error(ErrorId error_id, const char *msg){
    std::string error_message = "LANAU " + std::to_string(LanauApi::version) + " ";
    switch (error_id) {
        case file__nullptr:
            error_message += "(file.nullptr - std::runtime_error)";
            break;
        case pull__unexpected_EOF:
            error_message += "(pull.unexpected_EOF - std::runtime_error)";
            break;
        case pull__invalid_chunck_descriptor:
            error_message += "(pull.invalid_chunck_descriptor - std::runtime_error)";
            break;
        case pull__invalid_fmt_chunck:
            error_message += "(pull.invalid_fmt_chunck - std::runtime_error)";
            break;
        case pull__invalid_data_chunck:
            error_message += "(pull.invalid_data_chunck - std::runtime_error)";
            break;
        case pull__invalid_metadata_chunck:
            error_message += "(pull.invalid_metadata_chunck - std::runtime_error)";
            break;
        default:
            break;
    }
    error_message += "\"";
    error_message += msg;
    error_message += "\"";
    throw std::runtime_error(error_message);
}

void * wav::popData(size_t size, size_t n){
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

wav::ChunckDecriptor * wav::getChunckDescriptor(){
    if(file) {
        // Read "RIFF" and check
        if(fread(wavFileInfo.chunckDescriptor.riff, sizeof(char), 4, file) && strncmp(wavFileInfo.chunckDescriptor.riff, "RIFF", 4) == 0){
            // get chunkSize
            wavFileInfo.chunckDescriptor.chunkSize = *(int*)popData(sizeof(int), 1); //
            // Read "WAVE" and check
            if(fread(wavFileInfo.chunckDescriptor.wave, sizeof(char), 4, file) && strncmp(wavFileInfo.chunckDescriptor.wave, "WAVE", 4) == 0) {
                return &wavFileInfo.chunckDescriptor;
            } throw_error(pull__invalid_chunck_descriptor, "Unable to read WAVE-str");
        } throw_error(pull__invalid_chunck_descriptor, "Unable to read RIFF-str");
    } throw_error(file__nullptr, "Unable to read file");
    return nullptr;
}

wav::FmtSubchunk * wav::getFmtSubchunk(){
    if(file){
        // buffer to store "fmt"+(char)32
        char fmt__buff_str[4];
        sprintf(fmt__buff_str, "fmt%c",32);
        // Read "fmt"+(char)32 and check
        if(fread(wavFileInfo.fmtSubChunck.fmt_, sizeof(char), 4, file) && strncmp(wavFileInfo.fmtSubChunck.fmt_, fmt__buff_str, 4) == 0){
            // read chuncksSize
            wavFileInfo.fmtSubChunck.SubChunk1Size = *(int*)popData(sizeof(int), 1);
            if(wavFileInfo.fmtSubChunck.SubChunk1Size == 16){
                // read audio format
                wavFileInfo.fmtSubChunck.audioFormat = *(short*)popData(sizeof(short), 1);
                if(wavFileInfo.fmtSubChunck.audioFormat == 1){
                    // read nchannets
                    wavFileInfo.fmtSubChunck.numChannels = *(short*)popData(sizeof(short), 1);
                    // read sampleRate
                    wavFileInfo.fmtSubChunck.sampleRate = *(int*)popData(sizeof(int), 1);
                    // read byteRate
                    wavFileInfo.fmtSubChunck.byteRate = *(int*)popData(sizeof(int), 1);
                    // read blockAlign
                    wavFileInfo.fmtSubChunck.blockAlign = *(short*)popData(sizeof(short), 1);
                    // read bitsPerSample
                    wavFileInfo.fmtSubChunck.bitsPerSample = *(short*)popData(sizeof(short), 1);
                    if(wavFileInfo.fmtSubChunck.bitsPerSample == 8 || wavFileInfo.fmtSubChunck.bitsPerSample == 16) {
                        return &wavFileInfo.fmtSubChunck;
                    } throw_error(pull__invalid_fmt_chunck, "Unsopported bits per sample");
                } throw_error(pull__invalid_fmt_chunck, "Unsopported audio format");
            } throw_error(pull__invalid_fmt_chunck, "Unsopported chunck size");
        } throw_error(pull__invalid_fmt_chunck, "Unable to read fmt?-str");
    } throw_error(file__nullptr, "Unable to read file");
    return nullptr;
}

wav::DataSubChunck * wav::getDataChunck(){
    if(file) {
        size_t sample_unit_size = 0;
        // decide the best way to store samples
        switch (wavFileInfo.fmtSubChunck.bitsPerSample) {
            case 8: sample_unit_size = sizeof(uint8_t); break;
            case 16: sample_unit_size = sizeof(int16_t)*wavFileInfo.fmtSubChunck.numChannels; break;
            default: throw_error(pull__invalid_fmt_chunck, "Unsopported bits per sample"); break;
        }
        // read "DATA" and check
        if(fread(wavFileInfo.dataChunck.data, sizeof(char), 4, file) && strncmp(wavFileInfo.dataChunck.data, "DATA", 0) == 0){
            // read chunck size and check
            if ((wavFileInfo.dataChunck.SubChunk2Size = *(int*)popData(sizeof(int), 1))){
                // allocate the necessary memory to store samples
                wavFileInfo.dataChunck.samples = new int[wavFileInfo.dataChunck.SubChunk2Size/sample_unit_size];
                //(void**)malloc((wavFileInfo.dataChunck.SubChunk2Size/sample_unit_size));
                std::cout << "# samples size: " << wavFileInfo.dataChunck.SubChunk2Size << ", readable: " << (wavFileInfo.dataChunck.SubChunk2Size/sample_unit_size) * sample_unit_size << std::endl;
                // read samples
                int *_p = (int*)wavFileInfo.dataChunck.samples;
                for (int i = 0 ; i < (wavFileInfo.dataChunck.SubChunk2Size/sample_unit_size) ; i++){
                    _p[i] = *(int*)popData(sample_unit_size, 1);
//                    std::cout
//                    << (int&)_p[i]
//                    << " (" << i << " / " << (wavFileInfo.dataChunck.SubChunk2Size/sample_unit_size) << ")"<< std::endl;
                } return &wavFileInfo.dataChunck;
            } throw_error(pull__invalid_fmt_chunck, "Invalid chunck size");
        } throw_error(pull__invalid_data_chunck, "Unable to read DATA-str");
    } throw_error(file__nullptr, "Unable to read file");
    return nullptr;
}

wav::ListChunck * wav::getListInfoChunk(){
    if(file){
        // read "LIST" and check
        if(fread(wavFileInfo.listInfo.list , sizeof(char), 4, file) && strncmp(wavFileInfo.listInfo.list, "LIST", 4) == 0){
            // read chunck size and check
            wavFileInfo.listInfo.chunckSize = *(int*)popData(sizeof(int), 1);
            // ignore if not LIST/INFO
            prevNextChunck();
            if(strncmp(wavFileInfo.infoSubChunk.iuknSubChunk.iukn, "INFO", 4) != 0)
                ignoreChunck(wavFileInfo.listInfo.chunckSize);
            return &wavFileInfo.listInfo;
        } throw_error(pull__invalid_data_chunck, "Unable to read LIST-str");
    } throw_error(file__nullptr, "Unable to read file");
    return nullptr;
}

wav::InfoSubChunk * wav::getInfoChunck(){
    if(file){
        // read "INFO" and check
        if(fread(wavFileInfo.infoSubChunk.info , sizeof(char), 4, file) && strncmp(wavFileInfo.infoSubChunk.info, "INFO", 4) == 0){
            return &wavFileInfo.infoSubChunk;
        } throw_error(pull__invalid_data_chunck, "Unable to read INFO-str");
    } throw_error(file__nullptr, "Unable to read file");
    return nullptr;
}

wav::InfoSubChunk::SubChunck * wav::getinfoSubChunck(const char *chunckName, char * chunckNameAddr, InfoSubChunk::SubChunck &chunk){
    //std::cout << "+\tReading: " << chunckName << std::endl;
    if(file){
        // read "${chunckName}" and check
        if(fread(chunckNameAddr, sizeof(char), 4, file) && strncmp(chunckName, chunckNameAddr, 4) == 0){
            // read chunck size and check
            chunk.size = *(int*)popData(sizeof(int), 1);
            //std::cout << "-\tChunck size: " << chunk.size << std::endl;
//            fpos_t test;
//            fgetpos(file, &test);
            if(chunk.size){
                // read chunck data
                fread(chunk.data, sizeof(char), chunk.size, file);
                //std::cout << "chunckData: " << (char*)chunk.data << std::endl;;
                //ignoreChunck(1);
                return &chunk;
            } throw_error(pull__invalid_metadata_chunck, "Invalid chunck size");
        } throw_error(pull__invalid_data_chunck, ("Unable to read " + std::string(chunckName)+"-str").c_str());
    } throw_error(file__nullptr, "Unable to read file");
    return nullptr;
}

bool wav::prevNextChunck(){
    if(file && !feof(file)){
        fgetpos(file, &hpos);
        fread(wavFileInfo.infoSubChunk.iuknSubChunk.iukn, sizeof(char), 4, file);
        if(feof(file)) return false;
        wavFileInfo.infoSubChunk.iuknSubChunk.size = *(int*) popData(sizeof(int), 1);
        fsetpos(file, &hpos);
        return true;
    } return false;
}

void wav::ignoreChunck(size_t size){
    void ** buffer = (void**)calloc(size, 1);
    fread(buffer, 1, size, file);
    free(buffer);
}

wav::wav(){
    file = nullptr;
}

bool wav::push(const char *){
    return true;
}

bool wav::pull(const char * filename){
    if(fopen_rb(filename)){
        bool chunckDescriptorFound (false),
        fmtChunckFound(false),
        dataChunckFound(false);
        char * chunckName = wavFileInfo.infoSubChunk.iuknSubChunk.iukn;
        int * chunkSize = &wavFileInfo.infoSubChunk.iuknSubChunk.size;
        InfoSubChunk * infoSubChunck = &wavFileInfo.infoSubChunk;
        while (prevNextChunck()) {
            
            if(strncmp(chunckName, "RIFF", 4) == 0) {
                getChunckDescriptor();
                chunckDescriptorFound = true;
            } else if(strncmp(chunckName, "fmt", 3) == 0) {
                getFmtSubchunk();
                fmtChunckFound = true;
            } else if(strncmp(chunckName, "data", 4) == 0) {
                getDataChunck();
                dataChunckFound = true;
            }  else if(strncmp(chunckName, "LIST", 4) == 0)
                getListInfoChunk();
            else if(strncmp(chunckName, "INFO", 4) == 0)
                getInfoChunck();
            else if(strncmp(chunckName, "INAM", 4) == 0)
                getinfoSubChunck("INAM", infoSubChunck->inamSubChunk.inam, infoSubChunck->inamSubChunk);
            else if(strncmp(chunckName, "IPRD", 4) == 0)
                getinfoSubChunck("IPRD", infoSubChunck->iprdSubChunk.iprd, infoSubChunck->iprdSubChunk);
            else if(strncmp(chunckName, "IART", 4) == 0)
                getinfoSubChunck("IART", infoSubChunck->iartSubChunk.iart, infoSubChunck->iartSubChunk);
            else if(strncmp(chunckName, "ICRD", 4) == 0)
                getinfoSubChunck("ICRD", infoSubChunck->icrdSubChunk.icrd, infoSubChunck->icrdSubChunk);
            else if(strncmp(chunckName, "ITRK", 4) == 0)
                getinfoSubChunck("ITRK", infoSubChunck->itrkSubChunk.itrk, infoSubChunck->itrkSubChunk);
            else if(strncmp(chunckName, "ICMT", 4) == 0)
            getinfoSubChunck("ICMT", infoSubChunck->itcmtSubChunk.itcmt, infoSubChunck->itcmtSubChunk);
            else if(strncmp(chunckName, "ISFT", 4) == 0)
                getinfoSubChunck("ISFT", infoSubChunck->isftSubChunk.isft, infoSubChunck->isftSubChunk);
            else if(strncmp(chunckName, "IENG", 4) == 0)
            getinfoSubChunck("IENG", infoSubChunck->iengSubChunk.ieng, infoSubChunck->iengSubChunk);
            else if(strncmp(chunckName, "IGNR", 4) == 0)
                getinfoSubChunck("IGNR", infoSubChunck->ignrSubChunk.ignr, infoSubChunck->ignrSubChunk);
            else if(strncmp(chunckName, "ICOP", 4) == 0)
            getinfoSubChunck("ICOP", infoSubChunck->icopSubChunkv.icop, infoSubChunck->icopSubChunkv);
            else if(strncmp(chunckName, "ISBJ", 4) == 0)
                getinfoSubChunck("ISBJ", infoSubChunck->isbjSubChunk.isbj, infoSubChunck->isbjSubChunk);
            else if(strncmp(chunckName, "ISRC", 4) == 0)
            getinfoSubChunck("ISRC", infoSubChunck->isrcSubChunk.isrc, infoSubChunck->isrcSubChunk);
            else {
                if(strlen(chunckName) == 4 && isascii(chunckName[0])) {
                    ignoreChunck((int&)*chunkSize);
                } else ignoreChunck(1);
            }
        } return chunckDescriptorFound && dataChunckFound && fmtChunckFound;
    }
    throw_error(file__nullptr, "Unable to open file");
    return false;
}

wav::~wav(){
    if(file != nullptr)
        fclose(file);
}
