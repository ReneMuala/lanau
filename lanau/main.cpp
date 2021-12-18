//
//  main.cpp
//  lanau
//
//  Created by René Descartes Domingos Muala on 10/05/21.
//

#include <iostream>
#include "lanau.hpp"

int main(int argc, const char * argv[]) {
   
    // Creating a decoder
    LanauApi::Decoder::wav decoder;
    
    // Creating an encoder to the decoder
    LanauApi::Encoder::wav * encoder = (LanauApi::Encoder::wav*)&decoder;
    
    std::cout << "Decoding music.wav..." << std::endl;
    
    decoder.pull("test.wav");
    
    std::cout << "Genre: " << decoder.get_genre() << std::endl;
    std::cout << "Album Title: " << decoder.get_album_title() << std::endl;
    std::cout << "Artist: " << decoder.get_artist() << std::endl;
    std::cout << "Copyright: " << decoder.get_copyright() << std::endl;
    std::cout << "Date: " << decoder.get_creation_date() << std::endl;
    std::cout << "Engineer: " << decoder.get_engineer() << std::endl;
    std::cout << "Keywords: " << decoder.get_keywords() << std::endl;
    std::cout << "Software: " << decoder.get_software() << std::endl;
    std::cout << "Source: " << decoder.get_source() << std::endl;
    std::cout << "Technician: " << decoder.get_technician() << std::endl;
    std::cout << "Comment: " << decoder.get_text_comment() << std::endl;
    std::cout << "Track Number: " << decoder.get_track_number() << std::endl;
    std::cout << "Track Title: " << decoder.get_track_tittle() << std::endl;
    
    std::cout << "Encoding music2.wav..." << std::endl;
    
    encoder->set_copyright("(c) <https://www.robotplanet.dk/audio/>");
    encoder->set_engineer("[Engineer]");
    encoder->set_keywords("[Keywords]");
    encoder->set_software("[Software]");
    encoder->set_source("[Source]");
    encoder->set_technician("[Technician]");
    
    encoder->push("test.wav");
    
    return 0;
}

