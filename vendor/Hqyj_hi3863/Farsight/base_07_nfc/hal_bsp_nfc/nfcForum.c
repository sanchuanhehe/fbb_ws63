#include "nfcForum.h"
#include <string.h>

static void rtdHeader(uint8_t type, NDEFRecordStr *ndefRecord, uint8_t *I2CMsg)
{
    ndefRecord->header |= 1;
    ndefRecord->header |= BIT_SR;
    I2CMsg[HEADER] = ndefRecord->header;

    ndefRecord->typeLength = 1;
    I2CMsg[TYPE_LENGTH] = ndefRecord->typeLength;

    ndefRecord->type.typeCode = type;
    I2CMsg[TYPE_CODE] = ndefRecord->type.typeCode;
}


uint8_t composeRtdText(const NDEFDataStr *ndef, NDEFRecordStr *ndefRecord, uint8_t *I2CMsg)
{
    rtdHeader(RTD_TEXT, ndefRecord, I2CMsg);

    uint8_t payLoadLen = addRtdText(&ndefRecord->type.typePayload.text);
    memcpy_s(&I2CMsg[TYPE_PAYLOAD], payLoadLen, &ndefRecord->type.typePayload.text, payLoadLen);

    ndefRecord->payloadLength = ndef->rtdPayloadlength + payLoadLen; // added the typePayload
    I2CMsg[PALOAD_LENGTH] = ndefRecord->payloadLength;
    return TEXT_RECORD_LEN;
}


uint8_t composeRtdUri(const NDEFDataStr *ndef, NDEFRecordStr *ndefRecord, uint8_t *I2CMsg)
{
    rtdHeader(RTD_URI, ndefRecord, I2CMsg);
    uint8_t payLoadLen = addRtdUriRecord(ndef, &ndefRecord->type.typePayload.uri);
    memcpy_s(&I2CMsg[TYPE_PAYLOAD], payLoadLen, &ndefRecord->type.typePayload.uri, payLoadLen);

    ndefRecord->payloadLength = ndef->rtdPayloadlength + payLoadLen; // added the typePayload
    I2CMsg[PALOAD_LENGTH] = ndefRecord->payloadLength;

    return URI_RECORD_LEN;
}

void composeNDEFMBME(bool isFirstRecord, bool isLastRecord, NDEFRecordStr *ndefRecord)
{
    if (isFirstRecord) {
        ndefRecord->header |= BIT_MB;
    } else {
        ndefRecord->header &= ~MASK_MB;
    }

    if (isLastRecord) {
        ndefRecord->header |= BIT_ME;
    } else {
        ndefRecord->header &= ~MASK_ME;
    }
}
