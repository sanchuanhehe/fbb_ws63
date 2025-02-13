#include <stdbool.h>
#include "rtdText.h"
#include "rtdUri.h"
#include "ndef.h"
#include "nfc.h"

/**
 * @brief  写网址链接
 * @note
 * @param  position: 存储的位置
 * @param  *http: 存储的网址链接
 * @retval
 */
bool storeUrihttp(RecordPosEnu position, uint8_t *http)
{
    NDEFDataStr data;

    prepareUrihttp(&data, position, http);
    return NT3HwriteRecord(&data);
}

/**
 * @brief  写文本信息
 * @note
 * @param  position: 存储的位置
 * @param  *text: 存储的内容
 * @retval
 */
bool storeText(RecordPosEnu position, uint8_t *text)
{
    NDEFDataStr data;

    prepareText(&data, position, text);
    return NT3HwriteRecord(&data);
}
