//
//  lanau.hpp
//  lanau
//
//  Created by René Descartes Domingos Muala on 10/05/21.
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

#ifndef lanau_hpp
#define lanau_hpp

#include <stdio.h>
#include "encoders/wav/wav.hpp"
#include "decoders/wav/wav.hpp"

/// Landia audio API
class LanauApi {
public:
    
    constexpr static const char * version = "1.0 Demo";
    
    /// Encoders
    struct Encoder {
        Encoder() = delete;
        typedef ewav wav;
    };
    
    /// Decoders
    struct Decoder {
        Decoder() = delete;
        typedef dwav wav;
    };
    
private:
    LanauApi() = delete;
};

#endif /* lanau_hpp */
