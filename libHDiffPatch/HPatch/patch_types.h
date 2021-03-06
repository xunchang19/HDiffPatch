//  patch_types.h
//
/*
 The MIT License (MIT)
 Copyright (c) 2012-2017 HouSisong
 
 Permission is hereby granted, free of charge, to any person
 obtaining a copy of this software and associated documentation
 files (the "Software"), to deal in the Software without
 restriction, including without limitation the rights to use,
 copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the
 Software is furnished to do so, subject to the following
 conditions:
 
 The above copyright notice and this permission notice shall be
 included in all copies of the Software.
 
 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 OTHER DEALINGS IN THE SOFTWARE.
 */

#ifndef HPatch_patch_types_h
#define HPatch_patch_types_h
#ifdef __cplusplus
extern "C" {
#endif
    
    //hpatch_StreamPos_t for support large file
#ifdef _MSC_VER
    typedef unsigned __int64        hpatch_StreamPos_t;
    #define hpatch_inline _inline
#else
    typedef unsigned long long      hpatch_StreamPos_t;
    #define hpatch_inline inline
#endif
    
    typedef void* hpatch_TStreamInputHandle;
    typedef void* hpatch_TStreamOutputHandle;
    
    typedef struct hpatch_TStreamInput{
        hpatch_TStreamInputHandle streamHandle;
        hpatch_StreamPos_t        streamSize;
        //read() must return (out_data_end-out_data), otherwise error
        long                      (*read) (hpatch_TStreamInputHandle streamHandle,
                                           const hpatch_StreamPos_t readFromPos,
                                           unsigned char* out_data,unsigned char* out_data_end);
    } hpatch_TStreamInput;
    
    typedef struct hpatch_TStreamOutput{
        hpatch_TStreamOutputHandle streamHandle;
        hpatch_StreamPos_t         streamSize;
        //write() must return (out_data_end-out_data), otherwise error
        //   first writeToPos==0; the next writeToPos+=(data_end-data)
        long                       (*write)(hpatch_TStreamOutputHandle streamHandle,
                                            const hpatch_StreamPos_t writeToPos,
                                            const unsigned char* data,const unsigned char* data_end);
    } hpatch_TStreamOutput;
    

    #define hpatch_kMaxCompressTypeLength   256
    
    typedef struct hpatch_compressedDiffInfo{
        hpatch_StreamPos_t  newDataSize;
        hpatch_StreamPos_t  oldDataSize;
        int                 compressedCount;//need open hpatch_decompressHandle number
        char                compressType[hpatch_kMaxCompressTypeLength+1];
    } hpatch_compressedDiffInfo;
    
    typedef void*  hpatch_decompressHandle;
    typedef struct hpatch_TDecompress{
        int                (*is_can_open)(struct hpatch_TDecompress* decompressPlugin,
                                          const hpatch_compressedDiffInfo* compressedDiffInfo);
        //error return 0.
        hpatch_decompressHandle  (*open)(struct hpatch_TDecompress* decompressPlugin,
                                         hpatch_StreamPos_t dataSize,
                                         const struct hpatch_TStreamInput* codeStream,
                                         hpatch_StreamPos_t code_begin,
                                         hpatch_StreamPos_t code_end);//codeSize==code_end-code_begin
        void                     (*close)(struct hpatch_TDecompress* decompressPlugin,
                                          hpatch_decompressHandle decompressHandle);
        //decompress_part() must return (out_part_data_end-out_part_data), otherwise error
        long           (*decompress_part)(const struct hpatch_TDecompress* decompressPlugin,
                                          hpatch_decompressHandle decompressHandle,
                                          unsigned char* out_part_data,unsigned char* out_part_data_end);
    } hpatch_TDecompress;

    
    
    
    void mem_as_hStreamInput(hpatch_TStreamInput* out_stream,
                             const unsigned char* mem,const unsigned char* mem_end);
    void mem_as_hStreamOutput(hpatch_TStreamOutput* out_stream,
                              unsigned char* mem,unsigned char* mem_end);
    
    #define hpatch_BOOL   int
    #define hpatch_FALSE  ((int)0)
    #define hpatch_TRUE   ((int)(!hpatch_FALSE))
    
    #define  hpatch_kMaxPackedUIntBytes ((sizeof(hpatch_StreamPos_t)*8+6)/7+1)
    hpatch_BOOL hpatch_packUIntWithTag(unsigned char** out_code,unsigned char* out_code_end,
                                       hpatch_StreamPos_t uValue,int highTag,const int kTagBit);
    unsigned int hpatch_packUIntWithTag_size(hpatch_StreamPos_t uValue,const int kTagBit);
    #define hpatch_packUInt(out_code,out_code_end,uValue) \
                hpatch_packUIntWithTag(out_code,out_code_end,uValue,0,0)

    hpatch_BOOL hpatch_unpackUIntWithTag(const unsigned char** src_code,const unsigned char* src_code_end,
                                         hpatch_StreamPos_t* result,const unsigned int kTagBit);
    #define hpatch_unpackUInt(src_code,src_code_end,result) \
                hpatch_unpackUIntWithTag(src_code,src_code_end,result,0)

    
    //数据用rle压缩后的包类型2bit
    typedef enum TByteRleType{
        kByteRleType_rle0  = 0,    //00表示后面存的压缩0;    (包中不需要字节数据)
        kByteRleType_rle255= 1,    //01表示后面存的压缩255;  (包中不需要字节数据)
        kByteRleType_rle   = 2,    //10表示后面存的压缩数据;  (包中字节数据只需储存一个字节数据)
        kByteRleType_unrle = 3     //11表示后面存的未压缩数据;(包中连续储存多个字节数据)
    } TByteRleType;
    
    static const int kByteRleType_bit=2;
    
#ifdef __cplusplus
}
#endif
#endif
