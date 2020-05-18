#include <cmath>
#include <node.h>
#include <node_buffer.h>
#include <v8.h>
#include <stdint.h>
#include <string>
#include <algorithm>
#include "cryptonote_basic/cryptonote_basic.h"
#include "cryptonote_basic/cryptonote_format_utils.h"
#include "cryptonote_basic/blobdatatype.h"
#include "crypto/crypto.h"
#include "crypto/hash.h"
#include "common/base58.h"
#include "serialization/binary_utils.h"
#include "easylogging++.h"

#include <nan.h>

using namespace node;
using namespace v8;
using namespace cryptonote;
using namespace epee;

#define THROW_ERROR_EXCEPTION(x) Nan::ThrowError(x)

blobdata uint64be_to_blob(uint64_t num) {
    blobdata res = "        ";
    res[0] = num >> 56 & 0xff;
    res[1] = num >> 48 & 0xff;
    res[2] = num >> 40 & 0xff;
    res[3] = num >> 32 & 0xff;
    res[4] = num >> 24 & 0xff;
    res[5] = num >> 16 & 0xff;
    res[6] = num >> 8  & 0xff;
    res[7] = num       & 0xff;
    return res;
}

NAN_METHOD(convert_blob) {

    //std::cout << "convert_blob called" <<std::endl;

    if (info.Length() < 1)
        THROW_ERROR_EXCEPTION("You must provide one argument.");

    Local<Object> target = info[0]->ToObject();

    if (!Buffer::HasInstance(target)) return THROW_ERROR_EXCEPTION("Argument should be a buffer object.");

    blobdata input = std::string(Buffer::Data(target), Buffer::Length(target));
    blobdata output = "";

    //convert
    block b = AUTO_VAL_INIT(b);

    std::cout << "Checkpoint convert_blob 2" << std::endl;

    if (!parse_and_validate_block_from_blob(input, b)) {
        std::cout << "Checkpoint convert_blob 2.5 parse_and_validate_block_from_blob FAILED" << std::endl;
        THROW_ERROR_EXCEPTION("Failed to parse block");
        return;
    }

    output = get_block_hashing_blob(b);
    if (output.empty())
        return THROW_ERROR_EXCEPTION("Failed to create mining block");

    v8::Local<v8::Value> returnValue = Nan::CopyBuffer((char*)output.data(), output.size()).ToLocalChecked();
    info.GetReturnValue().Set(
        returnValue
    );

    std::cout << "Checkpoint convert_blob 6" << std::endl;
}

NAN_METHOD(get_block_id) {

    //std::cout << "get_block_id called" <<std::endl;

    if (info.Length() < 1)
        THROW_ERROR_EXCEPTION("You must provide one argument.");

    Local<Object> target = info[0]->ToObject();

    if (!Buffer::HasInstance(target))
        THROW_ERROR_EXCEPTION("Argument should be a buffer object.");

    blobdata input = std::string(Buffer::Data(target), Buffer::Length(target));
    blobdata output = "";

    block b = AUTO_VAL_INIT(b);
    if (!parse_and_validate_block_from_blob(input, b))
        THROW_ERROR_EXCEPTION("Failed to parse block");

    crypto::hash block_id;
    if (!get_block_hash(b, block_id))
        THROW_ERROR_EXCEPTION("Failed to calculate hash for block");

    char* cstr = reinterpret_cast<char*>(&block_id);
    v8::Local<v8::Value> returnValue = Nan::CopyBuffer(cstr, 32).ToLocalChecked();
    info.GetReturnValue().Set(
        returnValue
    );
}

NAN_METHOD(construct_block_blob) {

    //std::cout << "construct_block_blob called" <<std::endl;

    if (info.Length() < 2)
        THROW_ERROR_EXCEPTION("You must provide two arguments.");

    Local<Object> block_template_buf = info[0]->ToObject();
    Local<Object> nonce_buf = info[1]->ToObject();

    if (!Buffer::HasInstance(block_template_buf) || !Buffer::HasInstance(nonce_buf))
        THROW_ERROR_EXCEPTION("Both arguments should be buffer objects.");

    if (Buffer::Length(nonce_buf) != 4)
        THROW_ERROR_EXCEPTION("Nonce buffer has invalid size.");

    uint32_t nonce = *reinterpret_cast<uint32_t*>(Buffer::Data(nonce_buf));

    blobdata block_template_blob = std::string(Buffer::Data(block_template_buf), Buffer::Length(block_template_buf));
    blobdata output = "";

    block b = AUTO_VAL_INIT(b);
    if (!parse_and_validate_block_from_blob(block_template_blob, b))
        THROW_ERROR_EXCEPTION("Failed to parse block");

    b.nonce = nonce;

    if (!block_to_blob(b, output))
        THROW_ERROR_EXCEPTION("Failed to convert block to blob");

    v8::Local<v8::Value> returnValue = Nan::CopyBuffer((char*)output.data(), output.size()).ToLocalChecked();
    info.GetReturnValue().Set(
        returnValue
    );
}

NAN_METHOD(address_decode) {

    //std::cout << "address_decode called" <<std::endl;

    if (info.Length() < 1)
        THROW_ERROR_EXCEPTION("You must provide one argument.");

    Local<Object> target = info[0]->ToObject();

    if (!Buffer::HasInstance(target))
        THROW_ERROR_EXCEPTION("Argument should be a buffer object.");

    blobdata input = std::string(Buffer::Data(target), Buffer::Length(target));

    blobdata data;
    uint64_t prefix;
    if (!tools::base58::decode_addr(input, prefix, data)){
        info.GetReturnValue().Set(Nan::Undefined());
        return;
    }

    account_public_address adr;
    if (!::serialization::parse_binary(data, adr)) {
        info.GetReturnValue().Set(Nan::Undefined());
        return;
    }

    if (!crypto::check_key(adr.m_spend_public_key) || !crypto::check_key(adr.m_view_public_key)) {
        info.GetReturnValue().Set(Nan::Undefined());
        return;
    }

    info.GetReturnValue().Set(Nan::New(static_cast<uint32_t>(prefix)));
}


NAN_MODULE_INIT(init) {
    Nan::Set(target, Nan::New("construct_block_blob").ToLocalChecked(), Nan::GetFunction(Nan::New<FunctionTemplate>(construct_block_blob)).ToLocalChecked());
    Nan::Set(target, Nan::New("get_block_id").ToLocalChecked(), Nan::GetFunction(Nan::New<FunctionTemplate>(get_block_id)).ToLocalChecked());
    Nan::Set(target, Nan::New("convert_blob").ToLocalChecked(), Nan::GetFunction(Nan::New<FunctionTemplate>(convert_blob)).ToLocalChecked());
    Nan::Set(target, Nan::New("address_decode").ToLocalChecked(), Nan::GetFunction(Nan::New<FunctionTemplate>(address_decode)).ToLocalChecked());
}

NODE_MODULE(cryptonote, init)
